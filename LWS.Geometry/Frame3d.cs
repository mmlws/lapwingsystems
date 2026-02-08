namespace LWS.Geometry;

public struct Frame3d
{
    public Pt3d Origin;
    public Vec3d XAxis;
    public Vec3d YAxis;
    public Vec3d ZAxis;

    public Frame3d(Pt3d origin, Vec3d xaxis, Vec3d yaxis)
    {
        Origin = origin;
        XAxis = xaxis;
        YAxis = Vec3d.Cross(Vec3d.Cross(xaxis, yaxis), xaxis).Normalise();
        ZAxis = Vec3d.Cross(XAxis, YAxis);
    }

    public Frame3d(Pt2d origin, Vec2d xaxis, Vec2d yaxis)
    {
        Origin = new Pt3d(origin.X, origin.Y, 0);
        XAxis = new Vec3d(xaxis.Dx, xaxis.Dy, 0);
        YAxis = new Vec3d(yaxis.Dx, yaxis.Dy, 0);
        YAxis = Vec3d.Cross(Vec3d.Cross(XAxis, YAxis), XAxis).Normalise();
        ZAxis = Vec3d.Cross(XAxis, YAxis);
    }
}