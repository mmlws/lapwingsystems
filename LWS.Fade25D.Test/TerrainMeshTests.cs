using LWS.Fade25D.Rhino3dm;
using Rhino.FileIO;

namespace LWS.Fade25D.Test;
using LWS.Fade25D;

public class TerrainMeshTests
{
    static TerrainMeshTests()
    {
        GDAL.AllRegister();
    }
    
    [Fact]
    public void Save3dm()
    {
        var fullPath = Path.GetFullPath("geotiff/prospect_small.tif");
        using var tm = TerrainMesh.FromGeoTiff(fullPath, 1);
        using var file3dm = new File3dm();
        file3dm.AddTerrainMesh(tm);
        file3dm.Write("prospect.3dm", new File3dmWriteOptions());
    }
}