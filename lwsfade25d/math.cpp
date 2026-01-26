#include "math.h"

void transform_point_gdal(double const* transform, double& x, double& y) {
    const auto col = x;
    const auto row = y;
    x = transform[0] + col * transform[1] + row * transform[2];
    y = transform[3] + col * transform[4] + row * transform[5];
}