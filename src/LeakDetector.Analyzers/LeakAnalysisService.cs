using LeakDetector.Core.Model;
using LeakDetector.Core.Services;
using LeakDetector.Parsers;

namespace LeakDetector.Analyzers;

public sealed class LeakAnalysisService
{
    private readonly SimpleLeakAnalyzer _analyzer = new();

    public AnalysisReport AnalyzeLogFile(string path)
    {
        var jsonEvents = JsonlEventParser.ParseFile(path);
        return _analyzer.Analyze(jsonEvents);
    }
}