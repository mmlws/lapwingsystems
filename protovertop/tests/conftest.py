import pytest
import numpy as np
from pathlib import Path
from groundprofile import GroundProfile, gp_create
from vertprofile import VerticalProfile, vp_create
from problem import OptimisationProblem


# Test output directory
@pytest.fixture(scope="session")
def output_dir():
    """Create and return test output directory"""
    base = Path(__file__).parent / "outputs"
    base.mkdir(exist_ok=True)
    for subdir in ["profiles", "cutfill", "gradients", "flows"]:
        (base / subdir).mkdir(exist_ok=True)
    return base


# Problem parameter fixtures
@pytest.fixture
def default_problem():
    """Default optimization problem parameters"""
    return OptimisationProblem.default()


@pytest.fixture
def conservative_problem():
    """Conservative problem parameters (gentle curves, moderate grades)"""
    return OptimisationProblem.conservative()


@pytest.fixture
def aggressive_problem():
    """Aggressive problem parameters (tight curves, steep grades)"""
    return OptimisationProblem.aggressive()


@pytest.fixture
def highway_problem():
    """Highway design standards (AASHTO 110 km/h)"""
    return OptimisationProblem.highway()


@pytest.fixture
def local_road_problem():
    """Local road design standards (AASHTO 50 km/h)"""
    return OptimisationProblem.local_road()


# Simple ground profiles
@pytest.fixture
def flat_ground():
    """Flat ground profile at elevation 100m"""
    stations = np.array([0.0, 100.0, 200.0, 300.0])
    elevations = np.array([100.0, 100.0, 100.0, 100.0])
    return gp_create(stations, elevations)


@pytest.fixture
def sloped_ground():
    """Constant 5% upward slope"""
    stations = np.array([0.0, 100.0, 200.0, 300.0])
    elevations = np.array([100.0, 105.0, 110.0, 115.0])
    return gp_create(stations, elevations)


@pytest.fixture
def step_ground():
    """Step change at station 150m"""
    stations = np.array([0.0, 100.0, 200.0, 300.0])
    elevations = np.array([100.0, 100.0, 110.0, 110.0])
    return gp_create(stations, elevations)


@pytest.fixture
def sinusoidal_ground():
    """Rolling terrain with sinusoidal variation"""
    stations = np.linspace(0, 300, 31)
    elevations = 100 + 5 * np.sin(2 * np.pi * stations / 200)
    return gp_create(stations, elevations)


# Simple vertical profiles
@pytest.fixture
def simple_vp():
    """Basic vertical profile for testing"""
    segment_stations = np.array([0.0, 100.0, 200.0, 300.0])
    side_slope = 2.0  # 1V:2H
    road_width = 6.0  # 6m width
    return vp_create(segment_stations, side_slope, road_width)


@pytest.fixture
def narrow_vp():
    """Narrow road vertical profile"""
    segment_stations = np.array([0.0, 100.0, 200.0, 300.0])
    side_slope = 2.0
    road_width = 3.5  # Single lane
    return vp_create(segment_stations, side_slope, road_width)


@pytest.fixture
def wide_vp():
    """Wide road vertical profile (highway)"""
    segment_stations = np.array([0.0, 100.0, 200.0, 300.0])
    side_slope = 3.0  # Gentler slope
    road_width = 12.0  # Two lanes + shoulders
    return vp_create(segment_stations, side_slope, road_width)


# Single segment profiles
@pytest.fixture
def single_segment_ground():
    """Minimal ground profile: 2 stations, 1 segment"""
    stations = np.array([0.0, 100.0])
    elevations = np.array([100.0, 105.0])
    return gp_create(stations, elevations)


@pytest.fixture
def single_segment_vp():
    """Minimal vertical profile: 2 stations, 1 segment"""
    segment_stations = np.array([0.0, 100.0])
    return vp_create(segment_stations, 2.0, 6.0)
