#pragma once
#include <fade25d/Fade_2D.h>

#include "VertexCollection.h"
#include "TriangleCollection.h"

using namespace System;

namespace LWS::Fade25D {
    public ref class TerrainMesh {
        size_t _num_vertices = 0;
        size_t _num_triangles = 0;

        GEOM_FADE25D::Fade_2D* _dt;
        TerrainMesh(GEOM_FADE25D::CloudPrepare* cloud);

    public:
        static TerrainMesh();
        static TerrainMesh^ FromGeoTiff(String^ path, int band);

        size_t NumVertices();
        size_t NumTriangles();
        
        VertexCollection^ GetVertices();
        TriangleCollection^ GetTriangles();

        bool WritePLY(String^ path);
        
        ~TerrainMesh();
        !TerrainMesh();
    };
}
