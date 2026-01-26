#include "VertexIndexMap.h"

using namespace LWS::Fade25D;

VertexIndexMap::VertexIndexMap(std::vector<GEOM_FADE25D::Point2*>* vertices) {
    _map = new std::unordered_map<GEOM_FADE25D::Point2*, int>();
    for (size_t i = 0; i < vertices->size(); i++) {
        (*_map)[(*vertices)[i]] = static_cast<int>(i);
    }
}

VertexIndexMap::~VertexIndexMap() {
    this->!VertexIndexMap();
}

VertexIndexMap::!VertexIndexMap() {
    if (_map != nullptr) {
        delete _map;
        _map = nullptr;
    }
}

int VertexIndexMap::GetIndex(GEOM_FADE25D::Point2* pt) {
    return (*_map)[pt];
}
