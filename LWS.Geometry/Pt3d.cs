namespace LWS.Geometry;

public struct Pt3d(double x, double y, double z)
{
    public double X = x;
    public double Y = y;
    public double Z = z;

    public Pt3d Add(Vec3d v)
        => new(X + v.Dx, Y + v.Dy, Z + v.Dz);

    public Pt3d AddLengthSlope(Vec2d v, double length, double slope)
        => new(X + v.Dx * length, Y + v.Dy * length, Z + length * slope);

    public Pt3d AddLengthHeight(Vec2d v, double length, double height)
        => new(X + v.Dx * length, Y + v.Dy * length, Z + height);

    public double Distance2d(Pt3d other)
        => Math.Sqrt((X - other.X) * (X - other.X) + (Y - other.Y) * (Y - other.Y));
}