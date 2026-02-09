namespace LWS.LandXML;

/// <summary>A point on a cross-section surface (offset from centreline + elevation).</summary>
public record struct CrossSectPnt(double Offset, double Elevation);

/// <summary>A named surface within a cross-section (e.g. "Design", "Ground").</summary>
public class CrossSectSurf
{
    public required string Name { get; init; }
    public List<CrossSectPnt> Points { get; init; } = [];
}

/// <summary>
/// A cross-section at a specific station.
/// </summary>
public class CrossSect
{
    public double Station { get; init; }
    public List<CrossSectSurf> Surfaces { get; init; } = [];
}
