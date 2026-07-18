namespace LeakDetector.Core.Model;

public sealed record AnalysisReport(
    IReadOnlyList<LeakEntry> Leaks,
    int UnknownFreeCount)
{
    public int LeakCount => Leaks.Count;
    public ulong TotalLeakBytes => Leaks.Aggregate(0UL, (sum, e) => sum + e.Size);
}
