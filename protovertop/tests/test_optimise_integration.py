import pytest
import numpy as np
from pathlib import Path
from optimise import optimise
from groundprofile import gp_create
from vertprofile import vp_create
from tests.utils.plotting import plot_profile_elevation, plot_cutfill_diagram, plot_gradients, plot_all_results
from tests.utils.assertions import (
    assert_c0_continuity,
    assert_c1_continuity,
    assert_gradient_bounds,
    assert_curvature_bounds,
    assert_flow_conservation,
    assert_section_balance,
    assert_volumes_nonnegative
)
from tests.utils.fixtures import create_linear_ground, create_parabolic_ground, create_uniform_vp


@pytest.mark.integration
def test_flat_ground_optimal_solution(flat_ground, simple_vp, default_problem, output_dir):
    """
    OBVIOUS SOLUTION: Flat ground → road should be flat at ground elevation

    Expected:
    - All offsets u ≈ 0 (road at ground level)
    - All gradients ≈ 0 (flat)
    - All curvatures a2 ≈ 0 (no curvature)
    - Zero cut/fill volumes
    - Zero cost (no earthwork)
    """
    result = optimise(flat_ground, simple_vp, default_problem)

    assert result.success, "Optimization failed"

    # Check offsets near zero
    np.testing.assert_array_almost_equal(result.u, np.zeros_like(result.u), decimal=2,
                                        err_msg="Offsets should be near zero on flat ground")

    # Check zero volumes
    assert np.sum(result.v_cut) < 0.1, "Should have minimal cut on flat ground"
    assert np.sum(result.v_fill) < 0.1, "Should have minimal fill on flat ground"

    # Check flat profile (a1 ≈ 0, a2 ≈ 0)
    np.testing.assert_array_almost_equal(result.a1, np.zeros_like(result.a1), decimal=3,
                                        err_msg="Gradients should be near zero on flat ground")
    np.testing.assert_array_almost_equal(result.a2, np.zeros_like(result.a2), decimal=4,
                                        err_msg="Curvature should be zero on flat ground")

    # Check minimal cost
    assert result.total_cost < 100, f"Should have near-zero cost, got ${result.total_cost:.2f}"

    # Visualize
    plot_all_results(flat_ground, result, output_dir, "test_flat_ground")


@pytest.mark.integration
def test_constant_slope_optimal_solution(sloped_ground, simple_vp, default_problem, output_dir):
    """
    OBVIOUS SOLUTION: Constant 5% slope ground → road should follow with 5% grade

    Expected:
    - All offsets u ≈ 0
    - All gradients ≈ 0.05 (5%)
    - All a1 ≈ 0.05
    - All a2 ≈ 0 (straight line, no curvature)
    - Zero cut/fill volumes
    """
    result = optimise(sloped_ground, simple_vp, default_problem)

    assert result.success, "Optimization failed"

    # Check offsets near zero
    np.testing.assert_array_almost_equal(result.u, np.zeros_like(result.u), decimal=2,
                                        err_msg="Offsets should be near zero following constant slope")

    # Check constant gradient ≈ 5%
    expected_gradient = 0.05
    for i in range(result.num_segments):
        assert abs(result.a1[i] - expected_gradient) < 0.01, \
            f"Segment {i} gradient {result.a1[i]:.4f} should be ~{expected_gradient:.2f}"
        assert abs(result.a2[i]) < 1e-4, \
            f"Segment {i} should have zero curvature, got {result.a2[i]:.6e}"

    # Check minimal volumes
    assert np.sum(result.v_cut) < 1.0, "Should have minimal cut"
    assert np.sum(result.v_fill) < 1.0, "Should have minimal fill"

    # Visualize
    plot_all_results(sloped_ground, result, output_dir, "test_constant_slope")


@pytest.mark.integration
def test_step_change_smoothing(step_ground, simple_vp, default_problem, output_dir):
    """
    OBVIOUS SOLUTION: 10m step at midpoint → road should create smooth vertical curve

    Expected:
    - Road elevation transitions smoothly (no step)
    - Non-zero curvature around step location
    - Some cut/fill to create transition
    - C0/C1 continuity maintained
    """
    result = optimise(step_ground, simple_vp, default_problem)

    plot_all_results(step_ground, result, output_dir, "test_step_smoothing.")

    assert result.success, "Optimization failed"

    # Verify continuity
    assert_c0_continuity(result)
    assert_c1_continuity(result)

    # Check some earthwork occurred to smooth the step
    total_earthwork = np.sum(result.v_cut) + np.sum(result.v_fill)
    assert total_earthwork > 10.0, \
        f"Should have significant earthwork to smooth step, got {total_earthwork:.2f} m³"

    # Check that at least one segment has non-zero curvature
    max_curvature = np.max(np.abs(result.a2))
    assert max_curvature > 1e-4, \
        f"Should have curvature to smooth step, got max a2={max_curvature:.6e}"

    # Visualize



@pytest.mark.integration
def test_single_segment(single_segment_ground, single_segment_vp, default_problem, output_dir):
    """
    MINIMAL CASE: Single segment (2 stations)

    Expected:
    - Solution exists (not degenerate)
    - Single parabola connecting endpoints
    - Satisfies all constraints
    """
    result = optimise(single_segment_ground, single_segment_vp, default_problem)

    assert result.success, "Optimization should succeed for single segment"
    assert result.num_segments == 1, f"Expected 1 segment, got {result.num_segments}"
    assert result.num_stations == 2, f"Expected 2 stations, got {result.num_stations}"

    # No C0/C1 constraints to check (only one segment)
    # Just check gradient bounds
    assert_gradient_bounds(result, default_problem.g_min, default_problem.g_max)

    # Check non-negative volumes
    assert_volumes_nonnegative(result)

    # Visualize
    plot_all_results(single_segment_ground, result, output_dir, "test_single_segment")


@pytest.mark.integration
def test_parameter_variation_highway_vs_local(sinusoidal_ground, simple_vp,
                                               highway_problem, local_road_problem, output_dir):
    """
    Compare solutions with different design standards

    Expected:
    - Highway: gentler curves (larger K), tighter gradient limits, higher cost
    - Local road: tighter curves (smaller K), looser gradient limits, lower cost
    """
    # Solve with highway standards
    result_highway = optimise(sinusoidal_ground, simple_vp, highway_problem)
    assert result_highway.success, "Highway optimization failed"

    # Solve with local road standards
    result_local = optimise(sinusoidal_ground, simple_vp, local_road_problem)
    assert result_local.success, "Local road optimization failed"

    # Highway should have gentler curves (smaller |a2|)
    highway_max_curv = np.max(np.abs(result_highway.a2))
    local_max_curv = np.max(np.abs(result_local.a2))
    assert highway_max_curv < local_max_curv, \
        f"Highway should have gentler curves: highway a2_max={highway_max_curv:.6e}, " \
        f"local a2_max={local_max_curv:.6e}"

    # Print cost comparison for reference
    print(f"\nHighway cost: ${result_highway.total_cost:,.0f}")
    print(f"Local road cost: ${result_local.total_cost:,.0f}")

    # Visualize both solutions
    plot_all_results(sinusoidal_ground, result_highway, output_dir, "highway_design")
    plot_all_results(sinusoidal_ground, result_local, output_dir, "local_road_design")


@pytest.mark.integration
def test_linear_uphill_grade(default_problem, output_dir):
    """
    OBVIOUS SOLUTION: Linear uphill at exactly constraint limit (10%)

    Ground has 10% grade, road should follow exactly at 10% (constraint boundary)
    """
    # Create 300m ground with 10% upward slope (30m elevation gain)
    gp = create_linear_ground(100.0, 130.0, 300.0, num_points=11)
    vp = create_uniform_vp(300.0, 3)

    result = optimise(gp, vp, default_problem)
    assert result.success, "Optimization failed"

    # Check road follows ground (minimal offset)
    np.testing.assert_array_almost_equal(result.u, np.zeros_like(result.u), decimal=1,
                                        err_msg="Road should follow ground closely")

    # Check gradient is at upper limit (10%)
    for i in range(result.num_segments):
        assert abs(result.a1[i] - 0.1) < 0.01, \
            f"Segment {i} gradient should be ~10%, got {result.a1[i]*100:.2f}%"

    # Visualize
    plot_profile_elevation(gp, result,
                          output_dir / "profiles" / "test_linear_uphill.png")


@pytest.mark.integration
def test_linear_downhill_grade(default_problem, output_dir):
    """
    OBVIOUS SOLUTION: Linear downhill at exactly constraint limit (-10%)

    Ground has -10% grade, road should follow exactly at -10% (constraint boundary)
    """
    # Create 300m ground with -10% downward slope (30m elevation loss)
    gp = create_linear_ground(130.0, 100.0, 300.0, num_points=11)
    vp = create_uniform_vp(300.0, 3)

    result = optimise(gp, vp, default_problem)
    assert result.success, "Optimization failed"

    # Check road follows ground (minimal offset)
    np.testing.assert_array_almost_equal(result.u, np.zeros_like(result.u), decimal=1,
                                        err_msg="Road should follow ground closely")

    # Check gradient is at lower limit (-10%)
    for i in range(result.num_segments):
        assert abs(result.a1[i] - (-0.1)) < 0.01, \
            f"Segment {i} gradient should be ~-10%, got {result.a1[i]*100:.2f}%"

    # Visualize
    plot_profile_elevation(gp, result,
                          output_dir / "profiles" / "test_linear_downhill.png")


@pytest.mark.integration
def test_parabolic_valley(default_problem, output_dir):
    """
    OBVIOUS SOLUTION: Parabolic valley → road should smooth with vertical curve

    Expected:
    - Road uses curvature to follow valley shape
    - Sag curve constraints satisfied
    - Some earthwork to optimize alignment
    """
    # Create valley with vertex at 95m (5m below endpoints at 100m)
    gp = create_parabolic_ground(vertex_elev=95.0, length=300.0, num_points=31)
    vp = create_uniform_vp(300.0, 5)

    result = optimise(gp, vp, default_problem)
    assert result.success, "Optimization failed"

    # Check continuity
    assert_c0_continuity(result)
    assert_c1_continuity(result)

    # Check curvature constraints (sag curves should be positive a2)
    assert_curvature_bounds(result, default_problem.k_min_crest, default_problem.k_min_sag)

    # Should have some positive curvature (sag)
    assert np.any(result.a2 > 1e-4), "Should have positive curvature for valley"

    # Visualize
    plot_profile_elevation(gp, result,
                          output_dir / "profiles" / "test_parabolic_valley.png")
    plot_gradients(result, default_problem.g_min, default_problem.g_max,
                  output_dir / "gradients" / "test_parabolic_valley.png")


@pytest.mark.integration
def test_parabolic_hill(default_problem, output_dir):
    """
    OBVIOUS SOLUTION: Parabolic hill → road should smooth with vertical curve

    Expected:
    - Road uses curvature to follow hill shape
    - Crest curve constraints satisfied
    - Some earthwork to optimize alignment
    """
    # Create hill with vertex at 110m (10m above endpoints at 100m)
    gp = create_parabolic_ground(vertex_elev=110.0, length=300.0, num_points=31)
    vp = create_uniform_vp(300.0, 5)

    result = optimise(gp, vp, default_problem)
    assert result.success, "Optimization failed"

    # Check continuity
    assert_c0_continuity(result)
    assert_c1_continuity(result)

    # Check curvature constraints (crest curves should be negative a2)
    assert_curvature_bounds(result, default_problem.k_min_crest, default_problem.k_min_sag)

    # Should have some negative curvature (crest)
    assert np.any(result.a2 < -1e-4), "Should have negative curvature for hill"

    # Visualize
    plot_profile_elevation(gp, result,
                          output_dir / "profiles" / "test_parabolic_hill.png")
    plot_gradients(result, default_problem.g_min, default_problem.g_max,
                  output_dir / "gradients" / "test_parabolic_hill.png")


@pytest.mark.integration
def test_conservative_vs_aggressive_parameters(sinusoidal_ground, simple_vp,
                                                conservative_problem, aggressive_problem, output_dir):
    """
    Compare conservative vs aggressive design parameters

    Expected:
    - Conservative: gentler curves, tighter gradient limits, potentially higher cost
    - Aggressive: tighter curves, looser gradient limits, potentially lower cost
    """
    # Solve with conservative parameters
    result_conservative = optimise(sinusoidal_ground, simple_vp, conservative_problem)
    assert result_conservative.success, "Conservative optimization failed"

    # Solve with aggressive parameters
    result_aggressive = optimise(sinusoidal_ground, simple_vp, aggressive_problem)
    assert result_aggressive.success, "Aggressive optimization failed"

    # Conservative should have gentler curves
    conservative_max_curv = np.max(np.abs(result_conservative.a2))
    aggressive_max_curv = np.max(np.abs(result_aggressive.a2))
    assert conservative_max_curv < aggressive_max_curv, \
        "Conservative should have gentler curves than aggressive"

    # Print comparison
    print(f"\nConservative: max_curv={conservative_max_curv:.6e}, cost=${result_conservative.total_cost:,.0f}")
    print(f"Aggressive: max_curv={aggressive_max_curv:.6e}, cost=${result_aggressive.total_cost:,.0f}")

    # Visualize
    plot_all_results(sinusoidal_ground, result_conservative, output_dir, "conservative_design")
    plot_all_results(sinusoidal_ground, result_aggressive, output_dir, "aggressive_design")


@pytest.mark.integration
@pytest.mark.slow
def test_realistic_hilly_terrain_fine_spacing(default_problem, output_dir):
    """
    REALISTIC SCENARIO: 1km alignment with fine station spacing (10m) through hilly terrain

    Ground profile features:
    - 0-300m: Valley section (starts at 100m, dips to 95m, rises to 105m)
    - 300-500m: Straight upward slope (5% grade)
    - 500-700m: Rounded crest/hill (peak ~120m)
    - 700-1000m: Gentle rolling terrain

    With 101 stations (10m spacing), volume calculation should be accurate.

    Expected:
    - Road smoothly follows terrain with vertical curves
    - C0/C1 continuity throughout
    - Gradient and curvature constraints satisfied
    - Reasonable earthwork volumes (not zero due to fine spacing)
    """
    # Create ground profile with 101 stations (0, 10, 20, ..., 1000m)
    # Moderately bumpy terrain with a straight section
    stations = np.linspace(0, 1000, 101)
    elevations = np.zeros_like(stations)

    for i, s in enumerate(stations):
        if s <= 300:
            # Rolling valley section: gentle undulations
            base = 100.0 + 0.01 * s
            undulation = 2.5 * np.sin(2 * np.pi * s / 150.0)
            elevations[i] = base + undulation

        elif s <= 500:
            # Mostly straight slope section (4% grade) with minimal bumps
            base = 103.0 + 0.04 * (s - 300)
            small_variation = 0.5 * np.sin(2 * np.pi * s / 60.0)
            elevations[i] = base + small_variation

        else:
            # Gentle rolling section
            base = 111.0 + 0.005 * (s - 500)
            undulation = 2.0 * np.sin(2 * np.pi * s / 180.0) + 1.0 * np.sin(2 * np.pi * s / 70.0)
            elevations[i] = base + undulation

    gp = gp_create(stations, elevations)

    # Create vertical profile with 100 segments (same station spacing)
    vp = vp_create(stations, side_slope=2.0, road_width=6.0)

    result = optimise(gp, vp, default_problem)
    assert result.success, "Optimization failed"

    # Verify basic properties
    assert result.num_stations == 101, f"Expected 101 stations, got {result.num_stations}"
    assert result.num_segments == 100, f"Expected 100 segments, got {result.num_segments}"

    # Verify continuity
    assert_c0_continuity(result)
    assert_c1_continuity(result)

    # Verify constraints
    assert_gradient_bounds(result, default_problem.g_min, default_problem.g_max)
    assert_curvature_bounds(result, default_problem.k_min_crest, default_problem.k_min_sag)
    assert_volumes_nonnegative(result)

    # With fine spacing, we should capture real earthwork volumes
    total_earthwork = np.sum(result.v_cut) + np.sum(result.v_fill)
    print(f"\nTotal earthwork: {total_earthwork:.1f} m³")
    print(f"Total cost: ${result.total_cost:,.0f}")
    print(f"Cut: {np.sum(result.v_cut):.1f} m³, Fill: {np.sum(result.v_fill):.1f} m³")

    print(f"Setup time: {result.setup_time.total_seconds():.3f} s")
    print(f"Solve time: {result.solve_time.total_seconds():.3f} s")

    # Visualize
    plot_all_results(gp, result, output_dir, "realistic_hilly_terrain")
