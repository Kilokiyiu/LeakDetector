# MEMORY.md - LeakDetector 项目长期记忆

## 项目概况
- **项目名**: LeakDetector - C++ 内存泄漏检测工具
- **技术栈**: C# / .NET 10.0 (net10.0)，六项目模块化架构
- **IDE**: JetBrains Rider
- **工作空间**: G:\3.Projects_Indpnd\LeakDetector\LeakDetector
- **GitHub**: https://github.com/Kilokiyiu/LeakDetector

## 模块架构
| 模块 | 职责 |
|------|------|
| LeakDetector.Core | 数据模型、接口定义、分配记录存储 |
| LeakDetector.Parsers | 解析C++源码，识别new/delete调用 |
| LeakDetector.Analyzers | 对分配/释放记录做配对分析 |
| LeakDetector.Reporters | 输出泄漏报告（控制台/JSON/HTML） |
| LeakDetector.CLI | 命令行入口，参数解析，流程编排 |
| LeakDetector.Tests | xUnit单元测试+集成测试 |

## 核心技术路线
- 通过宏替换拦截 new/delete 调用对
- 记录分配信息（地址、大小、源文件:行号）
- 静态分析配对 + 运行时追踪（双模式）

## 学习资源
- Valgrind 源码：G:/3.Projects_Indpnd/LeakDetector/学习用/valgrind/
- 关键参考文件：memcheck/mc_include.h、mc_malloc_wrappers.c、mc_leakcheck.c
- 用户学习笔记：docs/readme.md（§1-3已完成，§4待补）

## 项目阶段
- 截至 2026-05-20：规划阶段，尚未编码
- 规划文档：docs/learning-and-development-plan.md

## 用户偏好
- 偏好引导式教学而非现成代码
- 编码前习惯先规划整体架构
- 递进式提问：先分析思路→再实现细节→最后代码
- 使用简体中文沟通，风格直接简洁
