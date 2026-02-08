namespace LWS.Geometry;

public static class RootFind
{
    /// <summary>
    /// Finds the X-intercept of the line through (x1,y1) and (x2,y2).
    /// </summary>
    public static double LinearFn(double x1, double y1, double x2, double y2)
        => x1 - y1 * (x2 - x1) / (y2 - y1);
}
