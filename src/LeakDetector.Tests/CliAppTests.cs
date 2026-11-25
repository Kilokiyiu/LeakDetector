using LeakDetector.Core.Model;

namespace LeakDetector.Tests;

public class CliAppTests
{
    private static AnalysisReport EmptyReport()
    {
        return new AnalysisReport(Array.Empty<LeakEntry>(), 0);
    }

    private static AnalysisReport OneLeakReport()
    {
        return new AnalysisReport(new[] { new LeakEntry("0x1", 4, "main.cpp", 7, false) }, 0);
    }

    [Fact]
    public void Run_MissingFile_Returns2()
    {
    }
}