# 示例规格说明：`examples/01_simple_leak`

> 版本：v0.1 黄金测试集 #01  
> 依赖头文件：[v0.1-leak-detector-h.md](v0.1-leak-detector-h.md)  
> 目的：验证「单次 `new` 未 `delete`」能否被检测并定位到源文件行号。

---

## 1. 示例概述

| 项 | 值 |
|----|-----|
| 目录 | `LeakDetector/examples/01_simple_leak/` |
| 场景 | 最简单的 definite leak：分配一个 `int`，故意不释放 |
| 预期工具结论 | 发现 **1** 处泄漏，**4** 字节（`sizeof(int)` 典型值） |
| Valgrind 分类 | definitely lost（可选对照，见 §8） |

---

## 2. 目录结构

创建以下文件（仅规格，需自行编写内容）：

```
examples/01_simple_leak/
├── main.cpp                 # 测试程序（必需）
├── CMakeLists.txt           # 构建脚本（推荐）
├── build.bat                # Windows 备用构建（可选）
├── expected_output.txt      # LeakDetector 预期输出（必需）
├── README.md                # 本示例简短说明（可选）
└── valgrind_reference.txt   # Valgrind 对照输出（可选，WSL）
```

---

## 3. `main.cpp` 规格

### 3.1 行为要求

1. **必须**在函数内（建议 `main`）执行一次堆分配
2. **必须**使用 `new`（会被 `leak_detector.h` 宏追踪）
3. **必须故意**不调用 `delete`
4. **必须**在 `return` 之前调用 `ReportLeaks()`
5. **不要**使用 `new[]`、智能指针、`malloc`（本示例范围外）
6. **不要**使用 `LeakDetectorGuard`（本示例要求显式调用 `ReportLeaks`）

### 3.2 推荐代码结构（骨架，行号供 expected_output 参考）

```cpp
// 1. 先 include 标准库
#include <iostream>

// 2. leak_detector.h 必须最后 include
#include "leak_detector.h"

int main() {
    // 第 A 行：故意泄漏 —— expected_output 中的 line 应对准这一行的 new
    int* p = new int(42);

    // 可选：使用 p，避免编译器「未使用变量」警告
    std::cout << "value = " << *p << '\n';

    // 故意不写: delete p;

    ReportLeaks();   // 必须在 return 前调用
    return 0;
}
```

### 3.3 行号约定

- `expected_output.txt` 中的 `[main.cpp:<line>]` **必须**指向 **`new int(42)` 所在行**
- 编写 `expected_output.txt` 时，先定稿 `main.cpp`，再数行号填入
- 若修改 `main.cpp` 增删空行，需同步更新 `expected_output.txt`

### 3.4 编译期注意

- 路径：`#include "../../include/leak_detector.h"` 或通过 CMake `target_include_directories` 添加 `include/`
- 确保 C++17：`set(CMAKE_CXX_STANDARD 17)`

---

## 4. `CMakeLists.txt` 规格

### 4.1 最低要求

```cmake
cmake_minimum_required(VERSION 3.16)
project(simple_leak LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指向 LeakDetector/include
set(LEAK_DETECTOR_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

add_executable(simple_leak main.cpp)
target_include_directories(simple_leak PRIVATE "${LEAK_DETECTOR_INCLUDE}")
```

### 4.2 构建与运行

```bash
cd examples/01_simple_leak
cmake -B build
cmake --build build
./build/simple_leak          # Linux / WSL
# build\Debug\simple_leak.exe  或  build\simple_leak.exe  # Windows
```

### 4.3 Windows 备用 `build.bat`（可选）

若不用 CMake，bat 需至少：

- 调用 MSVC `cl` 或 MinGW `g++`
- `-I../../include` 或等价参数
- 输出 executable 到当前目录或 `build/`

---

## 5. `expected_output.txt` 规格

用于验证 `ReportLeaks()` 输出。由于**指针地址每次运行变化**，验证方式分两层。

### 5.1 完整样例（指针用占位符）

```
[LeakDetector] FOUND 1 LEAK(S):
  LEAK: 4 bytes at <PTR>  [main.cpp:<LINE>]
```

将 `<LINE>` 替换为 `new int(42)` 的实际行号，例如 `12`。

### 5.2 人工验证规则

运行程序后，逐条检查：

| 检查项 | 期望 |
|--------|------|
| 第一行 | 以 `[LeakDetector] FOUND 1 LEAK(S):` 开头 |
| 泄漏条数 | 恰好 1 行 `LEAK:` |
| 大小 | `4 bytes`（32 位/64 位平台 `sizeof(int)` 通常为 4） |
| 指针 | 非空，形如 `0x...` |
| 位置 | `[main.cpp:<LINE>]` 与源码中 `new` 行一致 |
| 不应出现 | `No leaks detected.` |

### 5.3 程序 stdout 完整顺序

若 `main.cpp` 中有 `std::cout << "value = ..."`，完整输出为：

```
value = 42
[LeakDetector] FOUND 1 LEAK(S):
  LEAK: 4 bytes at 0x........  [main.cpp:12]
```

`expected_output.txt` 可以：

- **方案 A**：只包含 `ReportLeaks` 部分（推荐，便于 diff 脚本）
- **方案 B**：包含完整 stdout（需在文档注明）

---

## 6. 验收标准（Definition of Done）

全部满足即视为示例完成：

- [ ] 目录与文件按 §2 创建
- [ ] 编译无 error（warning 可接受）
- [ ] 运行退出码为 0
- [ ] 控制台输出 `FOUND 1 LEAK(S)`
- [ ] 泄漏报告指向正确的 `main.cpp` 行号
- [ ] 泄漏大小为 4 bytes
- [ ] 取消 `new` 行、改为 `new` + `delete p` 后，输出变为 `No leaks detected.`（回归测试）

---

## 7. 调试指南

### 7.1 报 `No leaks detected` 但确实没 delete

| 可能原因 | 排查 |
|---------|------|
| 未 include `leak_detector.h` | 检查 include |
| `leak_detector.h` 不在最后 | 调整顺序 |
| 宏未生效 | 确认 `#define new` 在头文件中 |
| 误写了 `delete p` | 检查源码 |
| `ReportLeaks` 在 `new` 之前调用 | 调整顺序 |

### 7.2 行号不对

- 宏展开后 `__LINE__` 是 **`new` 关键字所在行**，不是 `ReportLeaks` 行
- 若行号偏移，检查是否在 `new` 前有多行宏或续行

### 7.3 编译错误与 STL

- 将 `#include "leak_detector.h"` 移到**所有**其他 include 之后

### 7.4 链接错误 `undefined reference to operator new`

- 确认 `leak_detector.h` 中 operator 定义在头文件内（inline）或被正确链接

---

## 8. Valgrind 对照（可选，WSL）

在 Linux/WSL 中可用 Valgrind 建立「业界基准」认知。  
**注意**：默认 Valgrind **不会**自动追踪宏替换的 `new`，除非也做了 operator 重载或使用 `--leak-check=full` 且走 malloc 路径。  
本示例因 `leak_detector.h` 内部使用 `malloc`，Valgrind 可能看到 heap block，但**行号**来自我们的工具而非 Valgrind。

若编写独立对照程序（不含 leak_detector.h）：

```cpp
int main() {
    int* p = new int(42);
    (void)p;
    return 0;  // 故意泄漏
}
```

```bash
valgrind --leak-check=full --show-leak-kinds=all ./simple_leak_native
```

典型输出片段：

```
Definitely lost: 4 bytes in 1 blocks
```

将完整输出保存为 `valgrind_reference.txt`，用于学习「definitely lost」含义，**不要求与 LeakDetector 输出格式一致**。

---

## 9. 与 v0.2 的衔接

v0.2 将为本示例增加：

- 环境变量 `LEAK_DETECTOR_LOG_PATH=leak_log.jsonl`
- 运行后使用 `dotnet run -- analyze leak_log.jsonl` 得到与控制台等价的报告

届时在本文档同级增加 `expected_events.jsonl` 规格即可。

---

## 10. 完成后建议的下一个示例

| # | 名称 | 目的 |
|---|------|------|
| 02 | `no_leak` | 验证正确 `delete` 时无报告 |
| 03 | `array_leak` | 验证 `new[]` 未 `delete[]` |

详见 [detailed-roadmap.md](../detailed-roadmap.md) Phase 1 §1.2。

---

## 11. 参考

- 头文件规格：[v0.1-leak-detector-h.md](v0.1-leak-detector-h.md)
- 开发路线：[detailed-roadmap.md](../detailed-roadmap.md)
- Valgrind 泄漏分类：[学习笔记.md](../学习笔记.md) §4.2（待补全）
