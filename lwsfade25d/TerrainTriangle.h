#pragma once

namespace LWS::Fade25D {

    public value struct TerrainTriangle3d {
        Pt3d V0;
        Pt3d V1;
        Pt3d V2;

        TerrainTriangle3d(Pt3d v0, Pt3d v1, Pt3d v2) : V0(v0), V1(v1), V2(v2) {}
    };
    
    public value struct TerrainTriangle3i {
        int V0;
        int V1;
        int V2;
        
        TerrainTriangle3i(const int v0, const int v1, const int v2) : V0(v0), V1(v1), V2(v2) {}
    };
}
