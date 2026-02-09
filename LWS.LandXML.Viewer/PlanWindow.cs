using LWS.Geometry;
using Raylib_cs;

namespace LWS.LandXML.Viewer;

/// <summary>
/// Draws horizontal alignment elements (lines and arcs) in plan view (X,Y world coordinates).
/// </summary>
public class PlanWindow : ViewportWindow
{
    static readonly Color[] AlignmentColors =
    [
        Color.SkyBlue, Color.Lime, Color.Orange, Color.Magenta,
        Color.Gold, Color.Pink, Color.Violet, Color.Beige
    ];

    public LandXmlDocument? Document { get; set; }

    public PlanWindow() : base("Plan View")
    {
    }

    protected override void DrawContent()
    {
        if (Document == null) return;

        for (var i = 0; i < Document.Alignments.Count; i++)
        {
            var alignment = Document.Alignments[i];
            var color = AlignmentColors[i % AlignmentColors.Length];

            foreach (var element in alignment.Elements)
            {
                switch (element)
                {
                    case CoordGeomLine line:
                    {
                        var s = WorldToScreen(line.Start.X, line.Start.Y);
                        var e = WorldToScreen(line.End.X, line.End.Y);
                        Raylib.DrawLineV(
                            new System.Numerics.Vector2(s.X, s.Y),
                            new System.Numerics.Vector2(e.X, e.Y),
                            color);
                        break;
                    }
                    case CoordGeomCurve curve:
                    {
                        const int segments = 32;
                        var startAngle = Math.Atan2(curve.Start.Y - curve.Center.Y, curve.Start.X - curve.Center.X);
                        var endAngle = Math.Atan2(curve.End.Y - curve.Center.Y, curve.End.X - curve.Center.X);
                        var sweep = endAngle - startAngle;

                        if (curve.Rotation == RotationDirection.Cw)
                        {
                            if (sweep > 0) sweep -= 2 * Math.PI;
                        }
                        else
                        {
                            if (sweep < 0) sweep += 2 * Math.PI;
                        }

                        var prev = WorldToScreen(curve.Start.X, curve.Start.Y);
                        for (var seg = 1; seg <= segments; seg++)
                        {
                            var t = startAngle + sweep * seg / segments;
                            var px = curve.Center.X + curve.Radius * Math.Cos(t);
                            var py = curve.Center.Y + curve.Radius * Math.Sin(t);
                            var curr = WorldToScreen(px, py);
                            Raylib.DrawLineV(
                                new System.Numerics.Vector2(prev.X, prev.Y),
                                new System.Numerics.Vector2(curr.X, curr.Y),
                                color);
                            prev = curr;
                        }
                        break;
                    }
                }
            }

            // Station labels at element start points
            if (alignment.Elements.Count > 0)
            {
                double cumulativeStation = alignment.StartStation;
                for (var j = 0; j < alignment.Elements.Count; j++)
                {
                    var elem = alignment.Elements[j];
                    var s = WorldToScreen(elem.Start.X, elem.Start.Y);
                    Raylib.DrawText($"Sta {cumulativeStation:F0}",
                        (int)s.X + 4, (int)s.Y - 14, 10, color);
                    cumulativeStation += elem.Length;
                }

                // Label at end of last element
                var lastElem = alignment.Elements[^1];
                var end = WorldToScreen(lastElem.End.X, lastElem.End.Y);
                Raylib.DrawText($"Sta {cumulativeStation:F0}",
                    (int)end.X + 4, (int)end.Y - 14, 10, color);
            }

            // Draw alignment name near first point
            if (alignment.Elements.Count > 0)
            {
                var first = WorldToScreen(alignment.Elements[0].Start.X, alignment.Elements[0].Start.Y);
                Raylib.DrawText(alignment.Name, (int)first.X + 4, (int)first.Y + 4, 12, color);
            }
        }
    }

    public override void Refit()
    {
        if (Document == null || Document.Alignments.Count == 0) return;

        double minX = double.MaxValue, minY = double.MaxValue;
        double maxX = double.MinValue, maxY = double.MinValue;

        foreach (var alignment in Document.Alignments)
        {
            foreach (var element in alignment.Elements)
            {
                Expand(element.Start.X, element.Start.Y, ref minX, ref minY, ref maxX, ref maxY);
                Expand(element.End.X, element.End.Y, ref minX, ref minY, ref maxX, ref maxY);

                if (element is CoordGeomCurve curve)
                    Expand(curve.Center.X, curve.Center.Y, ref minX, ref minY, ref maxX, ref maxY);
            }
        }

        if (minX < double.MaxValue)
            RefitToBounds(minX, minY, maxX, maxY);
    }

    static void Expand(double x, double y, ref double minX, ref double minY, ref double maxX, ref double maxY)
    {
        if (x < minX) minX = x;
        if (y < minY) minY = y;
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
    }
}
