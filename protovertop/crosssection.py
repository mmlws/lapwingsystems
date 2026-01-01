import numpy as np
from dataclasses import dataclass

from groundprofile import GroundProfile, gp_elevation_at
from vertprofile import VerticalProfile, vp_elevation_at


@dataclass
class CrossSection:
    road_width: float
    slope: float
    elevation: float
    elevation_ground: float


def xs_cut_areas(vp: VerticalProfile, gp: GroundProfile, s: float, offsets: np.ndarray):
    elevations = vp_elevation_at(vp, s)
    elevation_ground = gp_elevation_at(gp, s)

    h = elevations + offsets - elevation_ground
    h[h > 0] = 0

    road_width = vp.road_width
    slope_width = vp.side_slope*h

    area = np.abs(road_width*h + slope_width*h)

    return area


def xs_fill_areas(vp: VerticalProfile, gp: GroundProfile, s: float, offsets: np.ndarray):
    elevations = vp_elevation_at(vp, s)
    elevation_ground = gp_elevation_at(gp, s)

    h = elevations + offsets - elevation_ground
    h[h < 0] = 0

    road_width = vp.road_width
    slope_width = vp.side_slope*h

    area = road_width*h + slope_width*h

    return area
