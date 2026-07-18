namespace LeakDetector.Core.Model;

public sealed record SummaryEvent(
    int Id,
    int LeakCount,
    ulong TotalLeakBytes) : MemoryEvent(Id);
