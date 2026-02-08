#pragma once
#include <gdal.h>

namespace LWS::Fade25D {
    public value struct GDAL {
        static void AllRegister() {
            GDALAllRegister();
        }
    };
}
