using ImGuiNET;
using Raylib_cs;

namespace LWS.LandXML.Viewer;

/// <summary>
/// Application state: holds the document, windows, file watching, selection, and menus.
/// </summary>
public class ViewerApp
{
    public LandXmlDocument? Document { get; private set; }
    public string? LoadedFilePath { get; private set; }

    public PlanWindow PlanWindow { get; } = new();
    public ProfileWindow ProfileWindow { get; } = new();
    public XSectionWindow XSectionWindow { get; } = new();

    int _selectedAlignmentIndex;
    bool _watchEnabled;
    FileSystemWatcher? _watcher;
    bool _reloadPending;
    DateTime _reloadRequestTime;
    static readonly TimeSpan ReloadDebounce = TimeSpan.FromMilliseconds(500);

    // File browser
    readonly FileBrowser _fileBrowser = new([".xml", ".landxml"]);
    bool _showFileBrowser;

    // Profile vertical exaggeration
    float _profileScale = 1f;

    public void LoadFile(string filePath)
    {
        try
        {
            Document = LandXmlDocument.Load(filePath);
            LoadedFilePath = filePath;
            _selectedAlignmentIndex = 0;
            UpdateWindows();
            RefitAll();
            SetupWatcher();
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Failed to load {filePath}: {ex.Message}");
        }
    }

    void UpdateWindows()
    {
        PlanWindow.Document = Document;
        ProfileWindow.Document = Document;
        ProfileWindow.SelectedAlignmentIndex = _selectedAlignmentIndex;
        XSectionWindow.Document = Document;
        XSectionWindow.SelectedAlignmentIndex = _selectedAlignmentIndex;
        XSectionWindow.SelectedXSectionIndex = 0;
    }

    void RefitAll()
    {
        PlanWindow.Refit();
        ProfileWindow.Refit();
        XSectionWindow.Refit();
    }

    void SetupWatcher()
    {
        DisposeWatcher();
        if (!_watchEnabled || LoadedFilePath == null) return;

        var dir = Path.GetDirectoryName(Path.GetFullPath(LoadedFilePath));
        var file = Path.GetFileName(LoadedFilePath);
        if (dir == null) return;

        _watcher = new FileSystemWatcher(dir, file)
        {
            NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.Size,
            EnableRaisingEvents = true
        };
        _watcher.Changed += (_, _) =>
        {
            _reloadPending = true;
            _reloadRequestTime = DateTime.UtcNow;
        };
    }

    void DisposeWatcher()
    {
        _watcher?.Dispose();
        _watcher = null;
    }

    public void Update()
    {
        // Handle debounced reload
        if (_reloadPending && DateTime.UtcNow - _reloadRequestTime > ReloadDebounce)
        {
            _reloadPending = false;
            if (LoadedFilePath != null)
                LoadFile(LoadedFilePath);
        }

    }

    public void DrawMenuBar()
    {
        if (ImGui.BeginMainMenuBar())
        {
            if (ImGui.BeginMenu("File"))
            {
                if (ImGui.MenuItem("Load..."))
                    _showFileBrowser = true;

                var watch = _watchEnabled;
                if (ImGui.MenuItem("Watch", null, ref watch))
                {
                    _watchEnabled = watch;
                    SetupWatcher();
                }

                ImGui.Separator();
                if (ImGui.MenuItem("Exit"))
                    Environment.Exit(0);

                ImGui.EndMenu();
            }

            if (ImGui.BeginMenu("View"))
            {
                if (ImGui.MenuItem("Refit All"))
                    RefitAll();

                ImGui.Separator();
                ImGui.SetNextItemWidth(120);
                if (ImGui.DragFloat("Profile Scale", ref _profileScale, 0.1f, 0.1f, 100f, "%.1fx"))
                    ProfileWindow.YScale = _profileScale;

                ImGui.EndMenu();
            }

            if (ImGui.BeginMenu("Windows"))
            {
                var planOpen = PlanWindow.IsOpen;
                if (ImGui.MenuItem("Plan", null, ref planOpen))
                    PlanWindow.IsOpen = planOpen;

                var profileOpen = ProfileWindow.IsOpen;
                if (ImGui.MenuItem("Profile", null, ref profileOpen))
                    ProfileWindow.IsOpen = profileOpen;

                var xsOpen = XSectionWindow.IsOpen;
                if (ImGui.MenuItem("Cross-Section", null, ref xsOpen))
                    XSectionWindow.IsOpen = xsOpen;

                ImGui.EndMenu();
            }

            // Alignment selector
            if (Document != null && Document.Alignments.Count > 0)
            {
                ImGui.Separator();
                ImGui.SetNextItemWidth(200);
                var names = Document.Alignments.Select(a => a.Name).ToArray();
                var sel = _selectedAlignmentIndex;
                if (ImGui.Combo("Alignment", ref sel, names, names.Length))
                {
                    _selectedAlignmentIndex = sel;
                    ProfileWindow.SelectedAlignmentIndex = sel;
                    XSectionWindow.SelectedAlignmentIndex = sel;
                    XSectionWindow.SelectedXSectionIndex = 0;
                    ProfileWindow.Refit();
                    XSectionWindow.Refit();
                }

                // Cross-section selector
                var alignment = Document.Alignments[_selectedAlignmentIndex];
                if (alignment.CrossSections.Count > 0)
                {
                    ImGui.SetNextItemWidth(150);
                    var stations = alignment.CrossSections.Select(cs => $"Sta {cs.Station:F1}").ToArray();
                    var xsSel = XSectionWindow.SelectedXSectionIndex;
                    if (ImGui.Combo("XSection", ref xsSel, stations, stations.Length))
                    {
                        XSectionWindow.SelectedXSectionIndex = xsSel;
                        XSectionWindow.Refit();
                    }
                }
            }

            ImGui.EndMainMenuBar();
        }

        // File browser
        if (_showFileBrowser)
        {
            _fileBrowser.Open();
            _showFileBrowser = false;
        }

        var selectedPath = _fileBrowser.Draw();
        if (selectedPath != null)
            LoadFile(selectedPath);

        // Ctrl + mouse wheel adjusts profile scale when Profile window is focused
        var io = ImGui.GetIO();
        if (io.KeyCtrl && Math.Abs(io.MouseWheel) > 0.01f && ProfileWindow.IsFocused)
        {
            var factor = io.MouseWheel > 0 ? 1.15f : 1f / 1.15f;
            _profileScale = Math.Clamp(_profileScale * factor, 0.1f, 100f);
            ProfileWindow.YScale = _profileScale;
        }
    }

    public void DrawWindows()
    {
        PlanWindow.Draw();
        ProfileWindow.Draw();
        XSectionWindow.Draw();
    }

    public void Unload()
    {
        DisposeWatcher();
        PlanWindow.Unload();
        ProfileWindow.Unload();
        XSectionWindow.Unload();
    }
}
