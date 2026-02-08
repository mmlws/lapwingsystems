#pragma once
#include <fade25d/Fade_2D.h>
#include <vector>

using namespace System;
using namespace LWS::Geometry;

namespace LWS::Fade25D {

    public ref class VertexCollection {
        std::vector<GEOM_FADE25D::Point2*>* _vertices;

    internal:
        VertexCollection(GEOM_FADE25D::Fade_2D* dt) {
            _vertices = new std::vector<GEOM_FADE25D::Point2*>();
            dt->getVertexPointers(*_vertices);
            Count = static_cast<int>(_vertices->size());
        }

        GEOM_FADE25D::Point2* GetPointer(int index) {
            return (*_vertices)[index];
        }

    public:
        ~VertexCollection() {
            this->!VertexCollection();
        }

        !VertexCollection() {
            if (_vertices != nullptr) {
                delete _vertices;
                _vertices = nullptr;
            }
        }

        initonly int Count;

        Pt3d GetPosition(int index) {
            auto* pt = (*_vertices)[index];
            return Pt3d(pt->x(), pt->y(), pt->z());
        }

    };
}
