#pragma once
#include <algorithm>
#include <limits>
#include <gdal.h>
#include <fade25d/Fade_2D.h>
#include <msclr/marshal_cppstd.h>

#include "VertexCollection.h"
#include "TriangleCollection.h"

using namespace System;
using namespace GEOM_FADE25D;

namespace LWS::Fade25D {
    public ref class TerrainMesh {
        int _num_vertices = 0;
        int _num_triangles = 0;
        double _min_height;

        Fade_2D* _dt;

        TerrainMesh(CloudPrepare* cloud) {
            _dt = new Fade_2D();
            _dt->insert(cloud, true);
            _num_vertices = _dt->numberOfPoints();
            _num_triangles = _dt->numberOfTriangles();
            _min_height = FindMinHeight(_dt);
            IndexVertices(_dt);
        }

        static void IndexVertices(Fade_2D* dt) {
            std::vector<Point2*> vertices;
            dt->getVertexPointers(vertices);
            for (int i = 0; i < static_cast<int>(vertices.size()); i++)
                vertices[i]->setCustomIndex(i);
        }

        static double FindMinHeight(Fade_2D* dt) {
            double min_h = (std::numeric_limits<double>::max)();
            std::vector<Point2*> vertices;
            dt->getVertexPointers(vertices);
            for (auto const v : vertices)
                min_h = std::min<double>(v->z(), min_h);
            return min_h;
        }
        
        static void TransformPointGdal(double const* transform, double& x, double& y) {
            const auto col = x;
            const auto row = y;
            x = transform[0] + col * transform[1] + row * transform[2];
            y = transform[3] + col * transform[4] + row * transform[5];
        }

    public:
        static TerrainMesh() {
            GDALAllRegister();
        }

        static TerrainMesh^ FromGeoTiff(String^ path, int band) {
            auto path_str = msclr::interop::marshal_as<std::string>(path);

            auto dataset = GDALOpen(path_str.c_str(), GA_ReadOnly);
            auto raster_band = GDALGetRasterBand(dataset, band);

            auto num_cols = GDALGetRasterXSize(dataset);
            auto num_rows = GDALGetRasterYSize(dataset);
            auto buffer = std::unique_ptr<float>(new float[num_cols * num_rows]);
            GDALRasterIO(raster_band, GF_Read, 0, 0, num_cols, num_rows, buffer.get(),
                         num_cols, num_rows, GDT_Float32, 0, 0);

            auto no_data_value = GDALGetRasterNoDataValue(raster_band, nullptr);

            double transform[6];
            GDALGetGeoTransform(dataset, transform);

            GDALClose(dataset);

            CloudPrepare cloud;
            auto p_buffer = buffer.get();
            for (auto r = 0; r < num_rows; r++)
                for (auto c = 0; c < num_cols; c++) {
                    double y = r;
                    double x = c;
                    auto z = *p_buffer;
                    if (z != no_data_value) {
                        TransformPointGdal(transform, x, y);
                        cloud.add(x, y, z);
                    }
                    p_buffer++;
                }

            return gcnew TerrainMesh(&cloud);
        }

        size_t NumVertices() {
            return _num_vertices;
        }

        size_t NumTriangles() {
            return _num_triangles;
        }

        bool WritePLY(String^ path) {
            auto path_str = msclr::interop::marshal_as<std::string>(path);
            return _dt->writePly(path_str.c_str(), false);
        }

        VertexCollection^ GetVertices() {
            return gcnew VertexCollection(_dt);
        }

        TriangleCollection^ GetTriangles() {
            return gcnew TriangleCollection(_dt);
        }
        
        double GetHeight(double x, double y) {
            double h;
            auto result = _dt->getHeight(x, y, h);
            if (result == false)
                return _min_height;
            return h;
        }
        
        double GetHeight(Pt2d p) {
            return GetHeight(p.X, p.Y);
        }
        
        double GetHeight(Pt3d p) {
            return GetHeight(p.X, p.Y);
        }

        ~TerrainMesh() {
            this->!TerrainMesh();
        }

        !TerrainMesh() {
            if (_dt != nullptr) {
                delete _dt;
                _dt = nullptr;
            }
        }
    };
}
