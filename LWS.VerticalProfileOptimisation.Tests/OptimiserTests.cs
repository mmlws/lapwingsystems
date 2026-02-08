using LWS.Geometry;

namespace LWS.VerticalProfileOptimisation.Tests;

public class OptimiserTests
{
    [Fact]
    public void FlatTerrain_StraightRoad_ProfileIsFlat()
    {
        // Flat terrain: 1000x1000m at z=123.456
        using var terrain = TerrainFactory.Flat(size: 1000, height: 123.456);

        // Straight 500m road along X axis, centered at origin
        var ha = new HorizontalAlignment();
        ha.AddPoint(new Pt2d(-250, 0));
        ha.AddPoint(new Pt2d(250, 0));
        ha.Refresh();

        var cost = new OptCostModel();
        var model = new OptModel
        {
            AccessTracks =
            [
                new AccessTrackOptModel
                {
                    Alignment = ha,
                    XSectionParams = new XSectionParams(),
                    CostModel = cost
                }
            ],
            CostModels = [cost],
        };

        var optimiser = new Optimiser();
        var result = optimiser.Optimise(terrain, model);

        Assert.NotNull(result);
        Assert.Empty(optimiser.Errors);
        Assert.Single(result.VerticalProfiles);

        var profile = result.VerticalProfiles[0];
        for (var i = 0; i < profile.NumSegments; i++)
        {
            Assert.True(Math.Abs(profile.A1[i]) < 0.01,
                $"Segment {i}: expected near-zero grade, got A1={profile.A1[i]}");
            Assert.True(Math.Abs(profile.A2[i]) < 0.01,
                $"Segment {i}: expected near-zero curvature, got A2={profile.A2[i]}");
        }
    }
}
