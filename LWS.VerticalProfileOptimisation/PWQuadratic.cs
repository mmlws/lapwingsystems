namespace LWS.VerticalProfileOptimisation;

public class PWQuadratic
{
    public List<double> Stations = [];
    public List<double> A0 = [];
    public List<double> A1 = [];
    public List<double> A2 = [];

    public PWQuadratic(double startStation, double stationSpacing, double endStation)
    {
        if (endStation <= startStation)
            throw new ArgumentException("endStation must be > startStation");
        if (stationSpacing <= 0)
            throw new ArgumentException("stationSpacing must be > 0");

        Stations.Add(startStation);
        var s = startStation + stationSpacing;
        while (s < endStation)
        {
            Stations.Add(s);
            s += stationSpacing;
        }
        Stations.Add(endStation);

        var numSeg = NumSegments;
        A0.AddRange(new double[numSeg]);
        A1.AddRange(new double[numSeg]);
        A2.AddRange(new double[numSeg]);
    }

    public int NumSegments => Stations.Count - 1;
    
    public double StartStation => Stations[0];
    
    public double EndStation => Stations[^1];

    public double[] SegmentLengths()
    {
        var n = NumSegments;
        var lengths = new double[n];
        for (var i = 0; i < n; i++)
            lengths[i] = Stations[i + 1] - Stations[i];
        return lengths;
    }

    void LocateSegment(double station, out int segIndex, out double localX)
    {
        var numSeg = NumSegments;

        var idx = Stations.BinarySearch(station);
        segIndex = idx < 0 ? ~idx - 1 : idx;

        segIndex = Math.Clamp(segIndex, 0, numSeg - 1);

        localX = station - Stations[segIndex];

        if (localX < 0.0)
            localX = 0.0;

        var segLen = Stations[segIndex + 1] - Stations[segIndex];
        if (localX > segLen)
            localX = segLen;
    }

    public double Eval(double station)
    {
        LocateSegment(station, out int seg, out double x);
        return A0[seg] + A1[seg] * x + A2[seg] * x * x;
    }

    public double EvalD(double station)
    {
        LocateSegment(station, out int seg, out double x);
        return A1[seg] + 2.0 * A2[seg] * x;
    }

    public double EvalDD(double station)
    {
        LocateSegment(station, out int seg, out double x);
        return 2.0 * A2[seg];
    }
}
