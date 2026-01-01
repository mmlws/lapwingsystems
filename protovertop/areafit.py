import numpy as np
import xalglib as alg


def __roots(x: np.ndarray, y: np.ndarray):
    sign_changes = np.where(np.diff(np.sign(y)))[0]

    found = []
    for i in sign_changes:
        x1, x2 = x[i], x[i + 1]
        y1, y2 = y[i], y[i + 1]
        found.append(x1 - y1*(x2 - x1)/(y2 - y1))

    return found


def areafit_fill(u: np.ndarray, v: np.ndarray):
    idx = v > 0
    u = u[idx]
    v = v[idx]

    poly, rep = alg.polynomialfit(u, v, len(u), 3)
    coeff = alg.polynomialbar2pow(poly, 0.0, 1.0)

    c, b, a = coeff
    u_offset = (-b + np.sqrt(b ** 2 - 4*a*c))/(2*a)

    return np.array(coeff), u_offset, rep.rmserror


def areafit_cut(u: np.ndarray, v: np.ndarray):
    idx = v > 0
    u = u[idx]
    v = v[idx]

    poly, rep = alg.polynomialfit(u, v, len(u), 3)
    coeff = alg.polynomialbar2pow(poly, 0.0, 1.0)

    c, b, a = coeff
    u_offset = (-b - np.sqrt(b ** 2 - 4*a*c))/(2*a)

    return np.array(coeff), u_offset, rep.rmserror
