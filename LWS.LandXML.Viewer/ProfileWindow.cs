using System.Numerics;
using ImGuiNET;
using Raylib_cs;

namespace LWS.LandXML.Viewer;

/// <summary>
/// Draws vertical profile as Station (X) vs Elevation (Y), with Y flipped so up = higher elevation.
/// Supports both PVI-based and element-based profiles.
/// </summary>
public class ProfileWindow : ViewportWindow
{
    static readonly Color TangentColor = Color.SkyBlue;
    static readonly Color ParabolicColor = Color.Lime;
    static readonly Color CircularColor = Color.Orange;

    const int CurveSampleCount = 64;

    public LandXmlDocument? Document { get; set; }
    public int SelectedAlignmentIndex { get; set; }

    public ProfileWindow() : base("Profile View")
    {
    }

    protected override void DrawContent()
    {
        if (Document == null || Document.Alignments.Count == 0) return;

        var alignment = Document.Alignments[SelectedAlignmentIndex];
        if (alignment.Profile == null) return;

        var profile = alignment.Profile;

        if (profile.HasElements)
            DrawElementProfile(profile);
        else if (profile.PVIs.Count > 0)
            DrawPviProfile(profile);
    }

    void DrawPviProfile(Profile profile)
    {
        var pvis = profile.PVIs;

        DrawGridLines();

        // Draw PVI lines
        for (var i = 0; i < pvis.Count - 1; i++)
        {
            var s = WorldToScreen(pvis[i].Station, pvis[i].Elevation);
            var e = WorldToScreen(pvis[i + 1].Station, pvis[i + 1].Elevation);
            Raylib.DrawLineV(
                new System.Numerics.Vector2(s.X, s.Y),
                new System.Numerics.Vector2(e.X, e.Y),
                TangentColor);
        }

        // Draw PVI points
        foreach (var pvi in pvis)
        {
            var p = WorldToScreen(pvi.Station, pvi.Elevation);
            Raylib.DrawCircleV(new System.Numerics.Vector2(p.X, p.Y), 4f, Color.White);
            Raylib.DrawText($"{pvi.Elevation:F2}",
                (int)p.X + 6, (int)p.Y - 14, 10, Color.White);
        }
    }

    void DrawElementProfile(Profile profile)
    {
        var (minSta, maxSta) = profile.StationRange;
        double minElev = double.MaxValue, maxElev = double.MinValue;

        // Compute elevation bounds by sampling all elements
        foreach (var el in profile.Elements)
        {
            var samples = el is TangentElement ? 2 : CurveSampleCount;
            for (var i = 0; i <= samples; i++)
            {
                var sta = el.StartStation + (el.EndStation - el.StartStation) * i / samples;
                var elev = el.ElevationAt(sta);
                if (elev < minElev) minElev = elev;
                if (elev > maxElev) maxElev = elev;
            }
        }

        if (minElev > maxElev) return;

        DrawGridLines();

        // Draw each element
        for (var ei = 0; ei < profile.Elements.Count; ei++)
        {
            var element = profile.Elements[ei];
            var color = element switch
            {
                TangentElement => TangentColor,
                ParabolicElement => ParabolicColor,
                CircularElement => CircularColor,
                _ => Color.White
            };

            var samples = element is TangentElement ? 1 : CurveSampleCount;

            // Smoothstep elevation correction: closes endpoint gap while
            // preserving C1 continuity (zero derivative at t=0 and t=1).
            var targetEndElev = ei + 1 < profile.Elements.Count
                ? profile.Elements[ei + 1].StartElevation
                : element.EndElevation;
            var elevCorrection = targetEndElev - element.EndElevation;

            for (var i = 0; i < samples; i++)
            {
                var t1 = (double)i / samples;
                var t2 = (double)(i + 1) / samples;
                var sta1 = element.StartStation + (element.EndStation - element.StartStation) * t1;
                var sta2 = element.StartStation + (element.EndStation - element.StartStation) * t2;
                var blend1 = t1 * t1 * (3 - 2 * t1);
                var blend2 = t2 * t2 * (3 - 2 * t2);
                var elev1 = element.ElevationAt(sta1) + blend1 * elevCorrection;
                var elev2 = element.ElevationAt(sta2) + blend2 * elevCorrection;
                var s = WorldToScreen(sta1, elev1);
                var e = WorldToScreen(sta2, elev2);
                Raylib.DrawLineV(
                    new System.Numerics.Vector2(s.X, s.Y),
                    new System.Numerics.Vector2(e.X, e.Y),
                    color);
            }

            // Label at element start
            var startScreen = WorldToScreen(element.StartStation, element.StartElevation);
            Raylib.DrawCircleV(new System.Numerics.Vector2(startScreen.X, startScreen.Y), 3f, Color.White);

            var label = element switch
            {
                TangentElement t => $"g={t.Grade * 100:F2}%",
                ParabolicElement p => $"K={p.KValue:F1}",
                CircularElement c => $"R={c.Radius:F1}",
                _ => ""
            };
            var midSta = (element.StartStation + element.EndStation) / 2;
            var midScreen = WorldToScreen(midSta, element.ElevationAt(midSta));
            Raylib.DrawText(label, (int)midScreen.X + 4, (int)midScreen.Y - 14, 10, color);
        }

        // Draw end point of last element
        if (profile.Elements.Count > 0)
        {
            var last = profile.Elements[^1];
            var endScreen = WorldToScreen(last.EndStation, last.EndElevation);
            Raylib.DrawCircleV(new System.Numerics.Vector2(endScreen.X, endScreen.Y), 3f, Color.White);
        }
    }

    protected override void DrawMeasureGraphics(ImDrawListPtr drawList, Vector2 origin,
        double startWx, double startWy, double endWx, double endWy)
    {
        var p1 = origin + WorldToScreen(startWx, startWy);
        var p2 = origin + WorldToScreen(endWx, endWy);
        var corner = origin + WorldToScreen(endWx, startWy);

        var yellow = ImGui.GetColorU32(new Vector4(1, 1, 0, 1));
        var dim = ImGui.GetColorU32(new Vector4(1, 1, 0, 0.4f));

        // Right triangle: horizontal leg, vertical leg, slope
        drawList.AddLine(p1, corner, dim, 1f);
        drawList.AddLine(corner, p2, dim, 1f);
        drawList.AddLine(p1, p2, yellow, 1.5f);

        var dx = endWx - startWx;
        var dy = endWy - startWy;
        var grade = dx != 0 ? dy / dx * 100 : double.NaN;

        // ΔStation on horizontal leg
        var hMid = (p1 + corner) / 2f;
        drawList.AddText(new Vector2(hMid.X, hMid.Y + 4), yellow, $"{dx:F3} m");

        // ΔElevation on vertical leg
        var vMid = (corner + p2) / 2f;
        drawList.AddText(new Vector2(vMid.X + 4, vMid.Y), yellow, $"{dy:F3} m");

        // Grade on slope
        var sMid = (p1 + p2) / 2f;
        drawList.AddText(new Vector2(sMid.X + 5, sMid.Y - 15), yellow, $"{grade:F2}%");
    }

    public override void Refit()
    {
        if (Document == null || Document.Alignments.Count == 0) return;

        var alignment = Document.Alignments[SelectedAlignmentIndex];
        if (alignment.Profile == null) return;

        var profile = alignment.Profile;

        if (profile.HasElements)
        {
            var (minSta, maxSta) = profile.StationRange;
            double minElev = double.MaxValue, maxElev = double.MinValue;

            foreach (var el in profile.Elements)
            {
                var samples = el is TangentElement ? 2 : CurveSampleCount;
                for (var i = 0; i <= samples; i++)
                {
                    var sta = el.StartStation + (el.EndStation - el.StartStation) * i / samples;
                    var elev = el.ElevationAt(sta);
                    if (elev < minElev) minElev = elev;
                    if (elev > maxElev) maxElev = elev;
                }
            }

            if (minElev < double.MaxValue)
                RefitToBounds(minSta, minElev, maxSta, maxElev);
        }
        else if (profile.PVIs.Count > 0)
        {
            var pvis = profile.PVIs;
            double minSta = pvis[0].Station, maxSta = pvis[0].Station;
            double minElev = pvis[0].Elevation, maxElev = pvis[0].Elevation;
            foreach (var pvi in pvis)
            {
                if (pvi.Station < minSta) minSta = pvi.Station;
                if (pvi.Station > maxSta) maxSta = pvi.Station;
                if (pvi.Elevation < minElev) minElev = pvi.Elevation;
                if (pvi.Elevation > maxElev) maxElev = pvi.Elevation;
            }

            RefitToBounds(minSta, minElev, maxSta, maxElev);
        }
    }
}
