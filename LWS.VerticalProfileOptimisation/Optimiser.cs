using LWS.Fade25D;
using static LWS.VerticalProfileOptimisation.UtilArray;

namespace LWS.VerticalProfileOptimisation;

public class OptimiserResult
{
    public required PWQuadratic VerticalProfile;
}

public class Optimiser
{
    readonly TerrainMesh _terrain;

    public List<string> Errors = [];

    public Optimiser(TerrainMesh terrain)
    {
        _terrain = terrain;
    }

    public OptimiserResult? Optimise(OptModel model)
    {
        Errors.Clear();

        if (!Validate(model))
            return null;

        // TODO: add support for more access tracks
        var track = model.AccessTracks[0];
        var ha = track.Alignment;
        var cost = track.CostModel;
        var xs = track.XSectionParams;

        // Build the vertical profile station layout
        var vp = new PWQuadratic(ha.StartStation, 20.0, ha.StartStation + ha.Length);
        var numSegments = vp.NumSegments;
        var numStations = vp.Stations.Count;
        var segmentLengths = vp.SegmentLengths();

        var inf = double.PositiveInfinity;

        // Constraint buffers (reassigned via collection expressions)
        int[] indices;
        double[] coeffs;

        #region Variable Indices
        var numVars = 0;

        // Segment quadratic coefficients
        var a0 = CreateVariables(numSegments);
        var a1 = CreateVariables(numSegments);
        var a2 = CreateVariables(numSegments);

        // Volume variables at each station
        var vCut = CreateVariables(numStations);
        var vFill = CreateVariables(numStations);

        // Ground -> vertical profile offset variables
        var u = CreateVariables(numStations);

        // Slack variables for hockey-stick constraint
        var uFill = CreateVariables(numStations);
        var uCut = CreateVariables(numStations);

        // Transient flow
        var ftP = CreateVariables(numStations);
        var ftN = CreateVariables(numStations);

        // Loading flows (load from section)
        var flP = CreateVariables(numStations);
        var flN = CreateVariables(numStations);

        // Unloading flows (unload into section)
        var fuP = CreateVariables(numStations);
        var fuN = CreateVariables(numStations);

        // Borrow flows
        var fbP = CreateVariables(numStations);
        var fbN = CreateVariables(numStations);

        // Waste flows
        var fwP = CreateVariables(numStations);
        var fwN = CreateVariables(numStations);
        #endregion

        alglib.minqpcreate(numVars, out var optState);
        alglib.minqpsetalgosparsegenipm(optState, 1e-10);

        #region Objective Function
        var costHaulSegments = new double[segmentLengths.Length];
        for (var i = 0; i < segmentLengths.Length; i++)
            costHaulSegments[i] = segmentLengths[i] * cost.CostHaul;

        var costVec = new double[numVars];
        Scatter(costVec, vCut, cost.CostCut + cost.CostLoad);
        Scatter(costVec, vFill, cost.CostFill);
        Scatter(costVec, ftP, costHaulSegments);
        Scatter(costVec, ftN, costHaulSegments);
        Scatter(costVec, fwP, cost.CostWaste);
        Scatter(costVec, fwN, cost.CostWaste);
        Scatter(costVec, fbP, cost.CostBorrow);
        Scatter(costVec, fbN, cost.CostBorrow);
        alglib.minqpsetlinearterm(optState, costVec);
        #endregion

        #region Box Constraints (Station Variables)
        for (var i = 0; i < numStations; i++)
        {
            // Waste/borrow flows set to zero for now
            alglib.minqpsetbci(optState, fbN[i], 0, 0);
            alglib.minqpsetbci(optState, fbP[i], 0, 0);
            alglib.minqpsetbci(optState, fwN[i], 0, 0);
            alglib.minqpsetbci(optState, fwP[i], 0, 0);
            // Load and unload flows (>= 0)
            alglib.minqpsetbci(optState, fuP[i], 0, inf);
            alglib.minqpsetbci(optState, fuN[i], 0, inf);
            alglib.minqpsetbci(optState, flP[i], 0, inf);
            alglib.minqpsetbci(optState, flN[i], 0, inf);
            // Volumes (>= 0)
            alglib.minqpsetbci(optState, vCut[i], 0, inf);
            alglib.minqpsetbci(optState, vFill[i], 0, inf);
            // Offsets
            alglib.minqpsetbci(optState, u[i], track.MinOffset, track.MaxOffset);
        }
        #endregion

        #region Box Constraints (Segment Variables)
        for (var i = 0; i < numSegments; i++)
        {
            // Flow variables (>= 0)
            alglib.minqpsetbci(optState, ftN[i], 0, inf);
            alglib.minqpsetbci(optState, ftP[i], 0, inf);
        }
        #endregion

        #region C0/C1 Continuity Constraints
        for (var i = 1; i < numStations - 1; i++)
        {
            var ds = vp.Stations[i] - vp.Stations[i - 1];
            // C0: a0[i-1] + ds*a1[i-1] + ds^2*a2[i-1] - a0[i] = 0
            indices = [a0[i - 1], a1[i - 1], a2[i - 1], a0[i]];
            coeffs = [1, ds, ds * ds, -1];
            alglib.minqpaddlc2(optState, indices, coeffs, 4, 0.0, 0.0);

            // C1: a1[i-1] + 2*ds*a2[i-1] - a1[i] = 0
            indices = [a1[i - 1], a2[i - 1], a1[i]];
            coeffs = [1, 2 * ds, -1];
            alglib.minqpaddlc2(optState, indices, coeffs, 3, 0.0, 0.0);
        }
        #endregion

        #region Gradient Constraints
        for (var i = 0; i < numStations; i++)
        {
            var s = vp.Stations[i];
            var seg = Math.Min(i, numSegments - 1);
            var ds = s - vp.Stations[seg];
            indices = [a1[seg], a2[seg]];
            coeffs = [1, 2 * ds];
            alglib.minqpaddlc2(optState, indices, coeffs, 2, track.GradientMin, track.GradientMax);
        }
        #endregion

        #region Vertical Curvature Constraints
        var lb = -1.0 / (200 * track.KMinCrest);
        var ub = 1.0 / (200 * track.KMinSag);
        for (var i = 0; i < numSegments; i++)
            alglib.minqpsetbci(optState, a2[i], lb, ub);
        #endregion

        #region Gap Equality Constraints
        // P(s_i) - Z_i = u_i
        // a0[seg] + ds*a1[seg] + ds^2*a2[seg] - u[i] = terrainHeight
        for (var i = 0; i < numStations; i++)
        {
            var s = vp.Stations[i];
            var haPt = ha.PointAt(s);
            var terrainHeight = _terrain.GetHeight(haPt);
            var seg = Math.Min(i, numSegments - 1);
            var ds = s - vp.Stations[seg];
            indices = [a0[seg], a1[seg], a2[seg], u[i]];
            coeffs = [1, ds, ds * ds, -1];
            alglib.minqpaddlc2(optState, indices, coeffs, 4, terrainHeight, terrainHeight);
        }
        #endregion

        #region Flow Constraints
        for (var i = 0; i < numStations; i++)
        {
            if (i == 0)
            {
                // Positive path: fl_p + fb_p - ft_p - fu_p - fw_p = 0
                indices = [flP[i], fbP[i], ftP[i], fuP[i], fwP[i]];
                coeffs = [1, 1, -1, -1, -1];
                alglib.minqpaddlc2(optState, indices, coeffs, 5, 0, 0);

                // Negative path: ft_n + fl_n + fb_n - fu_n - fw_n = 0
                indices = [ftN[i], flN[i], fbN[i], fuN[i], fwN[i]];
                coeffs = [1, 1, 1, -1, -1];
                alglib.minqpaddlc2(optState, indices, coeffs, 5, 0, 0);
            }
            else if (i == numStations - 1)
            {
                // Positive path: ft_p[i-1] + fl_p + fb_p - fu_p - fw_p = 0
                indices = [ftP[i - 1], flP[i], fbP[i], fuP[i], fwP[i]];
                coeffs = [1, 1, 1, -1, -1];
                alglib.minqpaddlc2(optState, indices, coeffs, 5, 0, 0);

                // Negative path: fl_n + fb_n - fu_n - fw_n - ft_n[i-1] = 0
                indices = [flN[i], fbN[i], fuN[i], fwN[i], ftN[i - 1]];
                coeffs = [1, 1, -1, -1, -1];
                alglib.minqpaddlc2(optState, indices, coeffs, 5, 0, 0);
            }
            else
            {
                // Positive path: ft_p[i-1] + fl_p + fb_p - fu_p - fw_p - ft_p[i] = 0
                indices = [ftP[i - 1], flP[i], fbP[i], fuP[i], fwP[i], ftP[i]];
                coeffs = [1, 1, 1, -1, -1, -1];
                alglib.minqpaddlc2(optState, indices, coeffs, 6, 0, 0);

                // Negative path: ft_n[i] + fl_n + fb_n - fu_n - fw_n - ft_n[i-1] = 0
                indices = [ftN[i], flN[i], fbN[i], fuN[i], fwN[i], ftN[i - 1]];
                coeffs = [1, 1, 1, -1, -1, -1];
                alglib.minqpaddlc2(optState, indices, coeffs, 6, 0, 0);
            }
        }
        #endregion

        #region Section Balance Constraints
        for (var i = 0; i < numStations; i++)
        {
            // fu_p + fu_n - v_fill = 0
            indices = [fuP[i], fuN[i], vFill[i]];
            coeffs = [1, 1, -1];
            alglib.minqpaddlc2(optState, indices, coeffs, 3, 0, 0);

            // fl_p + fl_n - v_cut = 0
            indices = [flP[i], flN[i], vCut[i]];
            coeffs = [1, 1, -1];
            alglib.minqpaddlc2(optState, indices, coeffs, 3, 0, 0);
        }
        #endregion

        #region Cut/Fill Volume Constraints
        var xsCalc = new XSectionCalculator(_terrain);
        for (var i = 0; i < numStations; i++)
        {
            var s = vp.Stations[i];
            var areas = xsCalc.ComputeAreasAtOffsets(
                ha, s, xs,
                track.MinOffset, track.MaxOffset,
                model.NumSamplesVerticalOffsets);

            var stepSize = (track.MaxOffset - track.MinOffset) / (model.NumSamplesVerticalOffsets - 1);
            for (var j = 0; j < model.NumSamplesVerticalOffsets; j++)
            {
                var offset = track.MinOffset + stepSize * j;
                // v_cut >= areas[j].Cut when u[i] == offset
                // Linearised: v_cut >= Cut_j  (when u == offset_j)
                // This is a hockey-stick linearisation:
                //   v_cut - dCut/du * u >= Cut_j - dCut/du * offset_j
                // For adjacent samples, approximate the slope
            }

            // Piecewise-linear outer approximation of cut/fill as functions of offset
            for (var j = 0; j < model.NumSamplesVerticalOffsets - 1; j++)
            {
                var offset1 = track.MinOffset + stepSize * j;
                var offset2 = track.MinOffset + stepSize * (j + 1);

                // Cut constraint: v_cut >= linear interpolation between samples
                var cutSlope = (areas[j + 1].Cut - areas[j].Cut) / (offset2 - offset1);
                var cutIntercept = areas[j].Cut - cutSlope * offset1;
                // v_cut >= cutSlope * u + cutIntercept
                // v_cut - cutSlope * u >= cutIntercept
                indices = [vCut[i], u[i]];
                coeffs = [1, -cutSlope];
                alglib.minqpaddlc2(optState, indices, coeffs, 2, cutIntercept, inf);

                // Fill constraint: v_fill >= fillSlope * u + fillIntercept
                var fillSlope = (areas[j + 1].Fill - areas[j].Fill) / (offset2 - offset1);
                var fillIntercept = areas[j].Fill - fillSlope * offset1;
                indices = [vFill[i], u[i]];
                coeffs = [1, -fillSlope];
                alglib.minqpaddlc2(optState, indices, coeffs, 2, fillIntercept, inf);
            }
        }
        #endregion

        // Solve
        alglib.minqpoptimize(optState);
        alglib.minqpresults(optState, out var x, out var rep);

        if (rep.terminationtype < 0)
        {
            Errors.Add($"QP solver failed with termination type {rep.terminationtype}");
            return null;
        }

        // Extract solution into PWQuadratic
        for (var i = 0; i < numSegments; i++)
        {
            vp.A0[i] = x[a0[i]];
            vp.A1[i] = x[a1[i]];
            vp.A2[i] = x[a2[i]];
        }

        return new OptimiserResult { VerticalProfile = vp };

        int[] CreateVariables(int n)
        {
            var firstIndex = numVars;
            var vars = new int[n];
            for (var i = 0; i < n; i++)
            {
                vars[i] = firstIndex + i;
                numVars++;
            }
            return vars;
        }
    }

    bool Validate(OptModel model)
    {
        if (model.AccessTracks.Count == 0)
            Errors.Add("No access tracks defined.");

        foreach (var track in model.AccessTracks)
        {
            var cost = track.CostModel;
            if (cost.CostCut < 0)
                Errors.Add("Excavation cost must be non-negative.");
            if (cost.CostFill < 0)
                Errors.Add("Embankment cost must be non-negative.");
            if (cost.CostLoad < 0)
                Errors.Add("Loading cost must be non-negative.");
            if (cost.CostHaul < 0)
                Errors.Add("Hauling cost must be non-negative.");
            if (cost.CostBorrow < 0)
                Errors.Add("Borrow pit material cost must be non-negative.");
            if (cost.CostWaste < 0)
                Errors.Add("Waste pit disposal cost must be non-negative.");
            if (track.MinOffset > track.MaxOffset)
                Errors.Add("Minimum vertical offset must be less than maximum offset.");
            if (track.GradientMin > track.GradientMax)
                Errors.Add("Minimum gradient must be less than maximum gradient.");
            if (track.KMinCrest <= 0)
                Errors.Add("Minimum crest curvature must be positive.");
            if (track.KMinSag <= 0)
                Errors.Add("Minimum sag curvature must be positive.");
            if (track.BorrowCapacity < 0)
                Errors.Add("Borrow capacity must be non-negative.");
            if (track.WasteCapacity < 0)
                Errors.Add("Waste capacity must be non-negative.");
        }

        return Errors.Count == 0;
    }
}
