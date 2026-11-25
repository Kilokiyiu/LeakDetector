using LeakDetector.Core.Model;
using LeakDetector.Parsers;

namespace LeakDetector.Tests;

public class JsonlEventParserTests
{
    [Fact]
    public void ParseFile_ReadsAllocAndSummary()
    {
        // BaseDirectory ≈ .../LeakDetector.Tests/bin/Debug/netX.Y/ → 五层回到仓库根
        var path = Path.Combine(
            AppContext.BaseDirectory,
            "..", "..", "..", "..", "..", "testdata", "sample_events.jsonl");

        path = Path.GetFullPath(path);
        Assert.True(File.Exists(path), $"Missing test data: {path}");

        var events = JsonlEventParser.ParseFile(path);

        Assert.Equal(2, events.Count);
        Assert.IsType<AllocEvent>(events[0]);
        Assert.IsType<SummaryEvent>(events[1]);
    }
}