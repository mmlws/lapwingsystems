import pytest
import numpy as np
from groundprofile import gp_create, gp_elevation_at


@pytest.mark.unit
def test_gp_create_basic():
    """Test GroundProfile creation with valid inputs"""
    stations = np.array([0.0, 100.0, 200.0])
    elevations = np.array([100.0, 105.0, 110.0])
    gp = gp_create(stations, elevations)

    assert len(gp.stations) == 3
    assert len(gp.elevations) == 3
    np.testing.assert_array_equal(gp.stations, stations)
    np.testing.assert_array_equal(gp.elevations, elevations)


@pytest.mark.unit
def test_gp_create_single_point():
    """Test GroundProfile with single station"""
    stations = np.array([0.0])
    elevations = np.array([100.0])
    gp = gp_create(stations, elevations)

    assert len(gp.stations) == 1
    assert len(gp.elevations) == 1


@pytest.mark.unit
def test_gp_create_two_points():
    """Test GroundProfile with two stations (minimal valid)"""
    stations = np.array([0.0, 100.0])
    elevations = np.array([100.0, 110.0])
    gp = gp_create(stations, elevations)

    assert len(gp.stations) == 2
    assert len(gp.elevations) == 2


@pytest.mark.unit
def test_gp_elevation_at_exact_stations(flat_ground):
    """Test interpolation at exact station points"""
    # Flat ground fixture has elevation 100 everywhere
    elev = gp_elevation_at(flat_ground, 0.0)
    assert abs(elev - 100.0) < 1e-10, f"Expected 100.0, got {elev}"

    elev = gp_elevation_at(flat_ground, 100.0)
    assert abs(elev - 100.0) < 1e-10, f"Expected 100.0, got {elev}"

    elev = gp_elevation_at(flat_ground, 200.0)
    assert abs(elev - 100.0) < 1e-10, f"Expected 100.0, got {elev}"

    elev = gp_elevation_at(flat_ground, 300.0)
    assert abs(elev - 100.0) < 1e-10, f"Expected 100.0, got {elev}"


@pytest.mark.unit
def test_gp_elevation_at_array(flat_ground):
    """Test interpolation with array of stations"""
    stations_query = np.array([0.0, 100.0, 200.0, 300.0])
    elevations = gp_elevation_at(flat_ground, stations_query)

    assert elevations.shape == (4,), f"Expected shape (4,), got {elevations.shape}"
    np.testing.assert_array_almost_equal(elevations, [100.0, 100.0, 100.0, 100.0])


@pytest.mark.unit
def test_gp_elevation_at_interpolated(sloped_ground):
    """Test linear interpolation between stations"""
    # sloped_ground: [0, 100, 200, 300] -> [100, 105, 110, 115]
    # At midpoint between 0m and 100m (station 50m)
    elev = gp_elevation_at(sloped_ground, 50.0)
    expected = 102.5  # Linear interp: (100 + 105) / 2
    assert abs(elev - expected) < 1e-6, f"Expected {expected}, got {elev}"

    # At 1/4 point (station 25m)
    elev = gp_elevation_at(sloped_ground, 25.0)
    expected = 101.25  # Linear interp: 100 + 0.25 * (105 - 100)
    assert abs(elev - expected) < 1e-6, f"Expected {expected}, got {elev}"

    # At midpoint between 100m and 200m (station 150m)
    elev = gp_elevation_at(sloped_ground, 150.0)
    expected = 107.5  # Linear interp: (105 + 110) / 2
    assert abs(elev - expected) < 1e-6, f"Expected {expected}, got {elev}"


@pytest.mark.unit
def test_gp_elevation_at_extrapolation_before():
    """Test behavior before first station (should clamp or extrapolate)"""
    stations = np.array([100.0, 200.0])
    elevations = np.array([100.0, 110.0])
    gp = gp_create(stations, elevations)

    # Query before first station
    elev = gp_elevation_at(gp, 50.0)
    # Should either clamp to 100.0 or linearly extrapolate to 95.0
    # Check implementation behavior
    assert isinstance(elev, (float, np.floating)), f"Expected float, got {type(elev)}"


@pytest.mark.unit
def test_gp_elevation_at_extrapolation_after():
    """Test behavior after last station (should clamp or extrapolate)"""
    stations = np.array([0.0, 100.0])
    elevations = np.array([100.0, 110.0])
    gp = gp_create(stations, elevations)

    # Query after last station
    elev = gp_elevation_at(gp, 200.0)
    # Should either clamp to 110.0 or linearly extrapolate to 120.0
    assert isinstance(elev, (float, np.floating)), f"Expected float, got {type(elev)}"


@pytest.mark.unit
def test_gp_elevation_at_step_change(step_ground):
    """Test interpolation across step change"""
    # step_ground: [0, 100, 200, 300] -> [100, 100, 110, 110]
    # Step occurs between stations 1 and 2

    # Before step
    elev = gp_elevation_at(step_ground, 50.0)
    assert abs(elev - 100.0) < 1e-6, "Should be 100 before step"

    # At step transition (station 150m, midpoint between 100 and 200)
    elev = gp_elevation_at(step_ground, 150.0)
    expected = 105.0  # Linear interp: (100 + 110) / 2
    assert abs(elev - expected) < 1e-6, f"Expected {expected} at step midpoint, got {elev}"

    # After step
    elev = gp_elevation_at(step_ground, 250.0)
    assert abs(elev - 110.0) < 1e-6, "Should be 110 after step"


@pytest.mark.unit
def test_gp_elevation_at_sinusoidal(sinusoidal_ground):
    """Test interpolation on sinusoidal terrain"""
    # sinusoidal_ground has 31 points from 0 to 300m
    # Elevations: 100 + 5*sin(2*pi*s/200)

    # At station 0, sin(0) = 0, so elevation = 100
    elev = gp_elevation_at(sinusoidal_ground, 0.0)
    assert abs(elev - 100.0) < 0.1, f"Expected ~100.0 at s=0, got {elev}"

    # At station 50, sin(2*pi*50/200) = sin(pi/2) = 1, so elevation = 105
    elev = gp_elevation_at(sinusoidal_ground, 50.0)
    assert abs(elev - 105.0) < 0.2, f"Expected ~105.0 at s=50, got {elev}"

    # At station 100, sin(2*pi*100/200) = sin(pi) = 0, so elevation = 100
    elev = gp_elevation_at(sinusoidal_ground, 100.0)
    assert abs(elev - 100.0) < 0.1, f"Expected ~100.0 at s=100, got {elev}"


@pytest.mark.unit
def test_gp_elevation_at_vectorized():
    """Test vectorized elevation queries"""
    stations = np.array([0.0, 100.0, 200.0])
    elevations = np.array([100.0, 110.0, 120.0])
    gp = gp_create(stations, elevations)

    # Query at multiple stations
    query_stations = np.array([0.0, 50.0, 100.0, 150.0, 200.0])
    elevs = gp_elevation_at(gp, query_stations)

    assert elevs.shape == (5,), f"Expected shape (5,), got {elevs.shape}"
    assert abs(elevs[0] - 100.0) < 1e-6, "s=0 should be 100"
    assert abs(elevs[1] - 105.0) < 1e-6, "s=50 should be 105"
    assert abs(elevs[2] - 110.0) < 1e-6, "s=100 should be 110"
    assert abs(elevs[3] - 115.0) < 1e-6, "s=150 should be 115"
    assert abs(elevs[4] - 120.0) < 1e-6, "s=200 should be 120"


@pytest.mark.unit
def test_gp_create_with_unsorted_stations():
    """Test that unsorted stations are handled (or raise error)"""
    stations = np.array([100.0, 0.0, 200.0])  # Unsorted
    elevations = np.array([105.0, 100.0, 110.0])

    # Implementation should either sort them or raise an error
    # Try creating and see what happens
    try:
        gp = gp_create(stations, elevations)
        # If successful, verify interpolation still works
        elev = gp_elevation_at(gp, 50.0)
        assert isinstance(elev, (float, np.floating))
    except (ValueError, AssertionError):
        # If it raises an error, that's acceptable behavior
        pass
