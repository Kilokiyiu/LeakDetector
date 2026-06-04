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
