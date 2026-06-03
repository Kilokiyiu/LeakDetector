# LeakDetector 项目文档

面向 C/C++ 的内存泄漏检测工具（参考 Valgrind Memcheck），后端使用 C#（.NET 10），通过宏替换拦截 `new`/`delete` 并分析分配记录。


---

## 项目结构

```
LeakDetector/
├── include/                 # C++ 注入头文件（leak_detector.h）
├── examples/                # C++ 测试示例
├── src/
│   ├── LeakDetector.Core/
│   ├── LeakDetector.Parsers/
│   ├── LeakDetector.Analyzers/
│   ├── LeakDetector.Reporters/
│   ├── LeakDetector.CLI/
│   └── LeakDetector.Tests/
└── docs/                    # 本目录：项目正式文档
```

---

## 构建

```bash
cd LeakDetector
dotnet build LeakDetector.sln
dotnet test
```

### C++ 注入头文件 `leak_detector.h`

v0.1 在目标 C++ 程序中拦截 `new`/`delete`，维护分配表并在 `ReportLeaks()` 时报告泄漏。作用说明与实现规格见 [学习用/spec/v0.1-leak-detector-h.md](../../学习用/spec/v0.1-leak-detector-h.md)。

---

## 文档分工

| 位置 | 用途 |
|------|------|
| `LeakDetector/docs/` | 项目说明、架构、构建与使用 |
| `学习用/学习笔记.md` | 个人学习笔记（原 `docs/readme.md`） |
| `学习用/` | 规划、练手规格、Valgrind 精读 |
