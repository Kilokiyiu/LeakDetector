using LeakDetector.Core.Model;
using LeakDetector.Core.Services;

namespace LeakDetector.Tests;

public class SimpleLeakAnalyzerTests
{
    [Fact]
    public void SimpleLeak_OneAllocNoFree_ReportsOneLeak()
    {
        var events = new MemoryEvent[]
        {
            new AllocEvent(1, "0x1", 4, "new", "main.cpp", 7),
            new SummaryEvent(2, 1, 4)
        };

        var report = new SimpleLeakAnalyzer().Analyze(events);

        Assert.Equal(1, report.LeakCount);
        Assert.Equal(4UL, report.TotalLeakBytes);
        Assert.Equal("main.cpp", report.Leaks[0].File);
        Assert.Equal(7, report.Leaks[0].Line);
    }

    [Fact]
    public void NoLeak_AllocThenFree_ReportsZero()
    {
        var events = new MemoryEvent[]
        {
            new AllocEvent(1, "0x1", 4, "new", "main.cpp", 7),
            new FreeEvent(2, "0x1", "delete"),
            new SummaryEvent(3, 0, 0)
        };

        var report = new SimpleLeakAnalyzer().Analyze(events);
        Assert.Equal(0, report.LeakCount);
        Assert.Equal(0, report.UnknownFreeCount);
    }

    [Fact]
    public void ArrayLeak_MarkedAsArray()
    {
        var events = new MemoryEvent[]
        {
            new AllocEvent(1, "0x1", 40, "new[]", "main.cpp", 7),
            new SummaryEvent(2, 1, 40)
        };

        var report = new SimpleLeakAnalyzer().Analyze(events);

        Assert.True(report.Leaks[0].IsArray);
    }
}