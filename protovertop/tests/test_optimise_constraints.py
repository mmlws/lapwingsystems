import pytest
import numpy as np
from optimise import optimise
from tests.utils.assertions import (
    assert_c0_continuity,
    assert_c1_continuity,
    assert_gradient_bounds,
    assert_curvature_bounds,
    assert_flow_conservation,
    assert_section_balance,
    assert_offset_bounds,
    assert_volumes_nonnegative
)
from tests.utils.fixtures import create_linear_ground, create_parabolic_ground, create_uniform_vp


@pytest.mark.constraints
def test_c0_continuity_flat_ground(flat_ground, simple_vp, default_problem):
    """Verify C0 (elevation) continuity for flat terrain"""
    result = optimise(flat_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_c0_continuity(result, tol=1e-6)


@pytest.mark.constraints
def test_c0_continuity_sloped_ground(sloped_ground, simple_vp, default_problem):
    """Verify C0 (elevation) continuity for constant slope"""
    result = optimise(sloped_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_c0_continuity(result, tol=1e-6)


@pytest.mark.constraints
def test_c0_continuity_complex_terrain(sinusoidal_ground, simple_vp, default_problem):
    """Verify C0 (elevation) continuity for complex rolling terrain"""
    result = optimise(sinusoidal_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_c0_continuity(result, tol=1e-6)


@pytest.mark.constraints
def test_c1_continuity_flat_ground(flat_ground, simple_vp, default_problem):
    """Verify C1 (gradient) continuity for flat terrain"""
    result = optimise(flat_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_c1_continuity(result, tol=1e-6)


@pytest.mark.constraints
def test_c1_continuity_sloped_ground(sloped_ground, simple_vp, default_problem):
    """Verify C1 (gradient) continuity for constant slope"""
    result = optimise(sloped_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_c1_continuity(result, tol=1e-6)


@pytest.mark.constraints
def test_c1_continuity_complex_terrain(sinusoidal_ground, simple_vp, default_problem):
    """Verify C1 (gradient) continuity for complex rolling terrain"""
    result = optimise(sinusoidal_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_c1_continuity(result, tol=1e-6)


@pytest.mark.constraints
def test_gradient_bounds_default_problem(sinusoidal_ground, simple_vp, default_problem):
    """Verify gradient stays within [-10%, +10%] for default problem"""
    result = optimise(sinusoidal_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_gradient_bounds(result, default_problem.g_min, default_problem.g_max)


@pytest.mark.constraints
def test_gradient_bounds_highway(sinusoidal_ground, simple_vp, highway_problem):
    """Verify gradient stays within highway limits (±5%)"""
    result = optimise(sinusoidal_ground, simple_vp, highway_problem)
    assert result.success, "Optimization failed"
    assert_gradient_bounds(result, highway_problem.g_min, highway_problem.g_max)


@pytest.mark.constraints
def test_gradient_bounds_local_road(sinusoidal_ground, simple_vp, local_road_problem):
    """Verify gradient stays within local road limits (±12%)"""
    result = optimise(sinusoidal_ground, simple_vp, local_road_problem)
    assert result.success, "Optimization failed"
    assert_gradient_bounds(result, local_road_problem.g_min, local_road_problem.g_max)


@pytest.mark.constraints
def test_gradient_bounds_conservative(sinusoidal_ground, simple_vp, conservative_problem):
    """Verify gradient stays within conservative limits (±6%)"""
    result = optimise(sinusoidal_ground, simple_vp, conservative_problem)
    assert result.success, "Optimization failed"
    assert_gradient_bounds(result, conservative_problem.g_min, conservative_problem.g_max)


@pytest.mark.constraints
def test_curvature_bounds_default_problem(sinusoidal_ground, simple_vp, default_problem):
    """Verify vertical curvature satisfies K >= 5 for default problem"""
    result = optimise(sinusoidal_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_curvature_bounds(result, default_problem.k_min_crest, default_problem.k_min_sag)


@pytest.mark.constraints
def test_curvature_bounds_highway(sinusoidal_ground, simple_vp, highway_problem):
    """Verify vertical curvature satisfies highway K-values (K_crest=84, K_sag=55)"""
    result = optimise(sinusoidal_ground, simple_vp, highway_problem)
    assert result.success, "Optimization failed"
    assert_curvature_bounds(result, highway_problem.k_min_crest, highway_problem.k_min_sag)


@pytest.mark.constraints
def test_curvature_bounds_aggressive(sinusoidal_ground, simple_vp, aggressive_problem):
    """Verify vertical curvature satisfies aggressive K-values (K=3)"""
    result = optimise(sinusoidal_ground, simple_vp, aggressive_problem)
    assert result.success, "Optimization failed"
    assert_curvature_bounds(result, aggressive_problem.k_min_crest, aggressive_problem.k_min_sag)


@pytest.mark.constraints
def test_flow_conservation_flat_ground(flat_ground, simple_vp, default_problem):
    """Verify flow conservation at all stations for flat terrain"""
    result = optimise(flat_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_flow_conservation(result)


@pytest.mark.constraints
def test_flow_conservation_sloped_ground(sloped_ground, simple_vp, default_problem):
    """Verify flow conservation at all stations for sloped terrain"""
    result = optimise(sloped_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_flow_conservation(result)


@pytest.mark.constraints
def test_flow_conservation_step_ground(step_ground, simple_vp, default_problem):
    """Verify flow conservation at all stations for step change"""
    result = optimise(step_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_flow_conservation(result)


@pytest.mark.constraints
def test_flow_conservation_complex_terrain(sinusoidal_ground, simple_vp, default_problem):
    """Verify flow conservation at all stations for complex terrain"""
    result = optimise(sinusoidal_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_flow_conservation(result)


@pytest.mark.constraints
def test_section_balance_flat_ground(flat_ground, simple_vp, default_problem):
    """Verify section balance: v_cut = loads, v_fill = unloads for flat terrain"""
    result = optimise(flat_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_section_balance(result)


@pytest.mark.constraints
def test_section_balance_sloped_ground(sloped_ground, simple_vp, default_problem):
    """Verify section balance for sloped terrain"""
    result = optimise(sloped_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_section_balance(result)


@pytest.mark.constraints
def test_section_balance_complex_terrain(sinusoidal_ground, simple_vp, default_problem):
    """Verify section balance for complex terrain"""
    result = optimise(sinusoidal_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_section_balance(result)


@pytest.mark.constraints
def test_offset_bounds_default(sinusoidal_ground, simple_vp, default_problem):
    """Verify offset stays within default bounds [-4m, +4m]"""
    result = optimise(sinusoidal_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_offset_bounds(result, default_problem.min_offset, default_problem.max_offset)


@pytest.mark.constraints
def test_offset_bounds_highway(sinusoidal_ground, simple_vp, highway_problem):
    """Verify offset stays within highway bounds [-10m, +10m]"""
    result = optimise(sinusoidal_ground, simple_vp, highway_problem)
    assert result.success, "Optimization failed"
    assert_offset_bounds(result, highway_problem.min_offset, highway_problem.max_offset)


@pytest.mark.constraints
def test_offset_bounds_conservative(sinusoidal_ground, simple_vp, conservative_problem):
    """Verify offset stays within conservative bounds [-3m, +3m]"""
    result = optimise(sinusoidal_ground, simple_vp, conservative_problem)
    assert result.success, "Optimization failed"
    assert_offset_bounds(result, conservative_problem.min_offset, conservative_problem.max_offset)


@pytest.mark.constraints
def test_volumes_nonnegative_flat_ground(flat_ground, simple_vp, default_problem):
    """Verify all volumes and flows are non-negative for flat terrain"""
    result = optimise(flat_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_volumes_nonnegative(result)


@pytest.mark.constraints
def test_volumes_nonnegative_sloped_ground(sloped_ground, simple_vp, default_problem):
    """Verify all volumes and flows are non-negative for sloped terrain"""
    result = optimise(sloped_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_volumes_nonnegative(result)


@pytest.mark.constraints
def test_volumes_nonnegative_step_ground(step_ground, simple_vp, default_problem):
    """Verify all volumes and flows are non-negative for step change"""
    result = optimise(step_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_volumes_nonnegative(result)


@pytest.mark.constraints
def test_volumes_nonnegative_complex_terrain(sinusoidal_ground, simple_vp, default_problem):
    """Verify all volumes and flows are non-negative for complex terrain"""
    result = optimise(sinusoidal_ground, simple_vp, default_problem)
    assert result.success, "Optimization failed"
    assert_volumes_nonnegative(result)


@pytest.mark.constraints
def test_all_constraints_combined_default(default_problem):
    """Verify all constraints simultaneously for challenging terrain with default parameters"""
    # Create challenging terrain: valley followed by hill
    gp = create_parabolic_ground(vertex_elev=95.0, length=300.0, num_points=31)
    vp = create_uniform_vp(300.0, 5)

    result = optimise(gp, vp, default_problem)
    assert result.success, "Optimization failed"

    # Check all constraints
    assert_c0_continuity(result)
    assert_c1_continuity(result)
    assert_gradient_bounds(result, default_problem.g_min, default_problem.g_max)
    assert_curvature_bounds(result, default_problem.k_min_crest, default_problem.k_min_sag)
    assert_flow_conservation(result)
    assert_section_balance(result)
    assert_offset_bounds(result, default_problem.min_offset, default_problem.max_offset)
    assert_volumes_nonnegative(result)


@pytest.mark.constraints
def test_all_constraints_combined_highway(highway_problem):
    """Verify all constraints simultaneously with highway parameters"""
    gp = create_parabolic_ground(vertex_elev=110.0, length=500.0, num_points=51)
    vp = create_uniform_vp(500.0, 8)

    result = optimise(gp, vp, highway_problem)
    assert result.success, "Optimization failed"

    # Check all constraints
    assert_c0_continuity(result)
    assert_c1_continuity(result)
    assert_gradient_bounds(result, highway_problem.g_min, highway_problem.g_max)
    assert_curvature_bounds(result, highway_problem.k_min_crest, highway_problem.k_min_sag)
    assert_flow_conservation(result)
    assert_section_balance(result)
    assert_offset_bounds(result, highway_problem.min_offset, highway_problem.max_offset)
    assert_volumes_nonnegative(result)


@pytest.mark.constraints
def test_all_constraints_combined_aggressive(aggressive_problem):
    """Verify all constraints simultaneously with aggressive parameters"""
    # Steep terrain to push aggressive limits
    gp = create_linear_ground(100.0, 145.0, 300.0, num_points=31)
    vp = create_uniform_vp(300.0, 5)

    result = optimise(gp, vp, aggressive_problem)
    assert result.success, "Optimization failed"

    # Check all constraints
    assert_c0_continuity(result)
    assert_c1_continuity(result)
    assert_gradient_bounds(result, aggressive_problem.g_min, aggressive_problem.g_max)
    assert_curvature_bounds(result, aggressive_problem.k_min_crest, aggressive_problem.k_min_sag)
    assert_flow_conservation(result)
    assert_section_balance(result)
    assert_offset_bounds(result, aggressive_problem.min_offset, aggressive_problem.max_offset)
    assert_volumes_nonnegative(result)
