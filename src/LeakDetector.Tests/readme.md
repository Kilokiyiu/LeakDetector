# LeakDetector.Tests

**一句话**：用 xUnit 验证 Parsers 解析与 Core 对账行为。

---

## 测试内容

### JsonlEventParserTests

- 读取 `testdata/sample_events.jsonl`（相对测试输出目录上溯到仓库根）  
- 断言：2 个事件；依次为 `AllocEvent`、`SummaryEvent`

### SimpleLeakAnalyzerTests

| 用例 | 场景 | 期望 |
|------|------|------|
| `SimpleLeak_OneAllocNoFree_ReportsOneLeak` | 仅 alloc | 1 处泄漏，4 字节，`main.cpp:7` |
| `NoLeak_AllocThenFree_ReportsZero` | alloc 后 free | `LeakCount=0`，`UnknownFreeCount=0` |
| `ArrayLeak_MarkedAsArray` | `kind=new[]` | `IsArray == true` |

分析器用手写 `MemoryEvent` 构造，不经过 JSON。

本项目引用：`LeakDetector.Core`、`LeakDetector.Parsers`。  
不含对 Analyzers、Reporters、CLI 的测试。

---

## 怎么跑

```text
dotnet test src/LeakDetector.Tests
```

或在解决方案根：

```text
dotnet test
```
