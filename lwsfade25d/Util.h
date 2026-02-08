#pragma once
#include <fade25d/freeFunctions.h>

using namespace System;

namespace LWS::Fade25D {
    public ref class Util {
    public:
        static String^ GetFade25DVersion() {
            const char* nativeVersion = GEOM_FADE25D::getFade2DVersion();
            return gcnew String(nativeVersion);
        }
    };
}
