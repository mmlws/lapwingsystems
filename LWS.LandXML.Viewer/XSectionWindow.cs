using ImGuiNET;
using Raylib_cs;

namespace LWS.LandXML.Viewer;

/// <summary>
/// Draws cross-section surfaces as polylines (Offset vs Elevation), with Y flipped.
/// </summary>
public class XSectionWindow : ViewportWindow
{
    static readonly Color[] SurfaceColors =
    [
        Color.SkyBlue, Color.Lime, Color.Orange, Color.Magenta,
        Color.Gold, Color.Pink, Color.Violet, Color.Beige
    ];

    public LandXmlDocument? Document { get; set; }
    public int SelectedAlignmentIndex { get; set; }
    public int SelectedXSectionIndex { get; set; }

    public XSectionWindow() : base("Cross-Section View")
    {
    }

    protected override void DrawContent()
    {
        if (Document == null || Document.Alignments.Count == 0) return;

        var alignment = Document.Alignments[SelectedAlignmentIndex];
        if (alignment.CrossSections.Count == 0) return;

        SelectedXSectionIndex = Math.Clamp(SelectedXSectionIndex, 0, alignment.CrossSections.Count - 1);
        var cs = alignment.CrossSections[SelectedXSectionIndex];

        // Draw grid
        DrawGridLines();

        // Draw surfaces
        for (var si = 0; si < cs.Surfaces.Count; si++)
        {
            var surf = cs.Surfaces[si];
            var color = SurfaceColors[si % SurfaceColors.Length];

            for (var i = 0; i < surf.Points.Count - 1; i++)
            {
                var s = WorldToScreen(surf.Points[i].Offset, surf.Points[i].Elevation);
                var e = WorldToScreen(surf.Points[i + 1].Offset, surf.Points[i + 1].Elevation);
                Raylib.DrawLineV(
                    new System.Numerics.Vector2(s.X, s.Y),
                    new System.Numerics.Vector2(e.X, e.Y),
                    color);
            }

            // Draw points
            foreach (var pt in surf.Points)
            {
                var p = WorldToScreen(pt.Offset, pt.Elevation);
                Raylib.DrawCircleV(new System.Numerics.Vector2(p.X, p.Y), 3f, color);
            }

            // Surface name label near first point
            if (surf.Points.Count > 0)
            {
                var first = WorldToScreen(surf.Points[0].Offset, surf.Points[0].Elevation);
                Raylib.DrawText(surf.Name, (int)first.X + 4, (int)first.Y - 14, 10, color);
            }
        }

        // Draw centre-line reference
        var clTop = WorldToScreen(0, 1e6);
        var clBot = WorldToScreen(0, -1e6);
        Raylib.DrawLineV(
            new System.Numerics.Vector2(clTop.X, clTop.Y),
            new System.Numerics.Vector2(clBot.X, clBot.Y),
            new Color(80, 80, 80, 255));
    }

    protected override string FormatXLabel(double value) => $"{value:F1}";

    public override void Refit()
    {
        if (Document == null || Document.Alignments.Count == 0) return;

        var alignment = Document.Alignments[SelectedAlignmentIndex];
        if (alignment.CrossSections.Count == 0) return;

        SelectedXSectionIndex = Math.Clamp(SelectedXSectionIndex, 0, alignment.CrossSections.Count - 1);
        var cs = alignment.CrossSections[SelectedXSectionIndex];

        double minOff = 0, maxOff = 0;
        double minElev = double.MaxValue, maxElev = double.MinValue;
        foreach (var surf in cs.Surfaces)
        {
            foreach (var pt in surf.Points)
            {
                if (pt.Offset < minOff) minOff = pt.Offset;
                if (pt.Offset > maxOff) maxOff = pt.Offset;
                if (pt.Elevation < minElev) minElev = pt.Elevation;
                if (pt.Elevation > maxElev) maxElev = pt.Elevation;
            }
        }

        if (minElev < double.MaxValue)
            RefitToBounds(minOff, minElev, maxOff, maxElev);
    }
}
