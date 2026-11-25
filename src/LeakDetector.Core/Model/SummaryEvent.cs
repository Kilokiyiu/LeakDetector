namespace LeakDetector.Core.Model;

/// <summary>
/// 总结事件，表示一个内存事件的总结，包括泄漏的个数、泄漏的字节数等信息
/// </summary>
/// <param name="Id">The id of the memory event.</param>
/// <param name="LeakCount">The count of the leaks.</param>
/// <param name="TotalLeakBytes">The total bytes of the leaks.</param>
public sealed record SummaryEvent(
    int Id,
    int LeakCount,
    ulong TotalLeakBytes) : MemoryEvent(Id);
