namespace LWS.Geometry;

public struct Vec2d(double dx, double dy)
{
    public double Dx = dx;
    public double Dy = dy;

    public double Length()
        => Math.Sqrt(Dx * Dx + Dy * Dy);

    public Vec2d Normalise()
    {
        var len = Length();
        return new(Dx / len, Dy / len);
    }

    public Vec2d RotateCcw()
        => new(Dy, -Dx);

    public Vec2d RotateCw()
        => new(-Dy, Dx);

    public Vec2d Flip()
        => new(-Dx, -Dy);

    public static double Dot(Vec2d a, Vec2d b)
        => a.Dx * b.Dx + a.Dy * b.Dy;
}