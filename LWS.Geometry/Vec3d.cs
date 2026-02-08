namespace LWS.Geometry;

public struct Vec3d(double dx, double dy, double dz)
{
    public double Dx = dx;
    public double Dy = dy;
    public double Dz = dz;

    public double Length()
        => Math.Sqrt(Dx * Dx + Dy * Dy + Dz * Dz);

    public Vec3d Normalise()
    {
        var len = Length();
        return new(Dx / len, Dy / len, Dz / len);
    }

    public Vec3d RotateZCcw()
        => new(-Dy, Dx, Dz);

    public Vec3d RotateZCw()
        => new(Dy, -Dx, Dz);

    public Vec3d Flip()
        => new(-Dx, -Dy, -Dz);

    public static double Dot(Vec3d a, Vec3d b)
        => a.Dx * b.Dx + a.Dy * b.Dy + a.Dz * b.Dz;

    public static Vec3d Cross(Vec3d a, Vec3d b)
        => new(a.Dy * b.Dz - a.Dz * b.Dy,
            a.Dz * b.Dx - a.Dx * b.Dz,
            a.Dx * b.Dy - a.Dy * b.Dx);
}