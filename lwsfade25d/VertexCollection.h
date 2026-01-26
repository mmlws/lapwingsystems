#pragma once
#include <fade25d/Fade_2D.h>
#include <vector>

#include "Vertex3.h"
#include "VertexIndexMap.h"

using namespace System;

namespace LWS::Fade25D {

    public ref class VertexCollection {
        std::vector<GEOM_FADE25D::Point2*>* _vertices;

    internal:
        VertexCollection(GEOM_FADE25D::Fade_2D* dt);
        GEOM_FADE25D::Point2* GetPointer(int index);

    public:
        ~VertexCollection();
        !VertexCollection();

        initonly int Count;
        Vertex3d Get(int index);
        VertexIndexMap^ CreateIndexMap();
    };
}
