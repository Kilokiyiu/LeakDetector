# v0.2 四个已知问题已修齐

CLI 子命令与 Usage 统一为 `analyze`；退出码改为无泄漏 0 / 有泄漏 1 / 用法错 3；Reporter 补上插值 `$`；解析测试路径改为五层 `..`。`dotnet test` 4 通过。

**Implications**: 复盘验收基本达成；下一会话默认走开发路线 Slice A（给 CLI/Reporter 补测试锁契约），不要跳去系统观测。
