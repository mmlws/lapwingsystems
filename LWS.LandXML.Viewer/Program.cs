using ImGuiNET;
using Raylib_cs;
using rlImGui_cs;

namespace LWS.LandXML.Viewer;

public static class Program
{
    public static void Main(string[] args)
    {
        Raylib.SetConfigFlags(ConfigFlags.ResizableWindow | ConfigFlags.Msaa4xHint);
        Raylib.InitWindow(1400, 900, "LWS LandXML Viewer");
        Raylib.SetTargetFPS(60);

        rlImGui.Setup(true, true);

        var app = new ViewerApp();

        // Load file from command-line argument if provided
        if (args.Length > 0 && File.Exists(args[0]))
            app.LoadFile(args[0]);

        while (!Raylib.WindowShouldClose())
        {
            app.Update();

            Raylib.BeginDrawing();
            Raylib.ClearBackground(new Color(20, 20, 20, 255));

            rlImGui.Begin();

            // Dockspace
            var dockspaceId = ImGui.DockSpaceOverViewport(0, ImGui.GetMainViewport(), ImGuiDockNodeFlags.PassthruCentralNode);

            app.DrawMenuBar();
            app.DrawWindows();

            rlImGui.End();
            Raylib.EndDrawing();
        }

        app.Unload();
        rlImGui.Shutdown();
        Raylib.CloseWindow();
    }
}
