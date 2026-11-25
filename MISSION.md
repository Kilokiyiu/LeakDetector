# Mission: LeakDetector → 系统内存能力

## Why
近期要把现有 LeakDetector 真正吃透（能讲清、能跑通），作为扎实底座；更长期要学着做成可在 Windows / Linux 上用的**系统内存观测 +（可选）进程泄漏检测**能力，服务系统管理场景。

## Success looks like
- **近期（复盘）**
  - 能演示：C++ 写 JSONL → C# 解析/分析 → 报告泄漏
  - 能分清 Core / Parsers / Analyzers / Reporters / CLI 的边界
  - 能讲清档案卡思路与 `#define new` 拦截
- **后期（演进）**
  - 能区分两层：**进程插桩泄漏检测** vs **主机/进程内存指标观测**
  - 能在 Win/Linux 上采集关键内存指标并做简单告警/报告（不要求一上来精确到每次 `new` 的 file:line）
  - 采集层可插拔；分析/报告/CLI 外壳尽量复用

## Constraints
- 先稳 v0.2 教学管线，再开系统观测；不把两套采集硬揉成一种 API
- 材料继续偏通俗；系统 API 以各平台官方文档为准
- 「疑似泄漏（曲线推断）」和「确定泄漏（有账本）」用语要分开，避免运维误判

## Out of scope
- 近期复盘不展开：整机 Agent、分布式集群、完整 APM
- 不把「反射扫内存」当主方案
- Valgrind/Memcheck 全内部实现仅作对照，不作为移植目标
