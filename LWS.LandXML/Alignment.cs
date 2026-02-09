using LWS.Geometry;

namespace LWS.LandXML;

/// <summary>A line segment in coordinate geometry.</summary>
public record struct CoordGeomLine(Pt2d Start, Pt2d End);

/// <summary>
/// A horizontal alignment defined by coordinate geometry.
/// </summary>
public class Alignment
{
    public required string Name { get; init; }
    public double StartStation { get; init; }
    public List<CoordGeomLine> Lines { get; init; } = [];
    public Profile? Profile { get; set; }
    public List<CrossSect> CrossSections { get; init; } = [];

    /// <summary>Total length computed from the line segments.</summary>
    public double Length => Lines.Sum(l =>
    {
        var dx = l.End.X - l.Start.X;
        var dy = l.End.Y - l.Start.Y;
        return Math.Sqrt(dx * dx + dy * dy);
    });
}
