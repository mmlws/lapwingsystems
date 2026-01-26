using Rhino.FileIO;
using Rhino.Geometry;

namespace LWS.Fade25D.Rhino3dm;

public static class TerrainMeshIO
{
    public static bool AddTerrainMesh(this File3dm file3dm, TerrainMesh terrainMesh)
    {
        var mesh = new Mesh();

        var vertices = terrainMesh.GetVertices();
        for (var i = 0; i < vertices.Count; i++)
        {
            var v = vertices.Get(i);
            mesh.Vertices.Add(v.X, v.Y, v.Z);
        }

        var indexMap = vertices.CreateIndexMap();
        var triangles = terrainMesh.GetTriangles();
        for (var i = 0; i < triangles.Count; i++)
        {
            var f = triangles.GetTriangle3i(indexMap, i);
            mesh.Faces.AddFace(f.V0, f.V1, f.V2);
        }

        file3dm.Objects.AddMesh(mesh);

        return true;
    }
}