using System.Text;
using LeakDetector.Core.Model;

namespace LeakDetector.Reporters;

public sealed class ConsoleReporter
{
    public void Write(AnalysisReport report, TextWriter output)
    {
        if (report.LeakCount == 0)
        {
            output.WriteLine("[LeakDetector] No leaks detected");
            return;
        }

        output.WriteLine($"[LeakDetector] Found {report.LeakCount} leaks(S):");
        foreach (var leak in report.Leaks)
        {
            output.WriteLine($"LEAK: {leak.Size} bytes at {leak.Ptr}  [{leak.File}:{leak.Line}]");
            if (leak.IsArray) output.WriteLine("[Array]");
            output.WriteLine();
        }
    }

    public string Format(AnalysisReport report)
    {
        var sb = new StringBuilder();
        using var writer = new StringWriter(sb);
        Write(report, writer);
        return sb.ToString();
    }
}