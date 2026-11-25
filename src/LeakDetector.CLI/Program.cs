using LeakDetector.Analyzers;
using LeakDetector.Reporters;

namespace LeakDetector.CLI;

internal static class Program
{
    private static int Main(string[] args)
    {
        // 测试用：
        return CliApp.Run(
            args,
            path => new LeakAnalysisService().AnalyzeLogFile(path),
            Console.Out,
            Console.Error);
        //  测试代码结束

        if (args.Length != 2 || !string.Equals(args[0], "analyze", StringComparison.OrdinalIgnoreCase))
        {
            Console.Error.WriteLine("Usage: LeakDetector.CLI analyze <path-to.jsonl>");
            return 3;
        }

        var path = args[1];
        if (!File.Exists(path))
        {
            Console.WriteLine($"File {path} does not exist");
            return 2;
        }

        try
        {
            var report = new LeakAnalysisService().AnalyzeLogFile(path);
            new ConsoleReporter().Write(report, Console.Out);
            // 0 = 分析成功且无泄漏；1 = 分析成功但发现泄漏（便于 CI 拦住）
            return report.LeakCount > 0 ? 1 : 0;
        }
        catch (Exception e)
        {
            Console.Error.WriteLine(e.Message);
            return 4;
        }
    }
}