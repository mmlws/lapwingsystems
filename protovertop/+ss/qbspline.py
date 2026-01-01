import numpy as np


def zdiv(a, b):
    if b == 0:
        return 0
    return a / b


class QuadraticBSpline:
    def __init__(self, knots):
        self.degree = 2

        self.num_internal_knots = 0
        self.num_knots = 0
        self.num_coeff = 0
        self.num_coeffd = 0
        self.num_coeffdd = 0

        self.knots = np.empty((self.num_knots,))
        self.coeffs = np.empty((self.num_coeff,))
        self.coeffsd = np.empty((self.num_coeffd,))
        self.coeffsdd = np.empty((self.num_coeffdd,))

        self.set_knots(knots)

    def set_knots(self, knots):
        self.num_knots = len(knots) + 2 * self.degree
        self.num_internal_knots = self.num_knots - 2 * (self.degree + 1)
        self.num_coeff = self.num_knots - self.degree - 1
        self.num_coeffd = self.num_coeff - 1
        self.num_coeffdd = self.num_coeffd - 1

        self.knots = np.zeros(self.num_knots)
        self.internal_knots = self.knots[(self.degree + 1):-(self.degree + 1)]

        self.knots[:self.degree + 1] = knots[0]
        self.knots[-self.degree - 1:] = knots[-1]
        self.internal_knots[:] = knots[1:-1]

        self.coeffs = np.zeros(self.num_coeff)
        self.coeffsd = np.zeros(self.num_coeffd)
        self.coeffsdd = np.zeros(self.num_coeffdd)

        self.__update_derivative_coeffs()

    def set_coeffs(self, coeffs):
        if len(coeffs) != self.num_coeff:
            raise ValueError(f"Expected {self.num_coeff} coefficients")
        self.coeffs = coeffs
        self.__update_derivative_coeffs()

    def __update_derivative_coeffs(self):
        K = self.knots
        C = self.coeffs
        Cd = self.coeffsd
        Cdd = self.coeffsdd

        p = self.degree
        for i in range(self.num_coeffd):
            Cd[i] = zdiv(p, K[i + p + 1] - K[i + 1]) * (C[i + 1] - C[i])

        for i in range(self.num_coeffdd):
            Cdd[i] = zdiv(p - 1, K[i + p + 1] - K[i + 2]) * (Cd[i + 1] - Cd[i])

    def N0(self, i, x):
        K = self.knots
        if i < self.num_coeff - 1:
            supported = np.logical_and(K[i] <= x, x < K[i + 1])
        else:
            supported = np.logical_and(K[i] <= x, x <= K[i + 1])
        return supported.astype(float)

    def N1(self, i, x):
        K = self.knots
        N0 = self.N0
        return (zdiv(x - K[i], K[i + 1] - K[i]) * N0(i, x) +
                zdiv(K[i + 2] - x, K[i + 2] - K[i + 1]) * N0(i + 1, x))

    def N2(self, i, x):
        K = self.knots
        N1 = self.N1
        return (zdiv(x - K[i], K[i + 2] - K[i]) * N1(i, x) +
                zdiv(K[i + 3] - x, K[i + 3] - K[i + 1]) * N1(i + 1, x))

    def eval(self, x):
        return np.sum([self.coeffs[i] * self.N2(i, x) for i in range(self.num_coeff)], axis=0)

    def evald(self, x):
        return np.sum([self.coeffsd[i] * self.N1(i + 1, x) for i in range(self.num_coeffd)], axis=0)

    def evaldd(self, x):
        return np.sum([self.coeffsdd[i] * self.N0(i + 2, x) for i in range(self.num_coeffdd)], axis=0)

    def active_basis_slow(self, x):
        basis = np.array([self.N2(i, x) for i in range(self.num_coeff)])
        return np.nonzero(np.abs(basis) > 0)[0]

    def active_basis(self, x):
        end_adjustment = 0 if x < self.knots[-1] else 1
        i = self.knots[self.degree:-self.degree].searchsorted(x, side="right") - 1 - end_adjustment
        return [i, i + 1, i + 2]

    def active_basisd(self, x):
        end_adjustment = 0 if x < self.knots[-1] else 1
        i = self.knots[self.degree:-self.degree].searchsorted(x, side="right") - 1 - end_adjustment
        return [i, i + 1]

    def active_basisdd(self, x):
        end_adjustment = 0 if x < self.knots[-1] else 1
        i = self.knots[self.degree:-self.degree].searchsorted(x, side="right") - 1 - end_adjustment
        return [i]

    def active_basis_range(self, x_min, x_max):
        ab_min = self.active_basis(x_min)
        ab_max = self.active_basis(x_max)
        return list(range(ab_min[0], ab_max[-1] + 1))

    def active_basisd_range(self, x_min, x_max):
        ab_min = self.active_basisd(x_min)
        ab_max = self.active_basisd(x_max)
        return list(range(ab_min[0], ab_max[-1] + 1))

    def active_basisdd_range(self, x_min, x_max):
        ab_min = self.active_basisdd(x_min)
        ab_max = self.active_basisdd(x_max)
        return list(range(ab_min[0], ab_max[-1] + 1))

    def basisdd_support_interval(self, i):
        return [self.knots[i + 2], self.knots[i + 3]]

    def basisdd_support_interval_range(self, i_min, i_max):
        x_min = self.basisdd_support_interval(i_min)[0]
        x_max = self.basisdd_support_interval(i_max)[1]
        return [x_min, x_max]

    def tangent_segments(self, cdd_zero):
        segments = []
        curr_segment = []
        for i in range(0, self.num_coeffdd):
            curr_val = self.coeffsdd[i]
            if np.abs(curr_val) < cdd_zero:
                curr_segment.append(i)
            else:
                if len(curr_segment) > 0:
                    segments.append(curr_segment)
                curr_segment = []

        if len(curr_segment) > 0:
            segments.append(curr_segment)

        return segments

    def vertical_curve_segments(self, cdd_zero):
        segments = []

        curr_segment = []
        for i in range(0, self.num_coeffdd):
            curr_val = self.coeffsdd[i]
            if np.abs(curr_val) > cdd_zero:
                if len(curr_segment) == 0:
                    curr_segment = [i]
                elif np.sign(self.coeffsdd[curr_segment[-1]]) == np.sign(curr_val):
                    curr_segment.append(i)
                else:
                    segments.append(curr_segment)
                    curr_segment = [i]
            else:
                segments.append(curr_segment)
                curr_segment = []
        segments.append(curr_segment)

        segments = [s for s in segments if len(s) > 0]

        return segments


def __main():
    spline = QuadraticBSpline([0.0, 0.25, 0.5, 0.75, 1.0])
    for x in [0.0, 0.1, 0.25, 0.4, 0.5, 0.6, 0.75, 0.8, 1.0]:
        print(x)
        print([spline.N1(i + 1, x) for i in range(spline.num_coeffd)])
        print([i for i in spline.active_basisd(x)])
        print()

    print(spline.active_basisd_range(0.0, 0.49))


if __name__ == '__main__':
    __main()
