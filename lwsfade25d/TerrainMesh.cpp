#include <gdal.h>
#include <fade25d/Fade_2D.h>
#include <msclr/marshal_cppstd.h>

#include "TerrainMesh.h"
#include "./math.h"

using namespace GEOM_FADE25D;
using namespace LWS::Fade25D;
using namespace Runtime::InteropServices;

TerrainMesh::TerrainMesh(CloudPrepare* cloud) {
    _dt = new Fade_2D();
    _dt->insert(cloud, true);
    _num_vertices = _dt->numberOfPoints();
    _num_triangles = _dt->numberOfTriangles();
}

static TerrainMesh::TerrainMesh() {
    GDALAllRegister();
}

TerrainMesh^ TerrainMesh::FromGeoTiff(String^ path, int band) {
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
                transform_point_gdal(transform, x, y);
                cloud.add(x, y, z);
            }
            p_buffer++;
        }

    return gcnew TerrainMesh(&cloud);
}

size_t TerrainMesh::NumVertices() {
    return _num_vertices;
}

size_t TerrainMesh::NumTriangles() {
    return _num_triangles;
}

bool TerrainMesh::WritePLY(String^ path) {
    auto path_str = msclr::interop::marshal_as<std::string>(path);
    return _dt->writePly(path_str.c_str(), false);
}

TerrainMesh::~TerrainMesh() {
    this->!TerrainMesh();
}

void TerrainMesh::!TerrainMesh() {
    if (_dt != nullptr) {
        delete _dt;
        _dt = nullptr;
    }
}

VertexCollection^ TerrainMesh::GetVertices() {
    return gcnew VertexCollection(_dt);
}

TriangleCollection^ TerrainMesh::GetTriangles() {
    return gcnew TriangleCollection(_dt);
}
