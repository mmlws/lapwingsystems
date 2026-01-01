import numpy as np
from typing import Union

from dataclasses import dataclass


@dataclass
class GroundProfile:
    stations: np.ndarray
    elevations: np.ndarray


def gp_create(stations: np.ndarray, elevations: np.ndarray):
    return GroundProfile(stations, elevations)


def gp_create_from_csv(path: str):
    stations, profile = np.loadtxt(path, skiprows=1, unpack=True, usecols=(0, 1), delimiter=',')
    return GroundProfile(stations, profile)


def gp_elevation_at(gp: GroundProfile, s: Union[float, np.ndarray]):
    return np.interp(s, gp.stations, gp.elevations)