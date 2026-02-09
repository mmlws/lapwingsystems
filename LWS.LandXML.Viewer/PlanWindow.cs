using Raylib_cs;

namespace LWS.LandXML.Viewer;

/// <summary>
/// Draws horizontal alignment lines in plan view (X,Y world coordinates).
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

            foreach (var line in alignment.Lines)
            {
                var s = WorldToScreen(line.Start.X, line.Start.Y);
                var e = WorldToScreen(line.End.X, line.End.Y);
                Raylib.DrawLineV(
                    new System.Numerics.Vector2(s.X, s.Y),
                    new System.Numerics.Vector2(e.X, e.Y),
                    color);
            }

            // Station labels at line start points
            if (alignment.Lines.Count > 0)
            {
                double cumulativeStation = alignment.StartStation;
                for (var j = 0; j < alignment.Lines.Count; j++)
                {
                    var line = alignment.Lines[j];
                    var s = WorldToScreen(line.Start.X, line.Start.Y);
                    Raylib.DrawText($"Sta {cumulativeStation:F0}",
                        (int)s.X + 4, (int)s.Y - 14, 10, color);

                    var dx = line.End.X - line.Start.X;
                    var dy = line.End.Y - line.Start.Y;
                    cumulativeStation += Math.Sqrt(dx * dx + dy * dy);
                }

                // Label at end of last line
                var lastLine = alignment.Lines[^1];
                var end = WorldToScreen(lastLine.End.X, lastLine.End.Y);
                Raylib.DrawText($"Sta {cumulativeStation:F0}",
                    (int)end.X + 4, (int)end.Y - 14, 10, color);
            }

            // Draw alignment name near first point
            if (alignment.Lines.Count > 0)
            {
                var first = WorldToScreen(alignment.Lines[0].Start.X, alignment.Lines[0].Start.Y);
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
            foreach (var line in alignment.Lines)
            {
                Expand(line.Start.X, line.Start.Y, ref minX, ref minY, ref maxX, ref maxY);
                Expand(line.End.X, line.End.Y, ref minX, ref minY, ref maxX, ref maxY);
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
