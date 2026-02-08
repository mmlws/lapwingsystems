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
    public void Clear() { Left.Clear(); Right.Clear(); }
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

    public CutFillAreas ComputeAreas(XSectionParams xs, Frame3d frame)
    {
        ComputePoints(xs, frame, _xsPoints);
        var leftAreas = ComputeCutFillAreasInternal(_xsPoints.Left);
        var rightAreas = ComputeCutFillAreasInternal(_xsPoints.Right);
        return new CutFillAreas(
            leftAreas.Cut + rightAreas.Cut,
            leftAreas.Fill + rightAreas.Fill);
    }

    public CutFillAreas[] ComputeAreasAtOffsets(
        HorizontalAlignment ha, double station, XSectionParams xs,
        double minOffset, double maxOffset, int count)
    {
        var f2d = ha.FrameAt(station);
        var terrainHeight = _terrain.GetHeight(f2d.Origin);
        var f3d = f2d.ToFrame3d();

        var results = new CutFillAreas[count];
        var stepSize = (maxOffset - minOffset) / (count - 1);

        for (var i = 0; i < count; i++)
        {
            var offset = minOffset + stepSize * i;
            f3d.Origin.Z = terrainHeight + offset;
            results[i] = ComputeAreas(xs, f3d);
        }

        return results;
    }
}
