import numpy as np
import xalglib as alg

from areafit import areafit_cut, areafit_fill
from crosssection import xs_cut_areas, xs_fill_areas
from groundprofile import GroundProfile, gp_elevation_at
from vertprofile import VerticalProfile, vp_num_segments, vp_segment_lengths, vp_num_stations, vp_stations


def optimise(gp: GroundProfile, vp: VerticalProfile):

    cost_cut = 12.0     # $/m3
    cost_fill = 10.0    # $/m3
    cost_load = 5.0     # $/m3
    cost_haul = 5.0     # $/m
    cost_borrow = 15.0  # $/m3
    cost_waste = 8.0    # $/m3

    min_offset = -4.0
    max_offset = 4.0

    g_min = -0.1
    g_max = 0.1

    k_sag = 5
    k_crest = 5

    # waste/borrow pits
    borrow_capacity = 0.0
    waste_capacity = 0.0

    d = vp_segment_lengths(vp)
    stations = vp_stations(vp)
    num_segments = vp_num_segments(vp)
    num_stations = vp_num_stations(vp)

    num_vars = 0
    def create_variables(count):
        global num_vars
        variables = np.arange(count) + num_vars
        num_vars += count
        return variables

    # vertical profile coefficients
    a0 = create_variables(num_segments)
    a1 = create_variables(num_segments)
    a2 = create_variables(num_segments)

    # volume variables
    v_cut = create_variables(num_stations)
    v_fill = create_variables(num_stations)

    # ground -> vertical profile offset variables
    u = create_variables(num_stations)

    # slack variables to model hockey-stick constraint for area as a function of offset
    u_fill = create_variables(num_stations)
    u_cut = create_variables(num_stations)

    # transit flows
    ft_p = create_variables(num_segments)
    ft_n = create_variables(num_segments)

    # loading flows (load from section)
    fl_p = create_variables(num_stations)
    fl_n = create_variables(num_stations)

    # unloading flows (unload into section)
    fu_p = create_variables(num_stations)
    fu_n = create_variables(num_stations)

    # borrow flows
    fb_p = create_variables(num_stations)
    fb_n = create_variables(num_stations)

    # waste flows
    fw_p = create_variables(num_stations)
    fw_n = create_variables(num_stations)

    # all variables
    opt_state = alg.minqpcreate(num_vars)
    alg.minqpsetalgosparsegenipm(opt_state, 0.0)

    # Objective Function
    # sum (p + y) Vcut_j + q Vfill_j + c d_j (ft+_j + ft-_j)
    # Not included in prototype: Dead haul costs for borrow/waste pits
    cost_vec = np.zeros(num_vars)
    cost_vec[v_cut] = cost_cut+cost_load
    cost_vec[v_fill] = cost_fill
    cost_vec[ft_p] = cost_haul*d
    cost_vec[ft_n] = cost_haul*d
    cost_vec[fw_p] = cost_waste
    cost_vec[fw_n] = cost_waste
    cost_vec[fb_p] = cost_borrow
    cost_vec[fb_n] = cost_borrow
    alg.minqpsetlinearterm(opt_state, cost_vec)

    # Box constraints - flow variables (waste, borrow, load, unload at stations)
    # fb, fw, fl, fu > 0
    for i in range(0, num_stations):
        # by default set all waste and borrow flows equal to zero
        alg.minqpsetbci(opt_state, fb_n[i], 0.0, 0.0)
        alg.minqpsetbci(opt_state, fb_p[i], 0.0, 0.0)
        alg.minqpsetbci(opt_state, fw_n[i], 0.0, 0.0)
        alg.minqpsetbci(opt_state, fw_p[i], 0.0, 0.0)
        # load, unload flow
        alg.minqpsetbci(opt_state, fu_p[i], 0.0, np.inf)
        alg.minqpsetbci(opt_state, fu_n[i], 0.0, np.inf)
        alg.minqpsetbci(opt_state, fl_p[i], 0.0, np.inf)
        alg.minqpsetbci(opt_state, fl_n[i], 0.0, np.inf)

    # Box constraints - flow variables (transit on segments)
    # ft > 0
    for i in range(0, num_segments):
        alg.minqpsetbci(opt_state, ft_p[i], 0.0, np.inf)
        alg.minqpsetbci(opt_state, ft_n[i], 0.0, np.inf)

    # Box constraints - volumes per station
    # V > 0
    for i in range(0, num_stations):
        alg.minqpsetbci(opt_state, v_cut[i], 0.0, np.inf)
        alg.minqpsetbci(opt_state, v_fill[i], 0.0, np.inf)

    # Box constraints - offset
    # min_offset <= u <= max_offset
    for i in range(0, num_stations):
        alg.minqpsetbci(opt_state, u[i], min_offset, max_offset)

    # C0, C1 continuity constraints between segments
    for i in range(1, num_stations-1):
        # ds = s[i]-s[i-1]

        # C0
        # a0[i-1] + ds*a1[i-1] + ds*ds*a2[i-1] - a0[i] = 0
        ds = stations[i] - stations[i-1]
        coeffs = [1, ds, ds*ds, -1]
        indices = [a0[i-1], a1[i-1], a2[i-1], a0[i]]
        alg.minqpaddlc2(opt_state, indices, coeffs, 4, 0.0, 0.0)

        # C1
        # a1[i-1] + 2*ds*a2[i-1] - a1[i] = 0
        coeffs = [1, 2*ds, -1]
        indices = [a1[i-1], a2[i-1], a1[i]]
        alg.minqpaddlc2(opt_state, indices, coeffs, 3, 0.0, 0.0)

    # gradient constraints
    for i in range(0, num_stations):
        s = stations[i]
        seg = min(i, num_segments - 1)
        ds = s - stations[seg]
        indices = [a1[seg], a2[seg]]
        coeffs = [1, 2*ds]
        alg.minqpaddlc2(opt_state, indices, coeffs, 2, g_min, g_max)

    # curvature constraints
    for i in range(0, num_stations):
        lb = -1/(200*k_crest)
        ub = 1/(200*k_sag)
        alg.minqpsetbci(opt_state, [a2[i]], lb, ub)

    # gap equality constraints
    # P(s_i) - Z_i = u_i
    # a2*s^2 + a1*s + a0 - u_i = Zi
    for i in range(0, num_stations):
        s = stations[i]
        z = gp_elevation_at(gp, s)
        seg = min(i, num_segments-1)
        indices = [a0[seg], a1[seg], a2[seg], u[i]]
        ds = s - stations[seg]
        values = [1, ds, ds*ds, -1]
        rhs = z
        alg.minqpaddlc2(opt_state, indices, values, 4, rhs, rhs)

    # flow equality constraints
    for i in range(0, num_stations):
        # positive and negative flow paths
        # adjust if we are at first or last nodes
        if i == 0:
            indices = [fl_p[i], fb_p[i],
                       ft_p[i], fu_p[i], fw_p[i]]
            coeffs = [1, 1,
                      -1, -1, -1]
            alg.minqpaddlc2(opt_state, indices, coeffs, len(indices), 0.0, 0.0)

            indices = [ft_n[i], fl_n[i], fb_n[i],fu_n[i], fw_n[i]]
            coeffs = [1, 1, 1,-1, -1]
            alg.minqpaddlc2(opt_state, indices, coeffs, len(indices), 0.0, 0.0)

        elif i == num_stations - 1:
            indices = [ft_p[i - 1], fl_p[i], fb_p[i],
                       fu_p[i], fw_p[i]]
            coeffs = [1, 1, 1,
                      -1, -1]
            alg.minqpaddlc2(opt_state, indices, coeffs, len(indices), 0.0, 0.0)

            indices = [fl_n[i], fb_n[i],
                       fu_n[i], fw_n[i], ft_n[i - 1]]
            coeffs = [1, 1,
                      -1, -1, -1]
            alg.minqpaddlc2(opt_state, indices, coeffs, len(indices), 0.0, 0.0)
        else:
            indices = [ft_p[i - 1], fl_p[i], fb_p[i],
                       fu_p[i], fw_p[i], ft_p[i]]
            coeffs = [1, 1, 1,
                      -1, -1, -1]
            alg.minqpaddlc2(opt_state, indices, coeffs, len(indices), 0.0, 0.0)

            indices = [ft_n[i], fl_n[i], fb_n[i],
                       fu_n[i], fw_n[i], ft_n[i - 1]]
            coeffs = [1, 1, 1,
                      -1, -1, -1]
            alg.minqpaddlc2(opt_state, indices, coeffs, len(indices), 0.0, 0.0)

    # section balance constraints
    # v_fill = fu_p + fu_n
    # v_cut = fl_p + fl_n
    for i in range(0, num_stations):
        indices = [fu_p[i], fu_n[i], v_fill[i]]
        coeffs = [1, 1, -1]
        alg.minqpaddlc2(opt_state, indices, coeffs, len(indices), 0.0, 0.0)

        indices = [fl_p[i], fl_n[i], v_cut[i]]
        coeffs = [1, 1, -1]
        alg.minqpaddlc2(opt_state, indices, coeffs, len(indices), 0.0, 0.0)

    # scaling factors for each station
    v_scale_factors = []

    # cut volume constraints
    # - fit offset vs cut area to cross-section samples
    # - transform area fit to volume fit using station spacing
    # - add offset slack variable box and linear constraint relating to offset
    # - add quadratic volume constraint
    num_samples = 20
    sample_offsets = np.linspace(min_offset, max_offset, num_samples)
    for i in range(0, num_stations):
        s = stations[i]
        areas = xs_cut_areas(vp, gp, s, sample_offsets)
        pos_coeffs, u_offset, rms = areafit_cut(sample_offsets, areas)

        next_station = stations[i+1] if i < num_stations-1 else s
        prev_station = stations[i-1] if i > 0 else s
        length = (next_station - prev_station)/2.0
        pos_coeffs *= length    # adjust area fit to get volume fit

        c, b, a = pos_coeffs
        assert(a > 0)

        # slack variable constraint
        # u_cut <= u
        # u_cut - u <= 0
        alg.minqpaddlc2(opt_state, [u_cut[i], u[i]], [1, -1], 2, -np.inf, 0)
        # u_cut <= u_offset (where area is = zero)
        alg.minqpsetbci(opt_state, u_cut[i], min_offset, u_offset)

        # quadratic volume constraint
        # v_cut - a*uc^2 - b*uc >= c
        # [uc]^T [-a] [uc] + [-b 1]^T [uc v_cut] >= c
        # V_fill >= 0
        q_rows = [u_cut[i]]
        q_cols = [u_cut[i]]
        q_vals = [-a]
        b_idx = [u_cut[i], v_cut[i]]
        b_vals = [-b, 1.0]
        alg.minqpaddqc2list(opt_state, q_rows, q_cols, q_vals, 1, True,  b_idx, b_vals, 2, c, 0, False)

        k = max(abs(2*a*min_offset + b), abs(2*a*max_offset + b))
        v_scale_factors.append(k)


    # fill volume constraints
    # - fit offset vs fill area to cross-section samples
    # - transform area fit to volume fit using station spacing
    # - add offset slack variable box and linear constraint relating to offset
    # - add quadratic volume constraint
    for i in range(0, num_stations):
        s = stations[i]
        areas = xs_fill_areas(vp, gp, s, sample_offsets)
        pos_coeffs, u_offset, rms = areafit_fill(sample_offsets, areas)

        next_station = stations[i + 1] if i < num_stations - 1 else s
        prev_station = stations[i - 1] if i > 0 else s
        length = (next_station - prev_station)/2.0
        pos_coeffs *= length  # adjust area fit to volume fit

        c, b, a = pos_coeffs
        assert (a > 0)

        # slack variable constraint
        #  u_fill => u
        #  u_fill - u >= 0
        alg.minqpaddlc2(opt_state, [u_fill[i], u[i]], [1, -1], 2, 0, np.inf)
        # u_fill >= u_offset (where area is = zero)
        alg.minqpsetbci(opt_state, u_fill[i], u_offset, max_offset)

        # quadratic volume constraint
        # v_fill - a*uf^2 - b*uf >= c
        # [uf]^T [-a] [uf] + [-b 1]^T [uf v_fill] >= c
        # V_fill >= 0
        q_rows = [u_fill[i]]
        q_cols = [u_fill[i]]
        q_vals = [-a]
        b_idx = [u_fill[i], v_fill[i]]
        b_vals = [-b, 1.0]
        alg.minqpaddqc2list(opt_state, q_rows, q_cols, q_vals, 1, True, b_idx, b_vals, 2, c, 0, False)

        k = max(abs(2*a*min_offset + b), abs(2*a*max_offset + b))
        v_scale_factors.append(k)

    # Borrow and waste pit capacity constraints
    for i in range(num_stations):
        alg.minqpaddlc2(opt_state, [fb_p[i], fb_n[i]], [1, 1], 2, 0, borrow_capacity)
        alg.minqpaddlc2(opt_state, [fw_p[i], fw_n[i]], [1, 1], 2, 0, waste_capacity)

    # setup variable scaling (default 1)
    scaling = np.ones(num_vars)

    mean_ds = np.mean(np.diff(stations))
    scaling[a1] = 1/mean_ds
    scaling[a2] = 1/(mean_ds*mean_ds)

    v_scale_factors = np.array(v_scale_factors)
    v_scale_factors = np.clip(v_scale_factors, 1.0, 1e10)
    v_scale_mean = np.sqrt(np.mean(v_scale_factors**2))

    for i in range(num_stations):
        scaling[v_cut[i]] = v_scale_mean
        scaling[v_fill[i]] = v_scale_mean

        scaling[ft_p[i]] = v_scale_mean
        scaling[ft_n[i]] = v_scale_mean

        scaling[fu_p[i]] = v_scale_mean
        scaling[fu_n[i]] = v_scale_mean
        scaling[fl_p[i]] = v_scale_mean
        scaling[fl_n[i]] = v_scale_mean

        scaling[fb_p[i]] = v_scale_mean
        scaling[fb_n[i]] = v_scale_mean

        scaling[fw_p[i]] = v_scale_mean
        scaling[fw_n[i]] = v_scale_mean

    alg.minqpsetscale(opt_state, scaling)