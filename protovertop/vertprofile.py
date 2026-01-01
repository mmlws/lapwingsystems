from typing import Union
from dataclasses import dataclass

import numpy as np

from pwquadratic import (PWQuadratic, pwq_create, pwq_eval, pwq_num_segments, pwq_segment_lengths, pwq_num_stations,
                         pwq_stations, pwq_segment_index, pwq_segment_start_station)


@dataclass
class VerticalProfile:
    pwq: PWQuadratic
    side_slope: float
    road_width: float


def vp_create(segment_stations: np.ndarray, side_slope: float, road_width: float):
    pwq = pwq_create(segment_stations)
    return VerticalProfile(pwq, side_slope, road_width)


def vp_stations(vp: VerticalProfile):
    return pwq_stations(vp.pwq)


def vp_elevation_at(vp: VerticalProfile, station: Union[float, np.ndarray]):
    return pwq_eval(vp.pwq, station)


def vp_num_segments(vp: VerticalProfile):
    return pwq_num_segments(vp.pwq)


def vp_num_stations(vp: VerticalProfile):
    return pwq_num_stations(vp.pwq)


def vp_segment_lengths(vp: VerticalProfile):
    return pwq_segment_lengths(vp.pwq)


def vp_segment_index(vp: VerticalProfile, s: float):
    return pwq_segment_index(vp.pwq, s)

def vp_segment_start_station(vp: VerticalProfile, segment_index: int):
    return pwq_segment_start_station(vp.pwq, segment_index)