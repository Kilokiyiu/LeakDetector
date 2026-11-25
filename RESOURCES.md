# LeakDetector Resources

## Knowledge

- [JSON Lines 规范 — jsonlines.org](https://jsonlines.org/)
  JSONL 三条硬规则（UTF-8、每行一个 JSON、`\n` 分隔）。Use for: 解释为什么日志是「一行一事」而不是一个大 JSON 数组。

- [How to use a JSON DOM (System.Text.Json) — Microsoft Learn](https://learn.microsoft.com/en-us/dotnet/standard/serialization/system-text-json/use-dom)
  `JsonDocument` / `RootElement` / `GetProperty` / `using` 释放。Use for: 复习 `JsonlEventParser.ParseLine`。

- [System.Text.Json overview — Microsoft Learn](https://learn.microsoft.com/en-us/dotnet/standard/serialization/system-text-json/overview)
  序列化/反序列化总览；和 DOM 方式对比。Use for: 「为什么这里用手写 DOM 而不是直接 `Deserialize<T>`」。

- [new operator (C++) — Microsoft Learn](https://learn.microsoft.com/en-us/cpp/cpp/new-operator-cpp?view=msvc-170)
  `new` 表达式、placement 参数、与 `operator new` 的关系。Use for: 解释 `#define new new(__FILE__, __LINE__)` 在干什么。

- [\<new\> operators — Microsoft Learn](https://learn.microsoft.com/en-us/cpp/standard-library/new-operators?view=msvc-170)
  可替换/可重载的分配函数签名。Use for: 对照 `include/leak_detector.cpp` 里的 `operator new`/`delete`。

- 本仓库源码（最高优先级）
  `include/leak_detector.*`、`src/LeakDetector.*`、`testdata/*.jsonl`、`examples/*`。Use for: 一切「项目实际怎么做」的问题以代码为准。

- [How to interrogate unfamiliar code — Stack Overflow Blog](https://stackoverflow.blog/2022/08/15/how-to-interrogate-unfamiliar-code/)
  读陌生代码：先扫轮廓再抠细节；用测试与具体值走一遍。Use for: 用户面对「不是自己写的示例」时的默认学法（课 0009）。

- [Reading Code You Didn't Write — Aalto OpenCS](https://csfoundations.cs.aalto.fi/en/courses/introduction-to-programming-summer-2026/part-1/9-reading-code-you-didnt-write)
  工作流：扫形状 → 输入输出 → 带具体输入追踪 → 预测再跑 → 人话解释。Use for: 五步循环的教学骨架。

- [Software Architecture Guide — Martin Fowler](https://martinfowler.com/architecture/)
  「架构 = 重要的东西」：识别并保护关键决策（边界、契约、可演进性），而非堆术语。Use for: 从 `ParseLine` 抽设计决策、拆新产品阶段（课 0010）。

- [docs/README.md](docs/README.md)
  文档导航与现行能力一览；目录职责说明。Use for: 找架构页入口、确认文档写什么/不写什么。

- [Environment.Exit(Int32) — Microsoft Learn](https://learn.microsoft.com/en-us/dotnet/api/system.environment.exit)
  约定：`0` = 成功完成；非零 = 错误/异常情况。Use for: 解释 CLI 退出码为何「有泄漏返回 0」不符合常见习惯。

- [$ - string interpolation — Microsoft Learn](https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/tokens/interpolated)
  字面量必须以 `$` 开头，`{expr}` 才会被求值；没有 `$` 就原样输出花括号。Use for: `ConsoleReporter` 漏 `$` 那一行。

- [AppContext.BaseDirectory — Microsoft Learn](https://learn.microsoft.com/en-us/dotnet/api/system.appcontext.basedirectory)
  程序集探测基目录（测试时通常是 `bin/Debug/netX.Y/`）。Use for: 数 `..` 回到仓库根找 `testdata`。

## Wisdom (Communities)

- [.NET Discord / Stack Overflow (`system.text.json`)](https://stackoverflow.com/questions/tagged/system.text.json)
  Use for: JSON API 边角行为（`GetProperty` 抛错、`IDisposable` 生命周期）。
- 社区偏好：未声明拒绝；需要深入疑难时再推荐，不强行拉群。

## Gaps

- 本仓库尚无正式「JSONL 事件契约」独立文档（字段表）——已用 `reference/jsonl-and-parsing.html` 补上。
- C++ example 的一键运行脚本/README 命令未统一成官方文档——运行步骤写在 `reference/how-to-run.html`，以 CMake example + `dotnet` 为准。
- **后期系统观测尚未入库的官方资料（待补）：** Windows 内存性能计数器 / ETW / 工作集 API；Linux `/proc/meminfo`、`smaps`、cgroup v2 memory、PSI。等开始做那一层再精选高信任链接写入 Knowledge。
