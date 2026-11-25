using LeakDetector.Core.Model;
using LeakDetector.Reporters;

namespace LeakDetector.Tests;

public class ConsoleReporterTests
{
    [Fact]
    public void Format_NoLeaks_PrintsNoLeaksLine()
    {
        var report = new AnalysisReport(Array.Empty<LeakEntry>(), 0);
        var text = new ConsoleReporter().Format(report);

        Assert.Contains("No leaks detected", text);
        Assert.DoesNotContain("LEAK:", text);
    }

    [Fact]
    public void Format_OneLeak_InterpolatesFields()
    {
        var report = new AnalysisReport
        (
            new[]
            {
                new LeakEntry("0x1", 4, "main.cpp", 7, false)
            },
            0
        );

        var text = new ConsoleReporter().Format(report);

        Assert.Contains("4", text);
        Assert.Contains("0x1", text);
        Assert.Contains("main.cpp", text);
        Assert.Contains("7", text);
        Assert.DoesNotContain("{leak.", text);
    }
}