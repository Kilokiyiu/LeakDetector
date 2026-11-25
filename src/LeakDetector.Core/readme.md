# LeakDetector.Core

**一句话**：共同语言（事件/报告长什么样）+ 对账算法（有没有泄漏）。

其他项目都围着它转：Parsers 往里填事件，Analyzers 调用它的分析器，Reporters 读它的报告。

---

## 干什么 / 不干什么

| 做 | 不做 |
|----|------|
| 定义「借了 / 还了 / 总结」长什么样 | 读 `.jsonl` 文件 |
| 定义「一条泄漏 / 一份报告」长什么样 | 打印到控制台 |
| 用 alloc/free 配对算出谁漏了 | 解析命令行 |

通俗说：这里只有**词典**和**会计**，没有前台、没有翻译。

---

## 目录结构

```
Model/          # 词典
  MemoryEvent.cs      # 所有事件的共同祖先
  AllocEvent.cs       # 借内存
  FreeEvent.cs        # 还内存
  SummaryEvent.cs     # C++ 侧汇总事件
  LeakEntry.cs        # 一笔未释放的分配
  AnalysisReport.cs   # 整份分析结论
Services/       # 大脑
  SimpleLeakAnalyzer.cs
```

---

## Model 详解

### MemoryEvent（基类）

```csharp
public abstract record MemoryEvent(int Id);
```

所有事件至少有一个 `Id`（事件序号）。`SimpleLeakAnalyzer` 按 `Id` 排序后再复盘。

### AllocEvent（借）

| 字段 | 含义 |
|------|------|
| `Id` | 事件序号 |
| `Ptr` | 指针（字符串） |
| `Size` | 字节数 |
| `Kind` | `"new"` 或 `"new[]"` |
| `File` / `Line` | 分配所在源文件与行号 |
| `IsArray` | 计算属性：`Kind == "new[]"` |

### FreeEvent（还）

| 字段 | 含义 |
|------|------|
| `Id` | 事件序号 |
| `Ptr` | 被释放的指针 |
| `Kind` | `"delete"` 或 `"delete[]"` |

配对只按 `Ptr`；不比较 alloc/free 的 `Kind` 是否对应。

### SummaryEvent（汇总）

| 字段 | 含义 |
|------|------|
| `LeakCount` | 汇总中的泄漏条数 |
| `TotalLeakBytes` | 汇总中的泄漏总字节 |

`SimpleLeakAnalyzer` 遇到该类型时跳过；泄漏结论只来自 alloc/free 对账。

### LeakEntry（一条泄漏）

| 字段 | 含义 |
|------|------|
| `Ptr` / `Size` | 指针与大小 |
| `File` / `Line` | 分配位置 |
| `IsArray` | 是否数组分配 |

### AnalysisReport（整份报告）

| 成员 | 含义 |
|------|------|
| `Leaks` | 未释放分配列表 |
| `UnknownFreeCount` | 释放时账本中不存在该指针的次数 |
| `LeakCount` | = `Leaks.Count` |
| `TotalLeakBytes` | 所有泄漏 `Size` 之和 |

---

## Services：SimpleLeakAnalyzer

### 行为

1. 按 `Id` 排序  
2. 用 `Dictionary<指针, LeakEntry>` 跟踪存活分配  
3. `AllocEvent`：写入（同指针再次 alloc 会覆盖）  
4. `FreeEvent`：移除；移除失败则 `UnknownFreeCount++`  
5. `SummaryEvent`：忽略  
6. 返回 `AnalysisReport(live 中剩余项, unknownFreeCount)`

### 输入 / 输出

```text
IEnumerable<MemoryEvent>  ──►  Analyze()  ──►  AnalysisReport
```

### 例子

仅有 `AllocEvent(ptr=0x1, size=4, file=main.cpp, line=7)` 与 `SummaryEvent`：  
→ `LeakCount=1`，条目指向 `main.cpp:7`。

若另有匹配的 `FreeEvent(ptr=0x1)`：  
→ `LeakCount=0`。

---

## 依赖

无项目引用。
