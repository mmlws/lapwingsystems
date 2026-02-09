using LWS.Geometry;

namespace LWS.LandXML;

/// <summary>
/// A horizontal alignment defined by coordinate geometry.
/// </summary>
public class Alignment
{
    public required string Name { get; init; }
    public double StartStation { get; init; }
    public List<CoordGeomElement> Elements { get; init; } = [];
    public Profile? Profile { get; set; }
    public List<CrossSect> CrossSections { get; init; } = [];

    /// <summary>Total length computed from the geometry elements.</summary>
    public double Length => Elements.Sum(e => e.Length);
}


public abstract class CoordGeomElement
{
    public abstract Pt2d Start { get; }
    public abstract Pt2d End { get; }
    public abstract double Length { get; }
}


/// <summary>A line segment in coordinate geometry.</summary>
public class CoordGeomLine(Pt2d start, Pt2d end) : CoordGeomElement
{
    public override Pt2d Start { get; } = start;
    public override Pt2d End { get; } = end;
    public override double Length { get; } = Math.Sqrt(
        (end.X - start.X) * (end.X - start.X) +
        (end.Y - start.Y) * (end.Y - start.Y));
}

public class CoordGeomCurve : CoordGeomElement
{
    public override Pt2d Start { get; }
    public override Pt2d End { get; }
    public Pt2d Center { get; }
    public RotationDirection Rotation { get; }
    public double Radius { get; }
    public override double Length { get; }

    public CoordGeomCurve(Pt2d start, Pt2d end, Pt2d center, RotationDirection rot)
    {
        Start = start;
        End = end;
        Center = center;
        Rotation = rot;
        Radius = Math.Sqrt(
            (start.X - center.X) * (start.X - center.X) +
            (start.Y - center.Y) * (start.Y - center.Y));

        var startAngle = Math.Atan2(start.Y - center.Y, start.X - center.X);
        var endAngle = Math.Atan2(end.Y - center.Y, end.X - center.X);
        var sweep = endAngle - startAngle;

        if (rot == RotationDirection.Cw)
        {
            // Clockwise: sweep should be negative
            if (sweep > 0) sweep -= 2 * Math.PI;
        }
        else
        {
            // Counter-clockwise: sweep should be positive
            if (sweep < 0) sweep += 2 * Math.PI;
        }

        Length = Math.Abs(sweep) * Radius;
    }
}
