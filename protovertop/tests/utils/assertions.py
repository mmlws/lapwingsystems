import numpy as np


def assert_c0_continuity(result, tol=1e-6):
    """Verify C0 continuity at segment junctions

    Checks that elevation is continuous at all interior stations
    """
    stations = result.stations
    a0, a1, a2 = result.a0, result.a1, result.a2

    for i in range(1, result.num_stations - 1):
        s = stations[i]

        # Left segment evaluation at junction
        seg_left = i - 1
        ds_left = s - stations[seg_left]
        elev_left = a0[seg_left] + a1[seg_left] * ds_left + a2[seg_left] * ds_left**2

        # Right segment evaluation at junction (ds=0)
        seg_right = i
        elev_right = a0[seg_right]

        assert abs(elev_left - elev_right) < tol, \
            f"C0 discontinuity at station {i} (s={s:.1f}m): {elev_left:.6f} != {elev_right:.6f}"


def assert_c1_continuity(result, tol=1e-6):
    """Verify C1 continuity (gradient continuous) at segment junctions"""
    stations = result.stations
    a1, a2 = result.a1, result.a2

    for i in range(1, result.num_stations - 1):
        s = stations[i]

        # Left segment gradient at junction
        seg_left = i - 1
        ds_left = s - stations[seg_left]
        grad_left = a1[seg_left] + 2 * a2[seg_left] * ds_left

        # Right segment gradient at junction (ds=0)
        seg_right = i
        grad_right = a1[seg_right]

        assert abs(grad_left - grad_right) < tol, \
            f"C1 discontinuity at station {i} (s={s:.1f}m): grad {grad_left:.6f} != {grad_right:.6f}"


def assert_gradient_bounds(result, g_min, g_max, tol=1e-6):
    """Verify gradient constraints satisfied at all stations"""
    stations = result.stations
    a1, a2 = result.a1, result.a2

    for i in range(result.num_stations):
        s = stations[i]
        seg = min(i, result.num_segments - 1)
        ds = s - stations[seg]

        gradient = a1[seg] + 2 * a2[seg] * ds

        assert gradient >= g_min - tol, \
            f"Gradient below minimum at station {i} (s={s:.1f}m): {gradient:.6f} < {g_min}"
        assert gradient <= g_max + tol, \
            f"Gradient above maximum at station {i} (s={s:.1f}m): {gradient:.6f} > {g_max}"


def assert_curvature_bounds(result, k_min_crest, k_min_sag, tol=1e-6):
    """Verify vertical curvature constraints K >= K_min"""
    a2 = result.a2

    # Convert K_min to a2 bounds: a2 = ±1/(200*K)
    lb = -1.0 / (200.0 * k_min_crest)  # Crest curves
    ub = 1.0 / (200.0 * k_min_sag)     # Sag curves

    for i in range(result.num_segments):
        assert a2[i] >= lb - tol, \
            f"Curvature too tight (crest) at segment {i}: a2={a2[i]:.6e} < {lb:.6e}"
        assert a2[i] <= ub + tol, \
            f"Curvature too tight (sag) at segment {i}: a2={a2[i]:.6e} > {ub:.6e}"


def assert_flow_conservation(result, tol=1e-4):
    """Verify flow conservation at all stations"""
    ft_p, ft_n = result.ft_p, result.ft_n
    fl_p, fl_n = result.fl_p, result.fl_n
    fu_p, fu_n = result.fu_p, result.fu_n
    fb_p, fb_n = result.fb_p, result.fb_n
    fw_p, fw_n = result.fw_p, result.fw_n

    for i in range(result.num_stations):
        # Positive flow balance
        if i == 0:
            inflow_p = fl_p[i] + fb_p[i]
            outflow_p = ft_p[i] + fu_p[i] + fw_p[i]
        elif i == result.num_stations - 1:
            inflow_p = ft_p[i-1] + fl_p[i] + fb_p[i]
            outflow_p = fu_p[i] + fw_p[i]
        else:
            inflow_p = ft_p[i-1] + fl_p[i] + fb_p[i]
            outflow_p = ft_p[i] + fu_p[i] + fw_p[i]

        balance_p = inflow_p - outflow_p
        assert abs(balance_p) < tol, \
            f"Positive flow imbalance at station {i}: {balance_p:.6e}"

        # Negative flow balance
        if i == 0:
            inflow_n = ft_n[i] + fl_n[i] + fb_n[i]
            outflow_n = fu_n[i] + fw_n[i]
        elif i == result.num_stations - 1:
            inflow_n = fl_n[i] + fb_n[i]
            outflow_n = fu_n[i] + fw_n[i] + ft_n[i-1]
        else:
            inflow_n = ft_n[i] + fl_n[i] + fb_n[i]
            outflow_n = fu_n[i] + fw_n[i] + ft_n[i-1]

        balance_n = inflow_n - outflow_n
        assert abs(balance_n) < tol, \
            f"Negative flow imbalance at station {i}: {balance_n:.6e}"


def assert_section_balance(result, tol=1e-4):
    """Verify section balance: v_cut = fl_p + fl_n, v_fill = fu_p + fu_n"""
    for i in range(result.num_stations):
        # Cut balance
        cut_balance = result.v_cut[i] - (result.fl_p[i] + result.fl_n[i])
        assert abs(cut_balance) < tol, \
            f"Cut section imbalance at station {i}: {cut_balance:.6e}"

        # Fill balance
        fill_balance = result.v_fill[i] - (result.fu_p[i] + result.fu_n[i])
        assert abs(fill_balance) < tol, \
            f"Fill section imbalance at station {i}: {fill_balance:.6e}"


def assert_offset_bounds(result, min_offset, max_offset, tol=1e-6):
    """Verify offset stays within bounds"""
    for i in range(result.num_stations):
        assert result.u[i] >= min_offset - tol, \
            f"Offset below minimum at station {i}: {result.u[i]:.6f} < {min_offset}"
        assert result.u[i] <= max_offset + tol, \
            f"Offset above maximum at station {i}: {result.u[i]:.6f} > {max_offset}"


def assert_volumes_nonnegative(result, tol=1e-6):
    """Verify all volumes and flows are non-negative"""
    assert np.all(result.v_cut >= -tol), "Negative cut volumes found"
    assert np.all(result.v_fill >= -tol), "Negative fill volumes found"
    assert np.all(result.ft_p >= -tol), "Negative transit flows (positive) found"
    assert np.all(result.ft_n >= -tol), "Negative transit flows (negative) found"
    assert np.all(result.fl_p >= -tol), "Negative loading flows (positive) found"
    assert np.all(result.fl_n >= -tol), "Negative loading flows (negative) found"
    assert np.all(result.fu_p >= -tol), "Negative unloading flows (positive) found"
    assert np.all(result.fu_n >= -tol), "Negative unloading flows (negative) found"
