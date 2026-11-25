# LeakDetector.Reporters

**一句话**：播报员——把 `AnalysisReport` 写成可读文本。

---

## 干什么 / 不干什么

| 做 | 不做 |
|----|------|
| 把报告写成文本 | 打开 / 解析日志文件 |
| 写入任意 `TextWriter` | 重新计算谁泄漏 |

---

## 主要类型

### ConsoleReporter

| 方法 | 作用 |
|------|------|
| `Write(report, TextWriter)` | 写入指定输出（CLI 使用 `Console.Out`） |
| `Format(report)` | 返回与 `Write` 相同内容的字符串 |

`Format` 通过 `StringWriter` 调用 `Write`。

---

## 输出格式

### 无泄漏

```text
[LeakDetector] No leaks detected
```

### 有泄漏

```text
[LeakDetector] Found N leaks(S):
LEAK: <size> bytes at <ptr>  [<file>:<line>]

```

若 `IsArray == true`，在该条 `LEAK:` 下多一行：

```text
[Array]
```

`Write` / `Format` 只输出上述泄漏相关文案；不输出 `UnknownFreeCount`、`TotalLeakBytes`。

---

## 依赖

→ `LeakDetector.Core`（`AnalysisReport` / `LeakEntry`）

---

## 在管线中的位置

```
AnalysisReport
  │
  ▼
ConsoleReporter.Write(..., Console.Out)
  │
  ▼
终端文本
```
