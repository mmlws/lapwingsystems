using System.Numerics;
using ImGuiNET;

namespace LWS.LandXML.Viewer;

/// <summary>
/// ImGui-based file browser popup for selecting files.
/// </summary>
public class FileBrowser
{
    string _currentDir;
    string _selectedFile = "";
    string[] _extensions;
    string[]? _dirs;
    string[]? _files;
    bool _needsRefresh = true;

    public FileBrowser(string[] extensions)
    {
        _extensions = extensions;
        _currentDir = Directory.GetCurrentDirectory();
    }

    public void Open()
    {
        ImGui.OpenPopup("Browse File");
        _needsRefresh = true;
    }

    /// <summary>
    /// Draw the popup. Returns the selected file path when confirmed, null otherwise.
    /// </summary>
    public string? Draw()
    {
        string? result = null;

        ImGui.SetNextWindowSize(new Vector2(700, 500), ImGuiCond.Appearing);
        var center = ImGui.GetMainViewport().GetCenter();
        ImGui.SetNextWindowPos(center, ImGuiCond.Appearing, new Vector2(0.5f, 0.5f));

        if (!ImGui.BeginPopupModal("Browse File"))
            return null;

        if (_needsRefresh)
        {
            Refresh();
            _needsRefresh = false;
        }

        // Navigation bar
        if (ImGui.Button("^"))
        {
            var parent = Directory.GetParent(_currentDir);
            if (parent != null)
            {
                _currentDir = parent.FullName;
                _selectedFile = "";
                _needsRefresh = true;
            }
        }

        ImGui.SameLine();

        // Drive buttons on Windows
        var drives = DriveInfo.GetDrives();
        if (drives.Length > 1)
        {
            foreach (var drive in drives)
            {
                if (!drive.IsReady) continue;
                var label = drive.Name.TrimEnd('\\');
                if (ImGui.SmallButton(label))
                {
                    _currentDir = drive.RootDirectory.FullName;
                    _selectedFile = "";
                    _needsRefresh = true;
                }

                ImGui.SameLine();
            }
        }

        ImGui.NewLine();

        // Current path (editable)
        var pathInput = _currentDir;
        ImGui.SetNextItemWidth(-1);
        if (ImGui.InputText("##path", ref pathInput, 2048, ImGuiInputTextFlags.EnterReturnsTrue))
        {
            if (Directory.Exists(pathInput))
            {
                _currentDir = pathInput;
                _selectedFile = "";
                _needsRefresh = true;
            }
            else if (File.Exists(pathInput))
            {
                result = pathInput;
                ImGui.CloseCurrentPopup();
                ImGui.EndPopup();
                return result;
            }
        }

        ImGui.Separator();

        // File list
        var listSize = new Vector2(-1, ImGui.GetContentRegionAvail().Y - 35);
        if (ImGui.BeginChild("filelist", listSize, ImGuiChildFlags.Borders))
        {
            if (_dirs != null)
            {
                foreach (var dir in _dirs)
                {
                    var name = Path.GetFileName(dir);
                    if (string.IsNullOrEmpty(name)) name = dir;

                    if (ImGui.Selectable("[" + name + "]", false, ImGuiSelectableFlags.AllowDoubleClick))
                    {
                        if (ImGui.IsMouseDoubleClicked(ImGuiMouseButton.Left))
                        {
                            _currentDir = dir;
                            _selectedFile = "";
                            _needsRefresh = true;
                        }
                    }
                }
            }

            if (_files != null)
            {
                foreach (var file in _files)
                {
                    var name = Path.GetFileName(file);
                    var isSelected = _selectedFile == file;

                    if (ImGui.Selectable(name, isSelected, ImGuiSelectableFlags.AllowDoubleClick))
                    {
                        _selectedFile = file;

                        if (ImGui.IsMouseDoubleClicked(ImGuiMouseButton.Left))
                        {
                            result = _selectedFile;
                            ImGui.CloseCurrentPopup();
                        }
                    }
                }
            }
        }

        ImGui.EndChild();

        // Bottom bar
        var selDisplay = string.IsNullOrEmpty(_selectedFile) ? "" : Path.GetFileName(_selectedFile);
        ImGui.SetNextItemWidth(ImGui.GetContentRegionAvail().X - 160);
        ImGui.InputText("##selected", ref selDisplay, 512, ImGuiInputTextFlags.ReadOnly);

        ImGui.SameLine();
        var canOpen = !string.IsNullOrEmpty(_selectedFile) && File.Exists(_selectedFile);
        if (!canOpen) ImGui.BeginDisabled();
        if (ImGui.Button("Open", new Vector2(70, 0)))
        {
            result = _selectedFile;
            ImGui.CloseCurrentPopup();
        }

        if (!canOpen) ImGui.EndDisabled();

        ImGui.SameLine();
        if (ImGui.Button("Cancel", new Vector2(70, 0)))
            ImGui.CloseCurrentPopup();

        ImGui.EndPopup();
        return result;
    }

    void Refresh()
    {
        try
        {
            _dirs = Directory.GetDirectories(_currentDir)
                .OrderBy(d => Path.GetFileName(d), StringComparer.OrdinalIgnoreCase)
                .ToArray();
        }
        catch
        {
            _dirs = [];
        }

        try
        {
            _files = Directory.GetFiles(_currentDir)
                .Where(f => _extensions.Length == 0 ||
                            _extensions.Any(ext =>
                                f.EndsWith(ext, StringComparison.OrdinalIgnoreCase)))
                .OrderBy(f => Path.GetFileName(f), StringComparer.OrdinalIgnoreCase)
                .ToArray();
        }
        catch
        {
            _files = [];
        }
    }
}
