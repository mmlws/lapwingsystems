#include "GDALRegister.h"

#include <gdal.h>
using namespace LWS::Fade25D;

void GDAL::AllRegister() {
    GDALAllRegister();
}
