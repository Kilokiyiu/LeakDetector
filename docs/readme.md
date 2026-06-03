# 学习用文档

个人学习 Valgrind、Memcheck、C++ 内存检测相关内容的笔记与规划，**不属于项目正式文档**。

项目正式文档见：[LeakDetector/docs/README.md](../LeakDetector/docs/README.md)

---

## 文档索引

| 文档 | 说明 |
|------|------|
| [readme.md](readme.md) | 学习笔记：内存基础、GC、影子内存、C++ 泄漏（§1–§4） |
| [mc_include.md](mc_include.md) | Valgrind 堆追踪模块（MC_Chunk、RedZone 等） |
| [learning-and-development-plan.md](learning-and-development-plan.md) | 学习与开发概要规划 |
| [detailed-roadmap.md](detailed-roadmap.md) | 详细开发路线（分阶段、验收标准） |

### 实现规格（学习/练手用）

| 文档 | 说明 |
|------|------|
| [spec/v0.1-leak-detector-h.md](spec/v0.1-leak-detector-h.md) | `include/leak_detector.h` v0.1 规格 |
| [spec/example-01-simple-leak.md](spec/example-01-simple-leak.md) | 首个 C++ 示例规格 |

### Valgrind 源码

| 路径 | 说明 |
|------|------|
| [valgrind/](valgrind/) | 克隆的 Valgrind 源码 |
| [valgrind/memcheck/学习记录/](valgrind/memcheck/学习记录/) | 按文件整理的精读笔记 |

### 其他参考

| 路径 | 说明 |
|------|------|
| [../开发方案/内存泄漏检测工具方案.md](../开发方案/内存泄漏检测工具方案.md) | 早期开发方案（宏替换 MVP） |
