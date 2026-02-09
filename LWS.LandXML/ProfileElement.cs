namespace LWS.LandXML;

/// <summary>
/// Base class for element-based vertical profile geometry.
/// A profile can be defined as a sequence of these elements (tangents and curves)
/// instead of PVI points.
/// </summary>
public abstract class ProfileElement
{
    public abstract double StartStation { get; }
    public abstract double EndStation { get; }
    public double Length => EndStation - StartStation;

    public abstract double ElevationAt(double station);
    public abstract double GradeAt(double station);

    public double StartElevation => ElevationAt(StartStation);
    public double EndElevation => ElevationAt(EndStation);
    public double StartGrade => GradeAt(StartStation);
    public double EndGrade => GradeAt(EndStation);
}

/// <summary>
/// A constant-grade tangent segment.
/// Elevation varies linearly: elev = startElev + grade * (sta - startSta).
/// </summary>
public class TangentElement : ProfileElement
{
    public override double StartStation { get; }
    public override double EndStation { get; }
    readonly double _startElevation;
    readonly double _grade;

    public TangentElement(double startStation, double endStation, double startElevation, double grade)
    {
        StartStation = startStation;
        EndStation = endStation;
        _startElevation = startElevation;
        _grade = grade;
    }

    public double Grade => _grade;

    public override double ElevationAt(double station) =>
        _startElevation + _grade * (station - StartStation);

    public override double GradeAt(double station) => _grade;
}

/// <summary>
/// A symmetric parabolic vertical curve.
/// Grade varies linearly from startGrade to endGrade over the curve length.
/// K-value = Length / |endGrade - startGrade|.
/// </summary>
public class ParabolicElement : ProfileElement
{
    public override double StartStation { get; }
    public override double EndStation { get; }
    readonly double _startElevation;
    readonly double _startGrade;
    readonly double _endGrade;

    public ParabolicElement(double startStation, double endStation, double startElevation,
        double startGrade, double endGrade)
    {
        StartStation = startStation;
        EndStation = endStation;
        _startElevation = startElevation;
        _startGrade = startGrade;
        _endGrade = endGrade;
    }

    /// <summary>Rate of curvature: length per unit grade change.</summary>
    public double KValue => Math.Abs(_endGrade - _startGrade) > 1e-12
        ? Length / (_endGrade - _startGrade)
        : double.PositiveInfinity;

    public override double ElevationAt(double station)
    {
        var ds = station - StartStation;
        var len = Length;
        return len > 0
            ? _startElevation + _startGrade * ds + (_endGrade - _startGrade) / (2 * len) * ds * ds
            : _startElevation;
    }

    public override double GradeAt(double station)
    {
        var ds = station - StartStation;
        var len = Length;
        return len > 0
            ? _startGrade + (_endGrade - _startGrade) / len * ds
            : _startGrade;
    }
}

/// <summary>
/// A circular vertical curve (small-angle approximation).
/// Positive radius = sag curve, negative radius = crest curve.
/// Grade varies linearly: grade = startGrade + ds / radius.
/// </summary>
public class CircularElement : ProfileElement
{
    public override double StartStation { get; }
    public override double EndStation { get; }
    readonly double _startElevation;
    readonly double _startGrade;
    readonly double _radius;

    public CircularElement(double startStation, double endStation, double startElevation,
        double startGrade, double radius)
    {
        StartStation = startStation;
        EndStation = endStation;
        _startElevation = startElevation;
        _startGrade = startGrade;
        _radius = radius;
    }

    /// <summary>Signed radius. Positive = sag, negative = crest.</summary>
    public double Radius => _radius;

    public override double ElevationAt(double station)
    {
        var ds = station - StartStation;
        return _startElevation + _startGrade * ds + ds * ds / (2 * _radius);
    }

    public override double GradeAt(double station)
    {
        var ds = station - StartStation;
        return _startGrade + ds / _radius;
    }
}
