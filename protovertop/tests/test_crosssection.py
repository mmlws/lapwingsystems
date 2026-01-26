import pytest
import numpy as np
from crosssection import xs_cut_areas, xs_fill_areas
from groundprofile import gp_create
from vertprofile import vp_create
from pwquadratic import pwq_set_coeffs


@pytest.mark.unit
def test_xs_cut_areas_zero_offset():
    """Test cut area when road is at ground level (offset=0)"""
    # Create flat ground at elevation 100
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))

    # Create vertical profile and initialize to same elevation as ground
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    # Set road elevation to 100m (flat, same as ground)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    # Evaluate at station 50m with zero offset
    s = 50.0
    offsets = np.array([0.0])

    areas = xs_cut_areas(vp, gp, s, offsets)

    assert abs(areas[0]) < 1e-10, f"Expected zero cut area at offset=0, got {areas[0]}"


@pytest.mark.unit
def test_xs_fill_areas_zero_offset():
    """Test fill area when road is at ground level (offset=0)"""
    # Create flat ground at elevation 100
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))

    # Create vertical profile and initialize to same elevation as ground
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    # Evaluate at station 50m with zero offset
    s = 50.0
    offsets = np.array([0.0])

    areas = xs_fill_areas(vp, gp, s, offsets)

    assert abs(areas[0]) < 1e-10, f"Expected zero fill area at offset=0, got {areas[0]}"


@pytest.mark.unit
def test_xs_cut_areas_negative_offset():
    """Test cut area when road is below ground (negative offset)"""
    # Create flat ground at elevation 100
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))

    # Create vertical profile at ground level
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    # Test with negative offset (road below ground by 2m)
    s = 50.0
    offsets = np.array([-2.0])  # 2m cut

    areas = xs_cut_areas(vp, gp, s, offsets)

    # Cut area should be positive for negative offset
    assert areas[0] > 0, f"Expected positive cut area for offset=-2.0, got {areas[0]}"

    # For 2m cut with road_width=6m and side_slope=2:
    # Area = road_width * |h| + side_slope * h * |h|
    # Area = 6 * 2 + 2 * 2 * 2 = 12 + 8 = 20 m²
    expected_approx = 20.0
    assert abs(areas[0] - expected_approx) < 2.0, \
        f"Expected cut area ~{expected_approx}, got {areas[0]}"


@pytest.mark.unit
def test_xs_fill_areas_positive_offset():
    """Test fill area when road is above ground (positive offset)"""
    # Create flat ground at elevation 100
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))

    # Create vertical profile at ground level
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    # Test with positive offset (road above ground by 2m)
    s = 50.0
    offsets = np.array([2.0])  # 2m fill

    areas = xs_fill_areas(vp, gp, s, offsets)

    # Fill area should be positive for positive offset
    assert areas[0] > 0, f"Expected positive fill area for offset=2.0, got {areas[0]}"

    # For 2m fill: Area = road_width * h + side_slope * h * h
    # Area = 6 * 2 + 2 * 2 * 2 = 12 + 8 = 20 m²
    expected_approx = 20.0
    assert abs(areas[0] - expected_approx) < 2.0, \
        f"Expected fill area ~{expected_approx}, got {areas[0]}"


@pytest.mark.unit
def test_xs_cut_areas_positive_offset():
    """Test that cut area is zero when road is above ground"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    s = 50.0
    offsets = np.array([2.0])  # Road above ground

    areas = xs_cut_areas(vp, gp, s, offsets)

    # No cut when road is above ground
    assert abs(areas[0]) < 1e-10, f"Expected zero cut area for positive offset, got {areas[0]}"


@pytest.mark.unit
def test_xs_fill_areas_negative_offset():
    """Test that fill area is zero when road is below ground"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    s = 50.0
    offsets = np.array([-2.0])  # Road below ground

    areas = xs_fill_areas(vp, gp, s, offsets)

    # No fill when road is below ground
    assert abs(areas[0]) < 1e-10, f"Expected zero fill area for negative offset, got {areas[0]}"


@pytest.mark.unit
def test_xs_cut_areas_varying_side_slope():
    """Test cut area sensitivity to side slope"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))

    # Test with steep side slope
    vp_steep = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=1.0, road_width=6.0)
    pwq_set_coeffs(vp_steep.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))
    areas_steep = xs_cut_areas(vp_steep, gp, 50.0, np.array([-2.0]))

    # Test with gentle side slope
    vp_gentle = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=3.0, road_width=6.0)
    pwq_set_coeffs(vp_gentle.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))
    areas_gentle = xs_cut_areas(vp_gentle, gp, 50.0, np.array([-2.0]))

    assert areas_gentle[0] > areas_steep[0], \
        f"Gentler slope should have larger cut area: {areas_gentle[0]} > {areas_steep[0]}"


@pytest.mark.unit
def test_xs_fill_areas_varying_side_slope():
    """Test fill area sensitivity to side slope"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))

    # Test with steep side slope
    vp_steep = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=1.0, road_width=6.0)
    pwq_set_coeffs(vp_steep.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))
    areas_steep = xs_fill_areas(vp_steep, gp, 50.0, np.array([2.0]))

    # Test with gentle side slope
    vp_gentle = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=3.0, road_width=6.0)
    pwq_set_coeffs(vp_gentle.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))
    areas_gentle = xs_fill_areas(vp_gentle, gp, 50.0, np.array([2.0]))

    assert areas_gentle[0] > areas_steep[0], \
        f"Gentler slope should have larger fill area: {areas_gentle[0]} > {areas_steep[0]}"


@pytest.mark.unit
def test_xs_cut_areas_varying_road_width():
    """Test cut area sensitivity to road width"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))

    # Narrow road
    vp_narrow = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=4.0)
    pwq_set_coeffs(vp_narrow.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))
    areas_narrow = xs_cut_areas(vp_narrow, gp, 50.0, np.array([-2.0]))

    # Wide road
    vp_wide = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=10.0)
    pwq_set_coeffs(vp_wide.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))
    areas_wide = xs_cut_areas(vp_wide, gp, 50.0, np.array([-2.0]))

    assert areas_wide[0] > areas_narrow[0], \
        f"Wider road should have larger cut area: {areas_wide[0]} > {areas_narrow[0]}"


@pytest.mark.unit
def test_xs_fill_areas_varying_road_width():
    """Test fill area sensitivity to road width"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))

    # Narrow road
    vp_narrow = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=4.0)
    pwq_set_coeffs(vp_narrow.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))
    areas_narrow = xs_fill_areas(vp_narrow, gp, 50.0, np.array([2.0]))

    # Wide road
    vp_wide = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=10.0)
    pwq_set_coeffs(vp_wide.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))
    areas_wide = xs_fill_areas(vp_wide, gp, 50.0, np.array([2.0]))

    assert areas_wide[0] > areas_narrow[0], \
        f"Wider road should have larger fill area: {areas_wide[0]} > {areas_narrow[0]}"


@pytest.mark.unit
def test_xs_cut_areas_small_offset():
    """Test cut area for small offset (1cm)"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    areas = xs_cut_areas(vp, gp, 50.0, np.array([-0.01]))

    # Should be small but non-zero
    assert areas[0] > 0, "Should have positive cut area"
    assert areas[0] < 0.1, f"Should have small cut area for 1cm offset, got {areas[0]}"


@pytest.mark.unit
def test_xs_fill_areas_small_offset():
    """Test fill area for small offset (1cm)"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    areas = xs_fill_areas(vp, gp, 50.0, np.array([0.01]))

    # Should be small but non-zero
    assert areas[0] > 0, "Should have positive fill area"
    assert areas[0] < 0.1, f"Should have small fill area for 1cm offset, got {areas[0]}"


@pytest.mark.unit
def test_xs_cut_areas_large_offset():
    """Test cut area for large offset (10m)"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    areas = xs_cut_areas(vp, gp, 50.0, np.array([-10.0]))

    # Should be large: 6*10 + 2*10*10 = 60 + 200 = 260
    assert areas[0] > 100, f"Expected large cut area for 10m offset, got {areas[0]}"


@pytest.mark.unit
def test_xs_fill_areas_large_offset():
    """Test fill area for large offset (10m)"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    areas = xs_fill_areas(vp, gp, 50.0, np.array([10.0]))

    # Should be large: 6*10 + 2*10*10 = 60 + 200 = 260
    assert areas[0] > 100, f"Expected large fill area for 10m offset, got {areas[0]}"


@pytest.mark.unit
def test_xs_area_symmetry():
    """Test that cut and fill areas are symmetric for equal magnitude offsets"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    for offset_mag in [0.5, 1.0, 2.0, 5.0]:
        areas_cut = xs_cut_areas(vp, gp, 50.0, np.array([-offset_mag]))
        areas_fill = xs_fill_areas(vp, gp, 50.0, np.array([offset_mag]))

        # Should be approximately equal (symmetric cross-section)
        assert abs(areas_cut[0] - areas_fill[0]) < 0.01, \
            f"Cut and fill areas should be symmetric: cut={areas_cut[0]}, fill={areas_fill[0]}"


@pytest.mark.unit
def test_xs_area_vectorized_cut():
    """Test vectorized computation of cut areas"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    offset_array = np.array([-3.0, -2.0, -1.0, 0.0, 1.0])
    areas = xs_cut_areas(vp, gp, 50.0, offset_array)

    assert areas.shape == offset_array.shape, "Should return array of same shape"
    # Negative offsets should have non-zero cut
    assert areas[0] > 0, "u=-3 should have cut"
    assert areas[1] > 0, "u=-2 should have cut"
    assert areas[2] > 0, "u=-1 should have cut"
    # Zero and positive offsets should have zero cut
    assert abs(areas[3]) < 1e-10, "u=0 should have no cut"
    assert abs(areas[4]) < 1e-10, "u=1 should have no cut"


@pytest.mark.unit
def test_xs_area_vectorized_fill():
    """Test vectorized computation of fill areas"""
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 100.0]), a1=np.array([0.0, 0.0]), a2=np.array([0.0, 0.0]))

    offset_array = np.array([-1.0, 0.0, 1.0, 2.0, 3.0])
    areas = xs_fill_areas(vp, gp, 50.0, offset_array)

    assert areas.shape == offset_array.shape, "Should return array of same shape"
    # Negative and zero offsets should have zero fill
    assert abs(areas[0]) < 1e-10, "u=-1 should have no fill"
    assert abs(areas[1]) < 1e-10, "u=0 should have no fill"
    # Positive offsets should have non-zero fill
    assert areas[2] > 0, "u=1 should have fill"
    assert areas[3] > 0, "u=2 should have fill"
    assert areas[4] > 0, "u=3 should have fill"


@pytest.mark.unit
def test_xs_areas_at_different_stations():
    """Test that areas are computed correctly at different stations"""
    # Create sloped ground
    gp = gp_create(np.array([0.0, 100.0, 200.0]), np.array([100.0, 105.0, 110.0]))
    vp = vp_create(np.array([0.0, 100.0, 200.0]), side_slope=2.0, road_width=6.0)
    # Set road to match ground (5% slope)
    pwq_set_coeffs(vp.pwq, a0=np.array([100.0, 105.0]), a1=np.array([0.05, 0.05]), a2=np.array([0.0, 0.0]))

    offset = np.array([-2.0])

    # Test at different stations - areas should be the same for constant offset
    areas_s0 = xs_cut_areas(vp, gp, 0.0, offset)
    areas_s50 = xs_cut_areas(vp, gp, 50.0, offset)
    areas_s100 = xs_cut_areas(vp, gp, 100.0, offset)

    # All should be positive (cut areas)
    assert areas_s0[0] > 0
    assert areas_s50[0] > 0
    assert areas_s100[0] > 0

    # Should be roughly the same (same offset magnitude, same cross-section geometry)
    assert abs(areas_s0[0] - areas_s50[0]) < 1.0
    assert abs(areas_s50[0] - areas_s100[0]) < 1.0
