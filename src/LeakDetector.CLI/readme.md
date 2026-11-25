# LeakDetector.CLI

**一句话**：前台——命令行入口：收日志路径、跑分析、打印结果、返回退出码。

---

## 干什么 / 不干什么

| 做 | 不做 |
|----|------|
| 检查参数是否为 `analyze <文件>` | 解析 JSON |
| 检查文件是否存在 | 配对 alloc/free |
| 调用 Analyzers 得到报告 | 拼装泄漏文案（由 Reporters 完成） |
| 调用 Reporters 打印 | |
| 按结果返回退出码 | |

---

## 怎么跑

仓库根目录：

```text
dotnet run --project src/LeakDetector.CLI -- analyze testdata/sample_events.jsonl
```

```text
dotnet run --project src/LeakDetector.CLI -- analyze examples/simple_leak/leak_log.jsonl
```

`--` 之后的参数传给本程序。

---

## 执行顺序

`Program.Main`：

1. 参数须为 2 个，且第一个（忽略大小写）为 `analyze`；否则 stderr 打印 Usage，退出 **3**  
2. 文件不存在：stdout 提示，退出 **2**  
3. `LeakAnalysisService.AnalyzeLogFile(path)`  
4. `ConsoleReporter.Write(report, Console.Out)`  
5. `LeakCount > 0` → 退出 **1**，否则 **0**  
6. 未捕获异常：stderr 打印 `e.Message`，退出 **4**

---

## 退出码

| 码 | 含义 |
|----|------|
| 0 | 分析完成，无泄漏 |
| 1 | 分析完成，发现泄漏 |
| 2 | 文件不存在 |
| 3 | 用法错误 |
| 4 | 运行异常 |

「有泄漏」与「工具异常」使用不同非零码。

---

## 例子

`testdata/sample_events.jsonl`（alloc + summary，无 free）：

```text
[LeakDetector] Found 1 leaks(S):
LEAK: 4 bytes at 0x000001A2B3C4D5E0  [main.cpp:7]
```

退出码 **1**。

`testdata/sample_no_leak.jsonl`（alloc + free + summary）：无泄漏文案，退出码 **0**。

---

## 依赖

| 引用 | 用途 |
|------|------|
| `LeakDetector.Analyzers` | `LeakAnalysisService` |
| `LeakDetector.Reporters` | `ConsoleReporter` |

---

## 在管线中的位置

```
.jsonl
  │
  ▼
CLI（参数 / 文件检查）
  │
  ▼
Analyzers → Parsers + Core
  │
  ▼
Reporters → 终端
  │
  ▼
退出码 0 / 1 / 2 / 3 / 4
```
