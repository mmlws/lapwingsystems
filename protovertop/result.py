from dataclasses import dataclass
from datetime import timedelta

import numpy as np


@dataclass
class OptimizationResult:
    """Results from vertical alignment optimization"""
    # Optimization status
    success: bool
    termination_type: int
    inner_iteration_count: int
    outer_iteration_count: int
    lagrange_bc: np.ndarray
    lagrange_lc: np.ndarray
    lagrange_qc: np.ndarray
    objective_value: float

    setup_time: timedelta
    solve_time: timedelta

    # Vertical profile coefficients
    a0: np.ndarray  # Constant coefficients [num_segments]
    a1: np.ndarray  # Linear coefficients [num_segments]
    a2: np.ndarray  # Quadratic coefficients [num_segments]

    # Offsets and volumes at stations
    u: np.ndarray         # Offset from ground [num_stations]
    v_cut: np.ndarray     # Cut volume [num_stations]
    v_fill: np.ndarray    # Fill volume [num_stations]
    u_cut: np.ndarray     # Cut slack variables [num_stations]
    u_fill: np.ndarray    # Fill slack variables [num_stations]

    # Flow variables
    ft_p: np.ndarray      # Positive transit flows [num_segments]
    ft_n: np.ndarray      # Negative transit flows [num_segments]
    fl_p: np.ndarray      # Positive loading flows [num_stations]
    fl_n: np.ndarray      # Negative loading flows [num_stations]
    fu_p: np.ndarray      # Positive unloading flows [num_stations]
    fu_n: np.ndarray      # Negative unloading flows [num_stations]
    fb_p: np.ndarray      # Positive borrow flows [num_stations]
    fb_n: np.ndarray      # Negative borrow flows [num_stations]
    fw_p: np.ndarray      # Positive waste flows [num_stations]
    fw_n: np.ndarray      # Negative waste flows [num_stations]

    # Total cost breakdown
    total_cost: float
    cut_cost: float
    fill_cost: float
    haul_cost: float
    borrow_cost: float
    waste_cost: float

    # Metadata
    num_segments: int
    num_stations: int
    stations: np.ndarray
