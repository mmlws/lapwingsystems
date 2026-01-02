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
    h = np.array(offsets)
    h[h > 0] = 0

    # Trapezoidal area: (bottom_width + top_width) / 2 * height
    # For cut: bottom = road_width, top = road_width + 2*side_slope*|h|
    # Area = road_width*|h| + side_slope*|h|²
    h_abs = np.abs(h)
    area = vp.road_width * h_abs + vp.side_slope * h_abs * h_abs

    return area


def xs_fill_areas(vp: VerticalProfile, gp: GroundProfile, s: float, offsets: np.ndarray):
    h = np.array(offsets)
    h[h < 0] = 0

    # Trapezoidal area: (top_width + bottom_width) / 2 * height
    # For fill: top = road_width, bottom = road_width + 2*side_slope*h
    # Area = road_width*h + side_slope*h²
    area = vp.road_width * h + vp.side_slope * h * h

    return area