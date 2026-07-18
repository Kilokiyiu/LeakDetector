using LeakDetector.Core.Model;

namespace LeakDetector.Core.Services;

public sealed class SimpleLeakAnalyzer
{
    public AnalysisReport Analyze(IEnumerable<MemoryEvent> events)
    {
        var ordered = events.OrderBy(e => e.Id).ToList();
        var live = new Dictionary<string, LeakEntry>(StringComparer.Ordinal);
        var unknownFreeCount = 0;

        foreach (var ev in ordered)
        {
            switch (ev)
            {
                case AllocEvent alloc:
                    live[alloc.Ptr] = new LeakEntry(
                        alloc.Ptr,
                        alloc.Size,
                        alloc.File,
                        alloc.Line,
                        alloc.IsArray);
                    break;

                case FreeEvent free:
                    if (!live.Remove(free.Ptr))
                    {
                        unknownFreeCount++;
                    }
                    break;

                case SummaryEvent:
                    break;
            }
        }

        return new AnalysisReport(live.Values.ToList(), unknownFreeCount);
    }
}
