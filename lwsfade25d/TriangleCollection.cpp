#include "TriangleCollection.h"

using namespace LWS::Fade25D;

TriangleCollection::TriangleCollection(GEOM_FADE25D::Fade_2D* dt) {
    _triangles = new std::vector<GEOM_FADE25D::Triangle2*>();
    dt->getTrianglePointers(*_triangles);
    Count = _triangles->size();
}

TriangleCollection::~TriangleCollection() {
    this->!TriangleCollection();
}

TriangleCollection::!TriangleCollection() {
    if (_triangles != nullptr) {
        delete _triangles;
        _triangles = nullptr;
    }
}

Triangle3d TriangleCollection::GetTriangle3d(int index) {
    auto* tri = (*_triangles)[index];
    auto* p0 = tri->getCorner(0);
    auto* p1 = tri->getCorner(1);
    auto* p2 = tri->getCorner(2);
    return Triangle3d(
        Vertex3d(p0->x(), p0->y(), p0->z()),
        Vertex3d(p1->x(), p1->y(), p1->z()),
        Vertex3d(p2->x(), p2->y(), p2->z())
    );
}

Triangle3i TriangleCollection::GetTriangle3i(VertexIndexMap^ indexMap, int index) {
    auto* tri = (*_triangles)[index];
    return Triangle3i(
        indexMap->GetIndex(tri->getCorner(0)),
        indexMap->GetIndex(tri->getCorner(1)),
        indexMap->GetIndex(tri->getCorner(2))
    );
}

