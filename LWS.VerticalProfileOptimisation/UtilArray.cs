namespace LWS.VerticalProfileOptimisation;

public static class UtilArray
{
    public static void Scatter(double[] array, int[] indices, double value)
    {
        foreach (var i in indices)
            array[i] = value;
    }

    public static void Scatter(double[] array, int[] indices, double[] values)
    {
        for (var i = 0; i < indices.Length; i++)
            array[indices[i]] = values[i];
    }
}
