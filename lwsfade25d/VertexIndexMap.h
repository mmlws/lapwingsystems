#pragma once
#include <fade25d/Fade_2D.h>
#include <unordered_map>
#include <vector>

using namespace System;

namespace LWS::Fade25D {

    public ref class VertexIndexMap {
        std::unordered_map<GEOM_FADE25D::Point2*, int>* _map;

    internal:
        VertexIndexMap(std::vector<GEOM_FADE25D::Point2*>* vertices);
    public:
        int GetIndex(GEOM_FADE25D::Point2* pt);
        ~VertexIndexMap();
        !VertexIndexMap();
    };
}
