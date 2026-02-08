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

    public static double[] Gather(double[] array, int[] indices)
    {
        var result = new double[indices.Length];
        for (var i = 0; i < indices.Length; i++)
            result[i] = array[indices[i]];
        return result;
    }
}
