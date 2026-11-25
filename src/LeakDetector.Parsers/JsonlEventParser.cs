using System.Text.Json;
using LeakDetector.Core.Model;

namespace LeakDetector.Parsers;

/// <summary>
///     将 C++ LeakDetector 写出的 JSONL 日志解析为 Core 事件模型。
/// </summary>
public static class JsonlEventParser
{
    /// <summary>
    ///     读取 JSONL 文件，按行解析为有序事件列表。
    /// </summary>
    public static IReadOnlyList<MemoryEvent> ParseFile(string path)
    {
        if (!File.Exists(path)) throw new FileNotFoundException("Json log file not found", path);

        var events = new List<MemoryEvent>();
        foreach (var line in File.ReadLines(path))
        {
            if (string.IsNullOrWhiteSpace(line)) continue;
            events.Add(ParseLine(line));
        }

        return events;
    }

    private static MemoryEvent ParseLine(string line)
    {
        using var doc = JsonDocument.Parse(line);
        var root = doc.RootElement;

        var id = root.GetProperty("id").GetInt32();
        var eventType = root.GetProperty("event").GetString() ?? throw new JsonException("Missing 'event' Failed");

        return eventType switch
        {
            "alloc" => new AllocEvent(
                id,
                root.GetProperty("ptr").GetString()!,
                root.GetProperty("size").GetUInt64(),
                root.GetProperty("kind").GetString()!,
                root.GetProperty("file").GetString()!,
                root.GetProperty("line").GetInt32()),

            "free" => new FreeEvent(
                id,
                root.GetProperty("ptr").GetString()!,
                root.GetProperty("kind").GetString()!),

            "summary" => new SummaryEvent(
                id,
                root.GetProperty("leak_count").GetInt32(),
                root.GetProperty("total_leak_bytes").GetUInt64()),

            _ => throw new JsonException($"Unknown event type: {eventType}")
        };
    }
}