# LeakDetector 项目文档

面向 C/C++ 的内存泄漏检测工具（参考 Valgrind Memcheck），后端使用 C#（.NET 10），通过宏替换拦截 `new`/`delete` 并分析分配记录。

**学习笔记**（§1–§4 内存/GC/影子内存等）见：[学习用/学习笔记.md](../../学习用/学习笔记.md)  
**学习文档索引**：[学习用/README.md](../../学习用/README.md)

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

---

## 文档分工

| 位置 | 用途 |
|------|------|
| `LeakDetector/docs/` | 项目说明、架构、构建与使用 |
| `学习用/学习笔记.md` | 个人学习笔记（原 `docs/readme.md`） |
| `学习用/` | 规划、练手规格、Valgrind 精读 |
