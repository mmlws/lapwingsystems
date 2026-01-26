import numpy as np
from groundprofile import gp_create
from vertprofile import vp_create


def create_linear_ground(start_elev, end_elev, length, num_points=11):
    """Create linear ground profile

    Args:
        start_elev: Starting elevation (m)
        end_elev: Ending elevation (m)
        length: Total length (m)
        num_points: Number of ground points

    Returns:
        GroundProfile with linear elevation change
    """
    stations = np.linspace(0, length, num_points)
    elevations = np.linspace(start_elev, end_elev, num_points)
    return gp_create(stations, elevations)


def create_parabolic_ground(vertex_elev, length, num_points=31):
    """Create parabolic ground profile (valley or hill)

    Args:
        vertex_elev: Elevation at center vertex (m)
        length: Total length (m)
        num_points: Number of ground points

    Returns:
        GroundProfile with parabolic shape
    """
    stations = np.linspace(0, length, num_points)
    # Parabola: z = vertex_elev + a*(s - length/2)^2
    # Choose 'a' to give 10m elevation change at ends
    a = 10.0 / (length/2)**2
    s_center = length / 2
    elevations = vertex_elev + a * (stations - s_center)**2
    return gp_create(stations, elevations)


def create_uniform_vp(length, num_segments, side_slope=2.0, road_width=6.0):
    """Create vertical profile with uniform segment spacing

    Args:
        length: Total length (m)
        num_segments: Number of segments
        side_slope: Road side slope (dimensionless)
        road_width: Road width (m)

    Returns:
        VerticalProfile with num_segments+1 stations
    """
    segment_stations = np.linspace(0, length, num_segments + 1)
    return vp_create(segment_stations, side_slope, road_width)
