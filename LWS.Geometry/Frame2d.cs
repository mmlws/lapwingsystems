namespace LWS.Geometry;

public struct Frame2d
{
    public Pt2d Origin;
    public Vec2d XAxis;
    public Vec2d YAxis;

    public Frame2d(Pt2d origin, Vec2d xaxis)
    {
        Origin = origin;
        XAxis = xaxis;
        YAxis = xaxis.RotateCcw();
    }

    public Frame3d ToFrame3d()
        => new(new Pt3d(Origin.X, Origin.Y, 0),
            new Vec3d(XAxis.Dx, XAxis.Dy, 0),
            new Vec3d(YAxis.Dx, YAxis.Dy, 0));
}