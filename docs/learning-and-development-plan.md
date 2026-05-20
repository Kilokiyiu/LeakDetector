# LeakDetector 学习与开发规划

> 基于现有学习笔记（§1-3）和项目架构，制定从理论到交付的完整路线

---

## 当前状态

| 项目 | 状态 |
|------|------|
| 学习笔记 | §1 内存基础 ✅ / §2 GC原理 ✅ / §3 Memcheck影子内存 ✅ / §4 待补 |
| 项目骨架 | 6 个 csproj 已创建，均为空壳（Class1.cs 占位） |
| Valgrind 源码 | 已克隆，memcheck/ 目录可用 |
| 目标框架 | .NET 10.0（net10.0） |

---

## Phase 1: 补全理论基础（补全 §4）

> 你的 §1-3 已经覆盖了"内存是什么"和"检测工具怎么工作的"，但还缺一块关键内容：**C++ 内存泄漏到底是怎么发生的**。

### 1.1 补充 §4：C++ 内存泄漏的本质

需要理解的核心问题：

1. **泄漏的定义**：分配了内存但从未释放，且程序已无法再访问到该指针
2. **泄漏的分类**（对标 Valgrind mc_leakcheck.c 的 9 种 case）：
   - **Definitely Lost**：没有任何指针指向该内存块（最严重）
   - **Indirectly Lost**：指向该内存块的指针本身也在泄漏内存中
   - **Possibly Lost**：只有内部指针指向该块（而非起始地址）
   - **Still Reachable**：程序退出时仍有指针指向（通常可忽略）
3. **C++ 中常见的泄漏场景**：
   - `new` 了忘记 `delete`
   - `new[]` 却用 `delete`（而非 `delete[]`）
   - 异常抛出导致 `delete` 未执行
   - 智能指针循环引用
   - 析构函数中未释放成员指针

### 1.2 学习任务

| 任务 | 产出 | 参考资源 |
|------|------|---------|
| 写 §4 笔记 | docs/readme.md §4 补全 | Valgrind mc_leakcheck.c 前 120 行注释 |
| 编写 5 个 C++ 泄漏示例程序 | include/leak_examples.cpp | 自己写，覆盖上述 5 种场景 |
| 用 Valgrind 跑这 5 个示例 | 截图/记录输出 | WSL 中 `valgrind --leak-check=full` |

### 1.3 关键认知目标

完成 Phase 1 后，你应该能回答：
- 什么算泄漏？什么不算？
- Valgrind 的 4 种泄漏分类分别是什么意思？
- 为什么 `new[]` + `delete`（非 `delete[]`）是问题？

---

## Phase 2: Valgrind 源码精读（原理对标）

> 你已经学了 Memcheck 的影子内存机制（§3），现在要深入理解它是**怎么拦截 malloc/free 并追踪泄漏的**。

### 2.1 精读路线

按以下顺序阅读 Valgrind memcheck 源码（`学习用/valgrind/memcheck/`）：

```
阅读顺序：
memcheck.h → mc_include.h → mc_malloc_wrappers.c → mc_leakcheck.c → mc_main.c
```

| 文件 | 核心关注点 | 你要学到的 |
|------|-----------|-----------|
| `memcheck.h` | 公共 API，`VALGRIND_DO_LEAK_CHECK` 宏 | 工具如何暴露接口给用户 |
| `mc_include.h` | `MC_Chunk` 结构体、`MC_AllocKind` 枚举 | **数据结构设计参考**：一个内存块需要记录什么 |
| `mc_malloc_wrappers.c` | 拦截 malloc/calloc/free/new/delete | **拦截机制**：怎么在分配和释放时做手脚 |
| `mc_leakcheck.c` | 泄漏检测算法（图遍历 + 9 分类） | **检测算法**：怎么判断一块内存是否泄漏 |
| `mc_main.c` | 工具主入口，注册回调 | 整体框架怎么串起来的 |

### 2.2 对标思考

读源码时，始终带着这个问题：

> **Valgrind 用了 X 方案，我的 LeakDetector 该用什么方案？**

| Valgrind 方案 | LeakDetector 对标 |
|--------------|------------------|
| 二进制插桩拦截 malloc/free | 宏替换 `#define new` / `#define delete` |
| MC_Chunk 哈希表追踪分配 | Core 模块的 AllocationRecord |
| 运行时扫描栈/堆/寄存器找指针 | Parsers 解析源码 + Analyzers 静态分析 |
| C 代码，直接操作内存 | C# / .NET 8.0，面向对象设计 |

### 2.3 学习任务

| 任务 | 产出 |
|------|------|
| 精读 mc_include.h，画出 MC_Chunk 的数据结构 | 笔记加入 docs/readme.md §5 |
| 精读 mc_malloc_wrappers.c，总结拦截点 | 笔记加入 docs/readme.md §5 |
| 精读 mc_leakcheck.c，画出泄漏检测流程图 | 笔记加入 docs/readme.md §5 |
| 写一份"Valgrind vs LeakDetector 技术路线对比" | docs/valgrind-vs-leakdetector.md |

---

## Phase 3: 架构设计（数据结构 + 模块接口）

> 你已有 6 个项目模块的骨架，现在需要定义清楚每个模块做什么、模块间怎么通信。

### 3.1 核心数据结构设计

参考 Valgrind 的 `MC_Chunk`，设计你的核心模型：

```
AllocationRecord（对标 MC_Chunk）
├── Address      : ulong    // 分配地址
├── Size         : int      // 分配大小
├── AllocKind    : enum     // new / new[] / malloc / custom
├── AllocSource   : string  // 分配发生的源文件:行号
├── AllocCallstack: string[] // 分配时的调用栈
├── IsFreed      : bool     // 是否已释放
├── FreeSource    : string? // 释放发生的源文件:行号（若已释放）
└── Timestamp    : DateTime // 分配时间
```

### 3.2 模块职责与接口

```
┌─────────────────────────────────────────────────────┐
│                      CLI                             │
│  解析命令行参数 → 调度 Parsers/Analyzers/Reporters  │
└──────────┬──────────────────────────────────────────┘
           │
    ┌──────┼──────────────────────┐
    ▼      ▼                      ▼
┌────────┐ ┌──────────┐  ┌───────────┐
│ Parsers│ │ Analyzers│  │ Reporters │
│ 解析C++│ │ 泄漏分析  │  │ 报告输出  │
│ 源文件 │ │ 配对检查  │  │ 控制台/文件│
└───┬────┘ └────┬─────┘  └────┬──────┘
    │           │              │
    ▼           ▼              ▼
┌──────────────────────────────────────┐
│              Core                     │
│  数据模型 + 内存分配记录 + 接口定义   │
└──────────────────────────────────────┘
```

| 模块 | 职责 | 核心接口 |
|------|------|---------|
| **Core** | 数据模型、接口定义、分配记录存储 | `IAllocationTracker`, `AllocationRecord`, `LeakResult` |
| **Parsers** | 解析 C++ 源码，识别 new/delete 调用 | `ISourceParser`, `ParseResult` |
| **Analyzers** | 对分配/释放记录做配对分析 | `ILeakAnalyzer`, `AnalysisReport` |
| **Reporters** | 输出泄漏报告（控制台/JSON/HTML） | `IReportFormatter`, `IReportWriter` |
| **CLI** | 命令行入口，参数解析，流程编排 | `Program.Main()` |
| **Tests** | 单元测试 + 集成测试 | xUnit 测试类 |

### 3.3 宏替换方案设计

这是整个工具的核心机制。需要在 `include/` 目录放置 C++ 头文件：

```cpp
// leak_detector.h（用户 #include 这个文件）
#ifdef LEAK_DETECTOR_ENABLED
  #define new    LeakDetector::Recorder::Record(__FILE__, __LINE__) * new
  #define delete LeakDetector::Recorder::RecordFree(__FILE__, __LINE__); delete
#endif
```

**关键设计决策**：
- 宏替换 vs 运算符重载：宏替换更简单，但有陷阱（如 placement new）
- 需要考虑：`new(std::nothrow)` / `new(p) T` (placement new) / `new T[n]` 等变体
- 记录数据写到哪里：共享内存文件？标准输出？回调函数？

### 3.4 学习任务

| 任务 | 产出 |
|------|------|
| 设计 Core 模块的接口和模型类 | src/LeakDetector.Core/Interface/ + Model/ |
| 设计宏替换头文件 | include/leak_detector.h |
| 画出完整的模块依赖关系图 | docs/architecture.md |
| 确定模块间项目引用关系 | 更新各 .csproj 的 ProjectReference |

---

## Phase 4: 核心实现（Core → Parsers → Analyzers）

> 从最内层模块开始，逐层向外实现。

### 4.1 实现顺序

```
Step 1: Core（数据模型 + 接口）
  ↓
Step 2: include/leak_detector.h（宏替换头文件）
  ↓
Step 3: Parsers（源码解析器）
  ↓
Step 4: Analyzers（泄漏分析器）
  ↓
Step 5: 冒烟测试（用 §1.2 的泄漏示例程序验证）
```

### 4.2 各步骤详细说明

**Step 1: Core 模块**
- `AllocationRecord` 模型类
- `LeakResult` 模型类（含泄漏类型：Definite/Indirect/Possible/Reachable）
- `IAllocationTracker` 接口（RegisterAlloc / RegisterFree / GetLeaks）
- `IAllocationStore` 接口（内存存储实现：Dictionary 或自定义数据结构）
- 编写 Core 的单元测试

**Step 2: 宏替换头文件**
- `include/leak_detector.h`：定义宏和记录器类
- 记录器类（C++）：在 new/delete 时把信息写到共享内存或标准输出
- 需要解决：C++ 侧如何把数据传给 C# 侧（管道？共享内存？文件？）

**Step 3: Parsers 模块**
- 解析 C++ 源码文件，提取 new/delete 调用信息
- 两种解析策略（先简单后复杂）：
  1. **正则匹配**（V1）：用正则表达式识别 `new T` 和 `delete p` 模式
  2. **语法树解析**（V2）：用 libclang 或 Tree-sitter 做精确解析
- 编写 Parsers 的单元测试

**Step 4: Analyzers 模块**
- 接收 Parsers 的输出，执行配对分析
- 对标 Valgrind 的泄漏分类算法
- 核心逻辑：遍历所有分配记录，找出没有配对释放的
- 编写 Analyzers 的单元测试

**Step 5: 冒烟测试**
- 用 Phase 1 写的 5 个 C++ 泄漏示例作为测试输入
- 验证工具能否正确检测出所有泄漏

### 4.3 学习要点

每个步骤实现时，需要额外学习的知识点：

| 步骤 | 需要学习的内容 |
|------|--------------|
| Core | C# record 类型、接口设计模式、.NET 集合性能 |
| 宏替换 | C++ 预处理器宏、placement new、RAII 模式 |
| Parsers | 正则表达式高级用法、编译原理基础（词法分析） |
| Analyzers | 图遍历算法、引用追踪 |
| 冒烟测试 | C++/C# 跨语言通信、进程间通信（IPC） |

---

## Phase 5: 交付完善（CLI + Reporters + Tests）

> 核心功能跑通后，完善用户体验和测试覆盖。

### 5.1 CLI 模块

- 使用 System.CommandLine 或 Spectre.Console.Cli 解析命令行参数
- 支持的命令：
  ```
  leakdetector analyze <source-dir>     # 分析指定目录
  leakdetector report <input-file>      # 生成报告
  leakdetector run <executable>         # 运行并检测（未来）
  ```

### 5.2 Reporters 模块

- 控制台报告（对标 Valgrind 输出格式，中文版）
- JSON 报告（结构化，方便集成 CI）
- HTML 报告（带语法高亮，方便查看）
- 泄漏报告格式参考：
  ```
  ==LeakDetector== 泄漏摘要
  ==LeakDetector==   definitely lost: 1,024 bytes in 2 blocks
  ==LeakDetector==   indirectly lost: 512 bytes in 1 blocks
  ==LeakDetector==   possibly lost: 0 bytes in 0 blocks
  ==LeakDetector==   still reachable: 256 bytes in 1 blocks
  ```

### 5.3 Tests 模块

- Core 单元测试：AllocationRecord 创建、配对、泄漏分类
- Parsers 单元测试：各种 new/delete 模式的解析
- Analyzers 单元测试：泄漏检测算法的各种场景
- Reporters 单元测试：报告格式验证
- 集成测试：端到端流程

### 5.4 额外完善

- [ ] README.md 更新（使用说明、构建方式、示例）
- [ ] GitHub Actions CI 配置
- [ ] NuGet 包发布准备（可选）

---

## 时间线建议

| 阶段 | 预估周期 | 里程碑 |
|------|---------|--------|
| Phase 1 | 3-5 天 | §4 笔记完成，5 个 C++ 泄漏示例跑通 |
| Phase 2 | 5-7 天 | 源码精读笔记完成，技术对比文档输出 |
| Phase 3 | 3-5 天 | 接口定义完成，架构文档输出 |
| Phase 4 | 10-15 天 | Core + Parsers + Analyzers 实现 + 冒烟测试通过 |
| Phase 5 | 7-10 天 | CLI + Reporters + 完整测试 + README |

---

## 学习资源索引

| 资源 | 位置 | 用途 |
|------|------|------|
| Valgrind 源码 | `G:/3.Projects_Indpnd/LeakDetector/学习用/valgrind/` | 原理对标 |
| memcheck 核心文件 | `valgrind/memcheck/mc_*.c` | 拦截/检测算法参考 |
| Valgrind 用户手册 | `valgrind/docs/xml/mc-manual.xml` | 泄漏分类详细说明 |
| 你的学习笔记 | `docs/readme.md` | 理论基础沉淀 |
| AddressSanitizer | https://github.com/google/sanitizers | 另一种检测方案对比 |
