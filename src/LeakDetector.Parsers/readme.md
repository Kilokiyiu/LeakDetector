# LeakDetector.Parsers

**一句话**：翻译官——把 C++ 写出的 JSONL，翻成 Core 事件对象。

---

## 干什么 / 不干什么

| 做 | 不做 |
|----|------|
| 打开 `.jsonl`，一行行读 | 判断有没有泄漏 |
| 按 `event` 映射为 `AllocEvent` / `FreeEvent` / `SummaryEvent` | 打印报告 |
| 空行跳过 | 处理命令行 |

边界：只做「日志 → 对象」；是否泄漏由 Core 判定。

---

## 主要类型

### JsonlEventParser

| 方法 | 作用 |
|------|------|
| `ParseFile(string path)` | 读文件 → `IReadOnlyList<MemoryEvent>` |
| `ParseLine(string line)`（私有） | 解析单行 JSON |

流程：

1. 文件不存在 → `FileNotFoundException`  
2. `File.ReadLines` 逐行读  
3. 空白行跳过  
4. `JsonDocument.Parse` 读字段  
5. 按 `event` 构造对应记录

---

## JSONL 字段契约

每一行一个 JSON 对象。

### alloc

```json
{"event":"alloc","id":1,"ptr":"0x...","size":4,"kind":"new","file":"main.cpp","line":7}
```

| JSON 字段 | 落到 |
|-----------|------|
| `id` | `AllocEvent.Id` |
| `ptr` | `Ptr`（字符串） |
| `size` | `Size`（`ulong`） |
| `kind` | `Kind`（`"new"` / `"new[]"`） |
| `file` / `line` | `File` / `Line` |

### free

```json
{"event":"free","id":2,"ptr":"0x...","kind":"delete"}
```

### summary

```json
{"event":"summary","id":3,"leak_count":0,"total_leak_bytes":0}
```

未知 `event` → `JsonException`。  
缺字段 → `GetProperty` 抛异常。

---

## 样例：testdata/sample_events.jsonl

解析结果为 2 个对象：`AllocEvent`、`SummaryEvent`。  
此处只有事件列表，不含泄漏结论。

---

## 依赖

→ `LeakDetector.Core`（仅 Model）

---

## 在管线中的位置

```
.jsonl
  │
  ▼
JsonlEventParser.ParseFile
  │
  ▼
List<MemoryEvent>
```
