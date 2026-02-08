using LWS.Fade25D;

namespace LWS.VerticalProfileOptimisation.Tests;

public static class TerrainFactory
{
    /// <summary>
    /// Creates a flat terrain grid centered at the origin.
    /// </summary>
    /// <param name="size">Side length in metres (grid spans -size/2 to +size/2)</param>
    /// <param name="height">Constant elevation of all points</param>
    public static TerrainMesh Flat(double size, double height)
    {
        var h = size / 2.0;
        return TerrainMesh.FromPoints(
            [-h, h, h, -h],
            [-h, -h, h, h],
            [height, height, height, height]);
    }
}
