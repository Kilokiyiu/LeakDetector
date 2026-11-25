# LeakDetector.Analyzers

**一句话**：店长——把「翻译」和「对账」串成一次完整分析。

---

## 干什么 / 不干什么

| 做 | 不做 |
|----|------|
| 对外提供：日志路径 → 分析报告 | 自己解析 JSON 字段 |
| 内部串联 Parsers + Core | 自己实现配对算法 |
| | 决定怎么打印报告 |

职责是编排，不是堆业务细节。

---

## 主要类型

### LeakAnalysisService

```csharp
public sealed class LeakAnalysisService
{
    private readonly SimpleLeakAnalyzer _analyzer = new();

    public AnalysisReport AnalyzeLogFile(string path)
    {
        var jsonEvents = JsonlEventParser.ParseFile(path);
        return _analyzer.Analyze(jsonEvents);
    }
}
```

1. `JsonlEventParser.ParseFile(path)` → 事件列表  
2. `_analyzer.Analyze(...)` → `AnalysisReport`

---

## 谁调用它

`LeakDetector.CLI`：

```csharp
var report = new LeakAnalysisService().AnalyzeLogFile(path);
```

---

## 依赖

| 引用 | 用途 |
|------|------|
| `LeakDetector.Core` | `SimpleLeakAnalyzer`、`AnalysisReport` |
| `LeakDetector.Parsers` | `JsonlEventParser` |

---

## 在管线中的位置

```
CLI
 │
 ▼
LeakAnalysisService.AnalyzeLogFile
 ├─► JsonlEventParser.ParseFile     (Parsers)
 └─► SimpleLeakAnalyzer.Analyze     (Core)
      │
      ▼
 AnalysisReport
```
