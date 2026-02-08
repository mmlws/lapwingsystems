using LWS.Fade25D;
using LWS.Geometry;

namespace LWS.VerticalProfileOptimisation;

public delegate double GetTerrainHeightFn(double x, double y);

public delegate Pt3d? RayShootFn(Pt3d from, Vec3d dir);

public struct CutFillAreas(double cut, double fill)
{
    public double Cut = cut;
    public double Fill = fill;
}

public class XSectionPoints
{
    public List<Pt3d> Left = [];
    public List<Pt3d> Right = [];

    public void Clear()
    {
        Left.Clear();
        Right.Clear();
    }
}

public class XSectionCalculator
{
    readonly TerrainMesh _terrain;
    readonly RayShooter _rayShooter;
    readonly XSectionPoints _xsPoints = new();

    public XSectionCalculator(TerrainMesh terrain)
    {
        _terrain = terrain;
        _rayShooter = new RayShooter(terrain.GetTriangles());
    }

    public void ComputePoints(XSectionParams xs, Frame3d frame, XSectionPoints result)
    {
        result.Clear();

        var centre = frame.Origin;
        var right = new Vec2d(frame.XAxis.Dx, frame.XAxis.Dy);
        var left = right.Flip();

        // Adjust for strip height (same as pushing terrain down)
        centre.Z += xs.Strip;

        // Left side
        result.Left.Add(centre);
        var laneEnd = centre.AddLengthSlope(left, xs.LaneWidth / 2.0, -xs.LaneCrossFall);
        result.Left.Add(laneEnd);
        var shoulderEnd = laneEnd.AddLengthSlope(left, xs.ShoulderWidth, -xs.ShoulderSlope);
        result.Left.Add(shoulderEnd);
        var ditchBottom = shoulderEnd.AddLengthHeight(left, xs.DitchWidth, -xs.DitchDepth);
        var ditchTz = _terrain.GetHeight(ditchBottom);
        if (ditchTz > ditchBottom.Z)
            result.Left.Add(ditchBottom);

        // Right side
        result.Right.Add(centre);
        laneEnd = centre.AddLengthSlope(right, xs.LaneWidth / 2.0, -xs.LaneCrossFall);
        result.Right.Add(laneEnd);
        shoulderEnd = laneEnd.AddLengthSlope(right, xs.ShoulderWidth, -xs.ShoulderSlope);
        result.Right.Add(shoulderEnd);
        ditchBottom = shoulderEnd.AddLengthHeight(right, xs.DitchWidth, -xs.DitchDepth);
        ditchTz = _terrain.GetHeight(ditchBottom);
        if (ditchTz > ditchBottom.Z)
            result.Right.Add(ditchBottom);

        // Shift back down for strip
        for (var i = 0; i < result.Left.Count; i++)
        {
            var pt = result.Left[i];
            result.Left[i] = new Pt3d(pt.X, pt.Y, pt.Z - xs.Strip);
        }

        for (var i = 0; i < result.Right.Count; i++)
        {
            var pt = result.Right[i];
            result.Right[i] = new Pt3d(pt.X, pt.Y, pt.Z - xs.Strip);
        }
    }

    CutFillAreas ComputeCutFillAreasInternal(List<Pt3d> points)
    {
        var cutArea = 0.0;
        var fillArea = 0.0;

        var p1 = points[0];
        var h1 = p1.Z - _terrain.GetHeight(p1);

        for (var i = 1; i < points.Count; i++)
        {
            var p2 = points[i];
            var distance = p1.Distance2d(p2);
            var h2 = p2.Z - _terrain.GetHeight(p2);

            if (h1 <= 0 && h2 <= 0)
            {
                // Both below ground
                cutArea -= 0.5 * distance * (h2 + h1);
            }
            else if (h1 >= 0 && h2 >= 0)
            {
                // Both above ground
                fillArea += 0.5 * distance * (h2 + h1);
            }
            else if (h1 <= 0 && h2 >= 0)
            {
                // Below to above
                var d = RootFind.LinearFn(0, h1, distance, h2);
                cutArea -= d * h1;
                fillArea += d * h2;
            }
            else if (h1 >= 0 && h2 <= 0)
            {
                // Above to below
                var d = RootFind.LinearFn(0, h1, distance, h2);
                fillArea += d * h1;
                cutArea -= d * h2;
            }

            p1 = p2;
            h1 = h2;
        }

        return new CutFillAreas(cutArea, fillArea);
    }

    public CutFillAreas ComputeCutFillAreas(XSectionParams xs, Frame3d frame)
    {
        ComputePoints(xs, frame, _xsPoints);
        var leftAreas = ComputeCutFillAreasInternal(_xsPoints.Left);
        var rightAreas = ComputeCutFillAreasInternal(_xsPoints.Right);
        return new CutFillAreas(
            leftAreas.Cut + rightAreas.Cut,
            leftAreas.Fill + rightAreas.Fill);
    }

    public CutFillAreas[] ComputeCutFillAreasAtOffsets(
        HorizontalAlignment ha, XSectionParams xs, double station,
        double[] offsets)
    {
        var f2d = ha.FrameAt(station);
        var terrainHeight = _terrain.GetHeight(f2d.Origin);
        var f3d = f2d.ToFrame3d();

        var results = new CutFillAreas[offsets.Length];

        for (var i = 0; i < offsets.Length; i++)
        {
            f3d.Origin.Z = terrainHeight + offsets[i];
            results[i] = ComputeCutFillAreas(xs, f3d);
        }

        return results;
    }

    /// <summary>
    /// Fit a quadratic to cut area vs offset samples (positive areas only).
    /// Returns coefficients [c, b, a] where area = c + b*u + a*u²,
    /// and uOffset = the root where the quadratic crosses zero (lower root).
    /// </summary>
    public void QuadraticFitCutArea(double[] offsets, double[] areas, out double[] posCoeffs, out double uOffset)
    {
        // Filter to positive area samples
        var filtU = new List<double>();
        var filtV = new List<double>();
        for (var i = 0; i < offsets.Length; i++)
        {
            if (areas[i] > 0)
            {
                filtU.Add(offsets[i]);
                filtV.Add(areas[i]);
            }
        }

        alglib.polynomialfit(filtU.ToArray(), filtV.ToArray(), filtU.Count, 3, out var poly, out _);
        alglib.polynomialbar2pow(poly, 0.0, 1.0, out var coeffArr);

        var c = coeffArr[0];
        var b = coeffArr[1];
        var a = coeffArr[2];

        // Lower root: (-b - sqrt(b²-4ac)) / 2a
        uOffset = (-b - Math.Sqrt(b * b - 4 * a * c)) / (2 * a);
        posCoeffs = coeffArr;
    }

    /// <summary>
    /// Fit a quadratic to fill area vs offset samples (positive areas only).
    /// Returns coefficients [c, b, a] where area = c + b*u + a*u²,
    /// and uOffset = the root where the quadratic crosses zero (upper root).
    /// </summary>
    public void QuadraticFitFillArea(double[] offsets, double[] areas, out double[] posCoeffs, out double uOffset)
    {
        // Filter to positive area samples
        var filtU = new List<double>();
        var filtV = new List<double>();
        for (var i = 0; i < offsets.Length; i++)
        {
            if (areas[i] > 0)
            {
                filtU.Add(offsets[i]);
                filtV.Add(areas[i]);
            }
        }

        alglib.polynomialfit(filtU.ToArray(), filtV.ToArray(), filtU.Count, 3, out var poly, out _);
        alglib.polynomialbar2pow(poly, 0.0, 1.0, out var coeffArr);

        var c = coeffArr[0];
        var b = coeffArr[1];
        var a = coeffArr[2];

        // Upper root: (-b + sqrt(b²-4ac)) / 2a
        uOffset = (-b + Math.Sqrt(b * b - 4 * a * c)) / (2 * a);
        posCoeffs = coeffArr;
    }
}