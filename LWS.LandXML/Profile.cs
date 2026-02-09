namespace LWS.LandXML;

/// <summary>A Point of Vertical Intersection (station + elevation).</summary>
public record struct PVI(double Station, double Elevation);

/// <summary>
/// A vertical profile. Can be defined by PVI points, element-based geometry, or both.
/// </summary>
public class Profile
{
    public required string Name { get; init; }
    public List<PVI> PVIs { get; init; } = [];
    public List<ProfileElement> Elements { get; init; } = [];

    /// <summary>True if this profile has element-based geometry.</summary>
    public bool HasElements => Elements.Count > 0;

    /// <summary>Evaluate elevation at a station using the element list. Returns null if station is outside all elements.</summary>
    public double? ElevationAt(double station)
    {
        foreach (var el in Elements)
        {
            if (station >= el.StartStation && station <= el.EndStation)
                return el.ElevationAt(station);
        }

        return null;
    }

    /// <summary>Evaluate grade at a station using the element list. Returns null if station is outside all elements.</summary>
    public double? GradeAt(double station)
    {
        foreach (var el in Elements)
        {
            if (station >= el.StartStation && station <= el.EndStation)
                return el.GradeAt(station);
        }

        return null;
    }

    /// <summary>Station range of the element-based profile.</summary>
    public (double start, double end) StationRange =>
        Elements.Count > 0
            ? (Elements[0].StartStation, Elements[^1].EndStation)
            : PVIs.Count > 0
                ? (PVIs[0].Station, PVIs[^1].Station)
                : (0, 0);
}
