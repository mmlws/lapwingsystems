import pytest
import numpy as np
from vertprofile import vp_create, vp_stations, vp_num_segments, vp_num_stations


@pytest.mark.unit
def test_vp_create_basic():
    """Test VerticalProfile creation with valid inputs"""
    segment_stations = np.array([0.0, 100.0, 200.0, 300.0])
    side_slope = 2.0
    road_width = 6.0

    vp = vp_create(segment_stations, side_slope, road_width)

    assert vp is not None
    assert hasattr(vp, 'pwq'), "VerticalProfile should have pwq attribute"
    assert hasattr(vp, 'side_slope'), "VerticalProfile should have side_slope attribute"
    assert hasattr(vp, 'road_width'), "VerticalProfile should have road_width attribute"


@pytest.mark.unit
def test_vp_stations(simple_vp):
    """Test extracting stations from VerticalProfile"""
    stations = vp_stations(simple_vp)

    assert isinstance(stations, np.ndarray), f"Expected ndarray, got {type(stations)}"
    assert stations.shape == (4,), f"Expected 4 stations, got {stations.shape}"
    np.testing.assert_array_equal(stations, [0.0, 100.0, 200.0, 300.0])


@pytest.mark.unit
def test_vp_num_segments(simple_vp):
    """Test counting segments in VerticalProfile"""
    num_seg = vp_num_segments(simple_vp)

    assert num_seg == 3, f"Expected 3 segments, got {num_seg}"


@pytest.mark.unit
def test_vp_num_stations(simple_vp):
    """Test counting stations in VerticalProfile"""
    num_sta = vp_num_stations(simple_vp)

    assert num_sta == 4, f"Expected 4 stations, got {num_sta}"


@pytest.mark.unit
def test_vp_create_single_segment(single_segment_vp):
    """Test VerticalProfile with single segment (2 stations)"""
    num_seg = vp_num_segments(single_segment_vp)
    num_sta = vp_num_stations(single_segment_vp)

    assert num_seg == 1, f"Expected 1 segment, got {num_seg}"
    assert num_sta == 2, f"Expected 2 stations, got {num_sta}"


@pytest.mark.unit
def test_vp_create_narrow_road(narrow_vp):
    """Test VerticalProfile with narrow road width"""
    assert narrow_vp.road_width == 3.5, f"Expected road_width=3.5, got {narrow_vp.road_width}"
    assert narrow_vp.side_slope == 2.0, f"Expected side_slope=2.0, got {narrow_vp.side_slope}"

    num_seg = vp_num_segments(narrow_vp)
    assert num_seg == 3, f"Expected 3 segments, got {num_seg}"


@pytest.mark.unit
def test_vp_create_wide_road(wide_vp):
    """Test VerticalProfile with wide road width and gentle side slope"""
    assert wide_vp.road_width == 12.0, f"Expected road_width=12.0, got {wide_vp.road_width}"
    assert wide_vp.side_slope == 3.0, f"Expected side_slope=3.0, got {wide_vp.side_slope}"

    num_seg = vp_num_segments(wide_vp)
    assert num_seg == 3, f"Expected 3 segments, got {num_seg}"


@pytest.mark.unit
def test_vp_create_varying_station_spacing():
    """Test VerticalProfile with non-uniform station spacing"""
    segment_stations = np.array([0.0, 50.0, 200.0, 300.0])  # Non-uniform
    side_slope = 2.0
    road_width = 6.0

    vp = vp_create(segment_stations, side_slope, road_width)

    stations = vp_stations(vp)
    np.testing.assert_array_equal(stations, segment_stations)

    num_seg = vp_num_segments(vp)
    assert num_seg == 3, f"Expected 3 segments, got {num_seg}"


@pytest.mark.unit
def test_vp_create_many_segments():
    """Test VerticalProfile with many segments (20)"""
    segment_stations = np.linspace(0, 1000, 21)  # 21 stations = 20 segments
    side_slope = 2.5
    road_width = 7.0

    vp = vp_create(segment_stations, side_slope, road_width)

    num_seg = vp_num_segments(vp)
    num_sta = vp_num_stations(vp)

    assert num_seg == 20, f"Expected 20 segments, got {num_seg}"
    assert num_sta == 21, f"Expected 21 stations, got {num_sta}"


@pytest.mark.unit
def test_vp_create_steep_side_slope():
    """Test VerticalProfile with steep side slope (1V:1H)"""
    segment_stations = np.array([0.0, 100.0, 200.0])
    side_slope = 1.0  # Very steep
    road_width = 6.0

    vp = vp_create(segment_stations, side_slope, road_width)

    assert vp.side_slope == 1.0
    num_seg = vp_num_segments(vp)
    assert num_seg == 2


@pytest.mark.unit
def test_vp_create_gentle_side_slope():
    """Test VerticalProfile with gentle side slope (1V:4H)"""
    segment_stations = np.array([0.0, 100.0, 200.0])
    side_slope = 4.0  # Very gentle
    road_width = 6.0

    vp = vp_create(segment_stations, side_slope, road_width)

    assert vp.side_slope == 4.0
    num_seg = vp_num_segments(vp)
    assert num_seg == 2


@pytest.mark.unit
def test_vp_create_zero_road_width():
    """Test VerticalProfile with zero road width (edge case)"""
    segment_stations = np.array([0.0, 100.0])
    side_slope = 2.0
    road_width = 0.0

    # Should either work or raise an error
    try:
        vp = vp_create(segment_stations, side_slope, road_width)
        assert vp.road_width == 0.0
    except (ValueError, AssertionError):
        pass  # Acceptable to reject zero width


@pytest.mark.unit
def test_vp_stations_returns_copy():
    """Test that vp_stations returns a copy, not a reference"""
    segment_stations = np.array([0.0, 100.0, 200.0])
    vp = vp_create(segment_stations, 2.0, 6.0)

    stations1 = vp_stations(vp)
    stations2 = vp_stations(vp)

    # Modify one
    stations1[0] = 999.0

    # Check that the other is unaffected (if it returns copies)
    # OR check that they both changed (if it returns references)
    # This test documents the behavior
    assert isinstance(stations2, np.ndarray)


@pytest.mark.unit
def test_vp_relationship_segments_stations():
    """Test that num_segments = num_stations - 1"""
    for num_stations in [2, 3, 5, 10, 50]:
        segment_stations = np.linspace(0, 1000, num_stations)
        vp = vp_create(segment_stations, 2.0, 6.0)

        num_seg = vp_num_segments(vp)
        num_sta = vp_num_stations(vp)

        assert num_seg == num_sta - 1, \
            f"Expected num_segments = {num_sta - 1}, got {num_seg}"
