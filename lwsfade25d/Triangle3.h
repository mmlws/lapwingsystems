#pragma once
#include "VertexCollection.h"

namespace LWS::Fade25D {

    public value struct Triangle3d {
        Vertex3d V0;
        Vertex3d V1;
        Vertex3d V2;

        Triangle3d(Vertex3d v0, Vertex3d v1, Vertex3d v2) : V0(v0), V1(v1), V2(v2) {}
    };
    
    public value struct Triangle3i {
        int V0;
        int V1;
        int V2;
        
        Triangle3i(const int v0, const int v1, const int v2) : V0(v0), V1(v1), V2(v2) {}
    };
}
