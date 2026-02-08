using LWS.Geometry;

namespace LWS.VerticalProfileOptimisation;

public class HorizontalAlignment
{
    bool _dirty;
    readonly List<Pt2d> _points = [];
    double _startStation;
    readonly List<double> _pointStations = [];

    static void CheckDirty(bool dirty)
    {
        if (dirty)
            throw new InvalidOperationException("Horizontal alignment is out of date. Refresh required.");
    }

    public void AddPoint(Pt2d pt)
    {
        _points.Add(pt);
        _dirty = true;
    }

    public void Refresh()
    {
        if (!_dirty)
            return;

        var n = _points.Count;
        if (n < 2)
            throw new InvalidOperationException("Horizontal alignment must have at least two points");

        _pointStations.Clear();
        _pointStations.Add(_startStation);
        for (var i = 1; i < n; i++)
        {
            var dx = _points[i].X - _points[i - 1].X;
            var dy = _points[i].Y - _points[i - 1].Y;
            _pointStations.Add(_pointStations[i - 1] + Math.Sqrt(dx * dx + dy * dy));
        }

        _dirty = false;
    }

    public Pt2d PointAt(double station)
    {
        CheckDirty(_dirty);

        var n = _points.Count;
        station = Math.Clamp(station, _pointStations[0], _pointStations[n - 1]);

        var idx = _pointStations.BinarySearch(station);
        if (idx < 0) idx = ~idx - 1;
        idx = Math.Clamp(idx, 0, n - 2);

        var segStart = _pointStations[idx];
        var segEnd = _pointStations[idx + 1];
        var segLength = segEnd - segStart;

        var t = segLength > 0.0 ? (station - segStart) / segLength : 0.0;
        return new Pt2d(
            (1.0 - t) * _points[idx].X + t * _points[idx + 1].X,
            (1.0 - t) * _points[idx].Y + t * _points[idx + 1].Y);
    }

    public Frame2d FrameAt(double station)
    {
        CheckDirty(_dirty);

        var n = _points.Count;
        station = Math.Clamp(station, _pointStations[0], _pointStations[n - 1]);

        var idx = _pointStations.BinarySearch(station);
        if (idx < 0) idx = ~idx - 1;
        idx = Math.Clamp(idx, 0, n - 2);

        var segStart = _pointStations[idx];
        var segEnd = _pointStations[idx + 1];
        var segLength = segEnd - segStart;

        var t = segLength > 0.0 ? (station - segStart) / segLength : 0.0;
        var ox = (1.0 - t) * _points[idx].X + t * _points[idx + 1].X;
        var oy = (1.0 - t) * _points[idx].Y + t * _points[idx + 1].Y;
        var dx = _points[idx + 1].X - _points[idx].X;
        var dy = _points[idx + 1].Y - _points[idx].Y;
        var len = Math.Sqrt(dx * dx + dy * dy);

        return new Frame2d(new Pt2d(ox, oy), new Vec2d(dx / len, dy / len));
    }

    public double Length
    {
        get
        {
            CheckDirty(_dirty);
            return _pointStations[^1] - _pointStations[0];
        }
    }

    public double StartStation
    {
        get => _startStation;
        set
        {
            _startStation = value;
            _dirty = true;
        }
    }
}
