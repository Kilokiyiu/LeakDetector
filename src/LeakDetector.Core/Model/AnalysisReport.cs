namespace LeakDetector.Core.Model;

/// <summary>
/// 分析报告，表示一个分析报告，包括泄漏的条目、未知的释放次数等信息
/// </summary>
/// <param name="Leaks">The leaks of the analysis report.</param>
/// <param name="UnknownFreeCount">The count of the unknown frees.</param>
public sealed record AnalysisReport(
    IReadOnlyList<LeakEntry> Leaks,
    int UnknownFreeCount)
{
    public int LeakCount => Leaks.Count;
    public ulong TotalLeakBytes => Leaks.Aggregate(0UL, (sum, e) => sum + e.Size);
}
