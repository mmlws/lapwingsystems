using System.Numerics;
using System.Runtime.InteropServices;
using ImGuiNET;
using Raylib_cs;
using rlImGui_cs;

namespace LWS.LandXML.Viewer;

/// <summary>
/// Base class for 2D viewport windows. Each viewport renders to its own RenderTexture2D,
/// which is displayed inside an ImGui window.
/// </summary>
public abstract class ViewportWindow
{
    public string Title { get; }
    public bool IsOpen { get; set; } = true;

    protected Vector2 Offset; // camera offset in world units
    protected float Zoom = 1f;
    protected bool YFlipped = false; // true for Profile/XSection where +Y = up
    public float YScale { get; set; } = 1f; // vertical exaggeration factor

    RenderTexture2D _renderTexture;
    protected Vector2 ContentSize { get; private set; }
    public bool IsFocused { get; private set; }

    // Measure state
    bool _measureActive;
    bool _measureHasStart;
    Vector2 _measureStartWorld;

    // Grid state (shared between DrawGridLines and overlay)
    double _gridXInterval;
    double _gridYInterval;
    bool _gridActive;

    protected ViewportWindow(string title)
    {
        Title = title;
        _renderTexture = Raylib.LoadRenderTexture(16, 16);
    }

    public void Draw()
    {
        if (!IsOpen) return;

        var open = IsOpen;
        ImGui.SetNextWindowSize(new Vector2(600, 400), ImGuiCond.FirstUseEver);
        if (!ImGui.Begin(Title, ref open))
        {
            IsOpen = open;
            ImGui.End();
            return;
        }

        IsOpen = open;
        IsFocused = ImGui.IsWindowFocused();

        // Handle resize
        var size = ImGui.GetContentRegionAvail();
        if (size.X < 1) size.X = 1;
        if (size.Y < 1) size.Y = 1;

        if ((int)size.X != _renderTexture.Texture.Width || (int)size.Y != _renderTexture.Texture.Height)
        {
            Raylib.UnloadRenderTexture(_renderTexture);
            _renderTexture = Raylib.LoadRenderTexture((int)size.X, (int)size.Y);
        }

        ContentSize = size;

        // Handle input when hovered
        if (ImGui.IsWindowHovered(ImGuiHoveredFlags.None))
            HandleInput();

        // Render to texture
        Raylib.BeginTextureMode(_renderTexture);
        Raylib.ClearBackground(new Color(30, 30, 30, 255));
        DrawContent();
        Raylib.EndTextureMode();

        // Display the texture in ImGui
        var contentOrigin = ImGui.GetCursorScreenPos();
        rlImGui.ImageRenderTexture(_renderTexture);
        DrawGridLabelsOverlay(contentOrigin);
        DrawMeasureOverlay(contentOrigin);

        // Context menu
        if (ImGui.BeginPopupContextItem("viewport_ctx_" + Title))
        {
            if (ImGui.MenuItem("Measure", null, _measureActive))
            {
                _measureActive = !_measureActive;
                _measureHasStart = false;
            }

            if (ImGui.MenuItem("Refit"))
                Refit();

            ImGui.EndPopup();
        }

        ImGui.End();
    }

    protected abstract void DrawContent();
    public abstract void Refit();

    protected Vector2 WorldToScreen(double wx, double wy)
    {
        var cx = ContentSize.X / 2f;
        var cy = ContentSize.Y / 2f;
        var sx = cx + ((float)wx - Offset.X) * Zoom;
        var sy = YFlipped
            ? cy - ((float)wy - Offset.Y) * Zoom * YScale
            : cy + ((float)wy - Offset.Y) * Zoom * YScale;
        return new Vector2(sx, sy);
    }

    protected (double x, double y) ScreenToWorld(Vector2 screen)
    {
        var cx = ContentSize.X / 2f;
        var cy = ContentSize.Y / 2f;
        var wx = (screen.X - cx) / Zoom + Offset.X;
        var wy = YFlipped
            ? -((screen.Y - cy) / (Zoom * YScale)) + Offset.Y
            : (screen.Y - cy) / (Zoom * YScale) + Offset.Y;
        return (wx, wy);
    }

    /// <summary>Gets the mouse position relative to the viewport content area.</summary>
    Vector2 GetLocalMousePos()
    {
        var contentMin = ImGui.GetCursorScreenPos();
        var mousePos = ImGui.GetMousePos();
        return mousePos - contentMin;
    }

    void HandleInput()
    {
        var localMouse = GetLocalMousePos();

        // Middle-click drag to pan
        if (ImGui.IsMouseDown(ImGuiMouseButton.Middle))
        {
            var mouseDelta = ImGui.GetMouseDragDelta(ImGuiMouseButton.Middle);
            if (mouseDelta.LengthSquared() > 0)
            {
                var panDelta = YFlipped
                    ? new Vector2(mouseDelta.X / Zoom, -mouseDelta.Y / (Zoom * YScale))
                    : new Vector2(mouseDelta.X / Zoom, mouseDelta.Y / (Zoom * YScale));
                Offset -= panDelta;
                ImGui.ResetMouseDragDelta(ImGuiMouseButton.Middle);
            }
        }

        // Mouse wheel zoom centered on cursor (skip if Ctrl held — used for profile scale)
        var wheel = ImGui.GetIO().MouseWheel;
        if (Math.Abs(wheel) > 0.01f && !ImGui.GetIO().KeyCtrl)
        {
            var (worldX, worldY) = ScreenToWorld(localMouse);
            var zoomFactor = wheel > 0 ? 1.15f : 1f / 1.15f;
            Zoom *= zoomFactor;
            Zoom = Math.Clamp(Zoom, 0.0001f, 100000f);

            // Adjust offset so the world point under cursor stays put
            var newScreen = WorldToScreen(worldX, worldY);
            var screenDiff = localMouse - newScreen;
            var correction = YFlipped
                ? new Vector2(screenDiff.X / Zoom, -screenDiff.Y / (Zoom * YScale))
                : new Vector2(screenDiff.X / Zoom, screenDiff.Y / (Zoom * YScale));
            Offset -= correction;
        }

        // Measure tool clicks
        if (_measureActive && ImGui.IsMouseClicked(ImGuiMouseButton.Left))
        {
            var (wx, wy) = ScreenToWorld(localMouse);
            if (!_measureHasStart)
            {
                _measureStartWorld = new Vector2((float)wx, (float)wy);
                _measureHasStart = true;
            }
            else
            {
                _measureHasStart = false;
            }
        }

        // ESC cancels measure
        if (_measureActive && Raylib.IsKeyPressed(KeyboardKey.Escape))
        {
            _measureActive = false;
            _measureHasStart = false;
        }
    }

    void DrawMeasureOverlay(Vector2 contentOrigin)
    {
        if (!_measureActive || !_measureHasStart) return;

        var localMouse = ImGui.GetMousePos() - contentOrigin;
        var (endWx, endWy) = ScreenToWorld(localMouse);

        DrawMeasureGraphics(ImGui.GetWindowDrawList(), contentOrigin,
            _measureStartWorld.X, _measureStartWorld.Y, endWx, endWy);
    }

    protected virtual void DrawMeasureGraphics(ImDrawListPtr drawList, Vector2 origin,
        double startWx, double startWy, double endWx, double endWy)
    {
        var p1 = origin + WorldToScreen(startWx, startWy);
        var p2 = origin + WorldToScreen(endWx, endWy);
        var yellow = ImGui.GetColorU32(new Vector4(1, 1, 0, 1));

        drawList.AddLine(p1, p2, yellow, 1.5f);

        var dx = endWx - startWx;
        var dy = endWy - startWy;
        var dist = Math.Sqrt(dx * dx + dy * dy);
        var mid = (p1 + p2) / 2f;
        drawList.AddText(new Vector2(mid.X + 5, mid.Y - 15), yellow, $"{dist:F3}");
    }

    // --- Grid infrastructure ---

    const float TargetPixelGap = 100f;
    static readonly Color GridLineColor = new(45, 45, 45, 255);

    protected static double NiceNum(double value)
    {
        if (value <= 0) return 1;
        var exponent = Math.Floor(Math.Log10(value));
        var fraction = value / Math.Pow(10, exponent);
        double nice = fraction <= 1 ? 1 : fraction <= 2 ? 2 : fraction <= 5 ? 5 : 10;
        return nice * Math.Pow(10, exponent);
    }

    protected (double minX, double minY, double maxX, double maxY) GetViewportWorldBounds()
    {
        var (x0, y0) = ScreenToWorld(new Vector2(0, 0));
        var (x1, y1) = ScreenToWorld(new Vector2(ContentSize.X, ContentSize.Y));
        if (x0 > x1) (x0, x1) = (x1, x0);
        if (y0 > y1) (y0, y1) = (y1, y0);
        return (x0, y0, x1, y1);
    }

    protected void DrawGridLines()
    {
        var (minX, minY, maxX, maxY) = GetViewportWorldBounds();

        _gridXInterval = NiceNum(TargetPixelGap / Zoom);
        _gridYInterval = NiceNum(TargetPixelGap / (Zoom * YScale));
        _gridActive = true;

        // Vertical grid lines
        var xStart = Math.Floor(minX / _gridXInterval) * _gridXInterval;
        for (var x = xStart; x <= maxX; x += _gridXInterval)
        {
            var top = WorldToScreen(x, maxY);
            var bot = WorldToScreen(x, minY);
            Raylib.DrawLineV(
                new System.Numerics.Vector2(top.X, top.Y),
                new System.Numerics.Vector2(bot.X, bot.Y),
                GridLineColor);
        }

        // Horizontal grid lines
        var yStart = Math.Floor(minY / _gridYInterval) * _gridYInterval;
        for (var y = yStart; y <= maxY; y += _gridYInterval)
        {
            var left = WorldToScreen(minX, y);
            var right = WorldToScreen(maxX, y);
            Raylib.DrawLineV(
                new System.Numerics.Vector2(left.X, left.Y),
                new System.Numerics.Vector2(right.X, right.Y),
                GridLineColor);
        }
    }

    protected virtual string FormatXLabel(double value) => $"{value:F0}";
    protected virtual string FormatYLabel(double value) => $"{value:F1}";

    void DrawGridLabelsOverlay(Vector2 contentOrigin)
    {
        if (!_gridActive) return;
        _gridActive = false;

        var drawList = ImGui.GetWindowDrawList();
        var labelColor = ImGui.GetColorU32(new Vector4(0.45f, 0.45f, 0.45f, 1f));
        var (minX, minY, maxX, maxY) = GetViewportWorldBounds();

        // Y-axis labels: pinned to left edge, horizontal
        var yStart = Math.Floor(minY / _gridYInterval) * _gridYInterval;
        for (var y = yStart; y <= maxY; y += _gridYInterval)
        {
            var screenPt = WorldToScreen(0, y);
            var screenY = contentOrigin.Y + screenPt.Y;
            if (screenY < contentOrigin.Y + 2 || screenY > contentOrigin.Y + ContentSize.Y - 14)
                continue;
            drawList.AddText(
                new Vector2(contentOrigin.X + 4, screenY - 7),
                labelColor, FormatYLabel(y));
        }

        // X-axis labels: pinned to bottom edge, rotated 90° CW
        var xStart = Math.Floor(minX / _gridXInterval) * _gridXInterval;
        for (var x = xStart; x <= maxX; x += _gridXInterval)
        {
            var screenPt = WorldToScreen(x, 0);
            var screenX = contentOrigin.X + screenPt.X;
            if (screenX < contentOrigin.X + 2 || screenX > contentOrigin.X + ContentSize.X - 10)
                continue;

            var text = FormatXLabel(x);
            var textSize = ImGui.CalcTextSize(text);
            // After 90° CW rotation: original width → height, original height → width
            // Pivot is top-left of unrotated text; rotated text extends left and down
            var pivot = new Vector2(
                screenX + textSize.Y / 2f,
                contentOrigin.Y + ContentSize.Y - textSize.X - 4);
            AddTextRotated90CW(drawList, pivot, labelColor, text);
        }
    }

    /// <summary>
    /// Draws text rotated 90° clockwise by post-transforming ImDrawList vertices.
    /// </summary>
    static void AddTextRotated90CW(ImDrawListPtr drawList, Vector2 pivot, uint color, string text)
    {
        int vtxStart = drawList.VtxBuffer.Size;
        drawList.AddText(pivot, color, text);
        int vtxEnd = drawList.VtxBuffer.Size;

        const int vertSize = 20; // sizeof(ImDrawVert): Vector2 pos + Vector2 uv + uint col
        nint baseAddr = drawList.VtxBuffer.Data;

        for (int i = vtxStart; i < vtxEnd; i++)
        {
            nint addr = baseAddr + i * vertSize;
            float vx = BitConverter.Int32BitsToSingle(Marshal.ReadInt32(addr));
            float vy = BitConverter.Int32BitsToSingle(Marshal.ReadInt32(addr + 4));
            float dx = vx - pivot.X;
            float dy = vy - pivot.Y;
            // 90° CW: (dx, dy) → (-dy, dx)
            Marshal.WriteInt32(addr, BitConverter.SingleToInt32Bits(pivot.X - dy));
            Marshal.WriteInt32(addr + 4, BitConverter.SingleToInt32Bits(pivot.Y + dx));
        }
    }

    protected void RefitToBounds(double minX, double minY, double maxX, double maxY, float margin = 0.05f)
    {
        if (maxX <= minX || maxY <= minY) return;

        var rangeX = maxX - minX;
        var rangeY = maxY - minY;

        // Add margin
        minX -= rangeX * margin;
        maxX += rangeX * margin;
        minY -= rangeY * margin;
        maxY += rangeY * margin;

        rangeX = maxX - minX;
        rangeY = maxY - minY;

        Offset = new Vector2((float)(minX + rangeX / 2), (float)(minY + rangeY / 2));

        var zoomX = ContentSize.X > 0 ? ContentSize.X / (float)rangeX : 1f;
        var zoomY = ContentSize.Y > 0 ? ContentSize.Y / (float)rangeY : 1f;
        Zoom = Math.Min(zoomX, zoomY);
    }

    public void Unload()
    {
        Raylib.UnloadRenderTexture(_renderTexture);
    }
}
