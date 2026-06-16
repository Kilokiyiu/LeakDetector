# LeakDetector 项目文档

面向 C/C++ 的内存泄漏检测：宏替换拦截 `new`/`delete`，记录分配并在程序结束时报告未释放块。长期目标是用 C# 分析日志；当前 **v0.1 为纯 C++ 自包含**。

---

## 当前进度（2026-06）

| 模块 | 状态 |
|------|------|
| `include/leak_detector.h` + `leak_detector.cpp` | ✅ 可用（MSVC Debug 已验证） |
| `examples/simple_leak` | ✅ 可构建、可运行 |
| 其余示例（`no_leak` … `multi_leak`） | ✅ 核心 5 个已完成 |
| C#（Core / Parsers / CLI …） | ⬜ 空壳，见 [v0.2 自制指南](../../学习用/spec/v0.2-implementation-guide.md) |
| 学习笔记 §4 | ⬜ 待补 |

**下一步**：按 [v0.2-implementation-guide.md](../../学习用/spec/v0.2-implementation-guide.md) 自制 v0.2（C++ JSONL + C# analyze）。

---

## 快速开始

```powershell
cd LeakDetector\examples\simple_leak
.\build.ps1
```

预期输出：

```
value = 42
[LeakDetector] No leaks detected.
```

构建说明见 [examples/simple_leak/README.md](../examples/simple_leak/README.md)。

---

## 代码结构

```
LeakDetector/
├── include/
│   ├── leak_detector.h      # 声明 + #define new
│   └── leak_detector.cpp    # 实现（必须参与链接）
├── examples/
│   └── simple_leak/         # 首个示例
├── src/                     # C# 项目（v0.2+）
│   ├── LeakDetector.Core/
│   ├── LeakDetector.Parsers/
│   ├── LeakDetector.Analyzers/
│   ├── LeakDetector.Reporters/
│   ├── LeakDetector.CLI/
│   └── LeakDetector.Tests/
└── docs/                    # 本目录
```

### 接入方式（v0.1）

1. 编译时链接 `leak_detector.cpp`
2. 在**最后一个** `#include` 引入 `leak_detector.h`
3. 程序结束前调用 `ReportLeaks()`
4. 该头文件之后避免 `#include <iostream>` 等会触发 STL 内部分配的代码；输出用 `printf`

实现细节与 MSVC 注意点见 [v0.1 规格 · 实现差异](../../学习用/spec/v0.1-leak-detector-h.md)。

---

## 文档地图

### 日常开发（优先）

| 文档 | 说明 |
|------|------|
| **本文档** | 项目入口、进度、构建 |
| [detailed-roadmap.md](../../学习用/detailed-roadmap.md) | 分阶段路线、7 个示例清单、验收标准 |
| [spec/v0.1-leak-detector-h.md](../../学习用/spec/v0.1-leak-detector-h.md) | `leak_detector` 设计与编写指南 |
| [spec/v0.2-implementation-guide.md](../../学习用/spec/v0.2-implementation-guide.md) | **v0.2 自制指南**（JSONL + C# 分步任务） |
| [examples/simple_leak/README.md](../examples/simple_leak/README.md) | 示例 #01 构建说明 |
| [examples/README.md](../examples/README.md) | **expected_output.txt 格式与验收说明** |

### 学习与参考（按需）

| 文档 | 说明 |
|------|------|
| [学习用/readme.md](../../学习用/readme.md) | 学习区总索引 |
| [学习笔记.md](../../学习用/学习笔记.md) | 内存、GC、影子内存（§1–§3 ✅，§4 待补） |
| [mc_include.md](../../学习用/mc_include.md) | Valgrind 堆追踪（MC_Chunk、RedZone） |
| [learning-and-development-plan.md](../../学习用/learning-and-development-plan.md) | 早期概要规划（**部分已过期**，以 roadmap 为准） |
| [开发方案/内存泄漏检测工具方案.md](../../开发方案/内存泄漏检测工具方案.md) | 最初产品设想（存档） |

### Valgrind 源码

克隆在 `学习用/valgrind/`，精读笔记在 `学习用/valgrind/memcheck/学习记录/`。

---

## 版本路线（摘要）

```
v0.1  C++ 头文件 + 控制台报告          ← 已完成
v0.2  C++ 写 JSONL → C# 解析分析      ← 进行中，见 v0.2 自制指南
v0.3  完整 CLI + 测试 + CI
v0.4+ 调用栈、泄漏分类、多线程
```

详见 [detailed-roadmap.md § 二](../../学习用/detailed-roadmap.md#二总体架构演进三版产品)。
