#include "util.hpp"
#include <fade25d/freeFunctions.h>

using namespace System;
using namespace LWSFade25D;

String^ Util::GetVersion()
{
    // Call native Fade25D function
    const char* nativeVersion = GEOM_FADE25D::getFade2DVersion();

    // Convert to managed string
    return gcnew String(nativeVersion);
}
