#pragma once
#include <fade25d/Fade_2D.h>
#include <vector>

#include "Triangle3.h"
#include "VertexIndexMap.h"

using namespace System;

namespace LWS::Fade25D {

    public ref class TriangleCollection {
        std::vector<GEOM_FADE25D::Triangle2*>* _triangles;

    internal:
        TriangleCollection(GEOM_FADE25D::Fade_2D* dt);

    public:
        ~TriangleCollection();
        !TriangleCollection();

        initonly int Count;
        Triangle3d GetTriangle3d(int index);
        Triangle3i GetTriangle3i(VertexIndexMap^ indexMap, int index);
    };
}
