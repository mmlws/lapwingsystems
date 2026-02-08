#pragma once

#include "TriangleCollection.h"

using namespace GEOM_FADE25D;
using namespace LWS::Geometry;

namespace LWS::Fade25D {
    public ref class RayShooter {
        
        std::vector<std::pair<Triangle2*, Point2>>* _result;
        GEOM_FADE25D::RayShooter* _rs;

    public:
        RayShooter(TriangleCollection^ triangles) {
            _result = new std::vector<std::pair<Triangle2*, Point2>>();
            _rs = new GEOM_FADE25D::RayShooter(*triangles->_triangles);
        }

        bool FirstIntersection(Pt3d from, Vec3d dir, Pt3d% intersection) {
            auto seg = Segment2(
                Point2(from.X, from.Y, from.Z),
                Point2(from.X + dir.Dx, from.Y + dir.Dy, from.Z + dir.Dz));

            _result->clear();
            _rs->getIntersections(seg, RSQ_DOUBLE_PRECISION, *_result);

            if (_result->empty()) {
                return false;
            }

            auto first = _result->front().second;
            intersection.X = first.x();
            intersection.Y = first.y();
            intersection.Z = first.z();

            return true;
        }

        ~RayShooter() {
            this->!RayShooter();
        }

        !RayShooter() {
            if (_rs != nullptr) {
                delete _rs;
                _rs = nullptr;
            }
            if (_result != nullptr) {
                delete _result;
                _result = nullptr;
            }
        }
    };
}
