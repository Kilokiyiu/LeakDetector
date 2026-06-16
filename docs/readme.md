# LeakDetector 项目文档

面向 C/C++ 的内存泄漏检测：宏替换拦截 `new`/`delete`，记录分配并在程序结束时报告未释放块。

---

## 当前进度（2026-06）

### v0.1 — C++ 控制台报告

| 模块 | 状态 |
|------|------|
| `include/leak_detector.h` + `leak_detector.cpp` | ✅ 可用（MSVC Debug 已验证） |
| `examples/simple_leak` | ✅ 可构建、可运行 |
| `examples/no_leak` | ✅ |
| `examples/array_leak` | ✅ |
| `examples/exception_leak` | ✅ |
| `examples/multi_leak` | ✅ |
| `examples/new_delete_mismatch` | ⬜ 未做 |
| `examples/double_delete` | ⬜ 未做 |

### v0.2 — JSONL 日志 + C# 分析

| 模块 | 状态 |
|------|------|
| C++ JSONL 写入（`alloc` / `free` / `summary`） | ⬜ |
| `LeakDetector.Core` | ⬜ 空壳 |
| `LeakDetector.Parsers` | ⬜ 空壳 |
| `LeakDetector.Analyzers` | ⬜ 空壳 |
| `LeakDetector.Reporters` | ⬜ 空壳 |
| `LeakDetector.CLI` | ⬜ 空壳 |
| `LeakDetector.Tests` | ⬜ 空壳 |

### 后续版本

| 版本 | 目标 | 状态 |
|------|------|------|
| v0.3 | 完整 CLI + 测试 + CI | ⬜ |
| v0.4+ | 调用栈、泄漏分类、多线程 | ⬜ |
