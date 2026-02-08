namespace LWS.VerticalProfileOptimisation;

public class AccessTrackOptModel
{
    public required HorizontalAlignment Alignment;
    public required XSectionParams XSectionParams;
    public required OptCostModel CostModel;

    public double StationSpacing = 20;
    public int NumSamplesVerticalOffset = 20;
    
    public double MinOffset = -5.0;
    public double MaxOffset = 5.0;
    public double GradientMin = -0.1;
    public double GradientMax = 0.1;
    public double KMinCrest = 5.0;
    public double KMinSag = 5.0;
    public double BorrowCapacity = 0.0;
    public double WasteCapacity = 0.0;
}
