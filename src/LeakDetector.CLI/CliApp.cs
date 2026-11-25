using LeakDetector.Core.Model;
using LeakDetector.Reporters;

namespace LeakDetector.CLI;

public class CliApp
{
    //Func<string, AnalysisReport> analyzeFile是一个委托，输入string类型，输出一个AnalysisReport的报告，analyzeFile是这个获取的对象
    public static int Run(string[] args, Func<string, AnalysisReport> analyzeFile, TextWriter stdout, TextWriter stderr)
    {
        if (args.Length != 2 || !string.Equals(args[0], "analyze", StringComparison.OrdinalIgnoreCase))
        {
            stderr.WriteLine("Usage: LeakDetector.CLI analyze <path-to.jsonl>");
            return 3;
        }

        var path = args[1];
        if (!File.Exists(path))
        {
            stdout.WriteLine($"File {path} does not exist");
            return 2;
        }

        try
        {
            var report = analyzeFile(path);
            new ConsoleReporter().Write(report, stdout);
            return report.LeakCount > 0 ? 1 : 0;
        }
        catch (Exception e)
        {
            stderr.WriteLine(e.Message);
            return 4;
        }
    }
}