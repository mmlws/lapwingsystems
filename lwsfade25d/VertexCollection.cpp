#include "VertexCollection.h"

using namespace LWS::Fade25D;

VertexCollection::VertexCollection(GEOM_FADE25D::Fade_2D* dt) {
    _vertices = new std::vector<GEOM_FADE25D::Point2*>();
    dt->getVertexPointers(*_vertices);
    Count = static_cast<int>(_vertices->size());
}

VertexCollection::~VertexCollection() {
    this->!VertexCollection();
}

VertexCollection::!VertexCollection() {
    if (_vertices != nullptr) {
        delete _vertices;
        _vertices = nullptr;
    }
}

GEOM_FADE25D::Point2* VertexCollection::GetPointer(int index) {
    return (*_vertices)[index];
}

Vertex3d VertexCollection::Get(int index) {
    auto* pt = (*_vertices)[index];
    return Vertex3d(pt->x(), pt->y(), pt->z());
}

VertexIndexMap^ VertexCollection::CreateIndexMap() {
    return gcnew VertexIndexMap(_vertices);
}
