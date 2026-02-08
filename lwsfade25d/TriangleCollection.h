#pragma once
#include <fade25d/Fade_2D.h>
#include <vector>

#include "TerrainTriangle.h"
using namespace System;

namespace LWS::Fade25D {

    public ref class TriangleCollection {

    internal:
        std::vector<GEOM_FADE25D::Triangle2*>* _triangles;

        TriangleCollection(GEOM_FADE25D::Fade_2D* dt) {
            _triangles = new std::vector<GEOM_FADE25D::Triangle2*>();
            dt->getTrianglePointers(*_triangles);
            Count = static_cast<int>(_triangles->size());
        }

    public:
        ~TriangleCollection() {
            this->!TriangleCollection();
        }

        !TriangleCollection() {
            if (_triangles != nullptr) {
                delete _triangles;
                _triangles = nullptr;
            }
        }

        initonly int Count;

        TerrainTriangle3d GetTriangle3d(int index) {
            auto* tri = (*_triangles)[index];
            auto* p0 = tri->getCorner(0);
            auto* p1 = tri->getCorner(1);
            auto* p2 = tri->getCorner(2);
            return TerrainTriangle3d(
                Pt3d(p0->x(), p0->y(), p0->z()),
                Pt3d(p1->x(), p1->y(), p1->z()),
                Pt3d(p2->x(), p2->y(), p2->z())
            );
        }

        TerrainTriangle3i GetTriangle3i(int index) {
            auto* tri = (*_triangles)[index];
            return TerrainTriangle3i(
                tri->getCorner(0)->getCustomIndex(),
                tri->getCorner(1)->getCustomIndex(),
                tri->getCorner(2)->getCustomIndex()
            );
        }
    };
}
