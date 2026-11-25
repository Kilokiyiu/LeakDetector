# LeakDetector 文档导航

面向 C/C++ 的内存泄漏检测：进程内拦截 `new`/`delete` 记账；可选写出 JSONL，由 C# 管线解析、分析并报告。

文档只描述**当前架构与现行决策**。实现细节以仓库源码为准；变更历史看 git，不写进文档正文。

## 文档目录

| 路径 | 简要说明 |
|------|----------|
| [docs/](.) | 项目规格、示例说明、加固指南等 Markdown |
| [reference/](../reference/) | 架构速查（HTML）：模块边界、契约、运行方式 |
| [lessons/](../lessons/) | 短课：按 mission 练技能；入口 [lessons/index.html](../lessons/index.html) |
| [MISSION.md](../MISSION.md) | 学习/演进目标与范围约束 |
| [RESOURCES.md](../RESOURCES.md) | 外部高信任资料索引 |

若你已经能跑通管线，但不知道怎样读陌生示例、或怎样把方法里的设计迁移到新产品，请按下列顺序阅读学习方法材料：先打开课 [0009](../lessons/0009-learn-unread-code.html) 与速查 [读陌生代码五步](../reference/read-unread-code.html)，再打开课 [0010](../lessons/0010-from-method-to-architecture.html) 与速查 [产品架构梯子](../reference/product-architecture-ladder.html)。

## docs/ 内文

| 文档 | 说明 |
|------|------|
| [v0.1-leak-detector-h.md](v0.1-leak-detector-h.md) | C++ 插桩侧能力边界与用法（控制台报告） |
| [example-01-simple-leak.md](example-01-simple-leak.md) | 简单泄漏示例怎么跑、期望现象 |
| [v0.2-implementation-guide.md](v0.2-implementation-guide.md) | JSONL + C# 管线实现指南（动手步骤） |
| [v0.2-harden-guide.md](v0.2-harden-guide.md) | 契约测试与加固步骤（人话版） |
| [dev-roadmap.md](dev-roadmap.md) | 现行范围：已有能力、约束、明确不做 |
| [mc_include.md](mc_include.md) | 与 Valgrind 账本思路对照的学习笔记 |

## 当前架构（一句话）

**采集（C++）→ JSONL 契约 → 翻译（Parsers）→ 判断（Core）→ 展示（Reporters）→ 门面（CLI）。**  
Parsers 不做泄漏判断；CLI 不塞业务算法。细则见 [reference/architecture.html](../reference/architecture.html)、[reference/module-map.html](../reference/module-map.html)。

## 现行能力对照

| 能力 | 状态 |
|------|------|
| C++ 拦截 + 控制台报告 | 可用（`include/`、`examples/*`） |
| JSONL（alloc / free / summary） | 可用 |
| C#：Core / Parsers / Analyzers / Reporters / CLI | 可用 |
| 单元测试（解析 + 分析） | 有；CLI/Reporter 契约测试仍缺 |
| 系统内存观测（Win/Linux） | 未做；与插桩采集分层，不共用一种 Event |
| GUI | 不做 |

源码入口：`include/leak_detector.*`、`src/LeakDetector.*`、`testdata/*.jsonl`。
