import numpy as np
from typing import Union
from dataclasses import dataclass


@dataclass
class PWQuadratic:
    """
    Piece-wise quadratic defined on segments. For each segment g:
    P_g(s) = a_{g,0} + a_{g,1} (s - s_{g,0})
    """
    num_segments: int
    stations: np.ndarray
    segment_stations: np.ndarray
    a0: np.ndarray
    a1: np.ndarray
    a2: np.ndarray


def pwq_create(stations: np.ndarray):
    num_segments = len(stations) - 1
    stations = np.sort(stations)
    segment_stations = np.column_stack([stations[:-1], stations[1:]])
    a0 = np.zeros(num_segments)
    a1 = np.zeros(num_segments)
    a2 = np.zeros(num_segments)
    return PWQuadratic(num_segments=num_segments, stations=stations, segment_stations=segment_stations,
                       a0=a0, a1=a1, a2=a2)


def pwq_create_from_coeffs(stations: np.ndarray, a0: np.ndarray, a1: np.ndarray, a2: np.ndarray):
    """
    Create a PWQuadratic from segment stations and coefficients.
    Assumes segment_stations are already sorted.
    Makes copies of all input arrays.
    """
    num_segments = len(stations) - 1
    segment_stations = np.column_stack([stations[:-1], stations[1:]])
    return PWQuadratic(
        num_segments=num_segments,
        stations=stations.copy(),
        segment_stations=segment_stations,
        a0=a0.copy(),
        a1=a1.copy(),
        a2=a2.copy()
    )


def pwq_stations(pwq: PWQuadratic):
    return np.array(pwq.stations)


def pwq_num_segments(pwq: PWQuadratic):
    return pwq.num_segments


def pwq_num_stations(pwq: PWQuadratic):
    return len(pwq.stations)


def pwq_segment_lengths(pwq: PWQuadratic):
    return pwq.stations[1:] - pwq.stations[:-1]


def pwq_set_coeffs(pwq: PWQuadratic, a0: np.ndarray, a1: np.ndarray, a2: np.ndarray):
    pwq.a0 = a0
    pwq.a1 = a1
    pwq.a2 = a2


def pwq_segment_index(pwq: PWQuadratic, s: float):
    return __pwq_left(pwq, s)


def pwq_segment_start_station(pwq: PWQuadratic, segment_index: int):
    return pwq.stations[segment_index]


def pwq_eval(pwq: PWQuadratic, s: np.ndarray):
    s = __pwq_clip(pwq, s)
    idx_left = __pwq_left(pwq, s)
    s_left = pwq.stations[idx_left]

    a0 = pwq.a0[idx_left]
    a1 = pwq.a1[idx_left]
    a2 = pwq.a2[idx_left]
    ds = s - s_left

    return a0 + a1*ds + a2*np.power(ds, 2)


def pwq_evald(pwq: PWQuadratic, s: np.ndarray):
    s = __pwq_clip(pwq, s)
    idx_left = __pwq_left(pwq, s)
    s_left = pwq.stations[idx_left]

    a1 = pwq.a1[idx_left]
    a2 = pwq.a2[idx_left]
    ds = s - s_left

    return a1 + 2*a2*ds


def pwq_evaldd(pwq: PWQuadratic, s: np.ndarray):
    s = __pwq_clip(pwq, s)
    idx_left = __pwq_left(pwq, s)
    a2 = pwq.a2[idx_left]
    return 2*a2


def __pwq_clip(pwq: PWQuadratic, s: np.ndarray):
    return np.clip(s, pwq.stations[0], pwq.stations[-1])


def __pwq_left(pwq: PWQuadratic, s: Union[np.ndarray, float]):
    """Find index of left edge of segment"""
    is_scalar = np.isscalar(s)
    if is_scalar:
        s = np.array([s])

    first_station = pwq.stations[0]
    s[s <= first_station] = first_station

    last_station = pwq.stations[-1]
    s[s >= last_station] = last_station - 1e-6

    indices = np.searchsorted(pwq.stations, s, side='right') - 1

    return indices[0] if is_scalar else indices