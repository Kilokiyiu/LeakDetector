#include "leak_detector.h"

#undef new

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <new>
#include <unordered_map>

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

namespace {

template<typename T>
struct MallocAllocator {
    using value_type = T;

    MallocAllocator() noexcept = default;

    template<typename U>
    MallocAllocator(const MallocAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        if (n == 0) {
            return nullptr;
        }
        void* p = std::malloc(n * sizeof(T));
        if (!p) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t) noexcept {
        std::free(p);
    }
};

using AllocMap = std::unordered_map<
    void*,
    AllocRecord,
    std::hash<void*>,
    std::equal_to<void*>,
    MallocAllocator<std::pair<void* const, AllocRecord>>>;

AllocMap g_allocMap;
std::mutex g_mutex;

enum class OutputMode { Console, Log, Both };

FILE* g_logFile = nullptr;
const char* g_logPath = nullptr;
unsigned g_nextEventId = 1;
bool g_logInitDone = false;

OutputMode get_output_mode() {
    const char* mode = std::getenv("LEAK_DETECTOR_MODE");
    if (!mode || mode[0] == '\0') {
        return OutputMode::Console;
    }
    if (std::strcmp(mode, "log") == 0) {
        return OutputMode::Log;
    }
    if (std::strcmp(mode, "both") == 0) {
        return OutputMode::Both;
    }
    return OutputMode::Console;
}

bool console_report_enabled() {
    const OutputMode mode = get_output_mode();
    return mode == OutputMode::Console || mode == OutputMode::Both;
}

void ensure_log_file_open() {
    if (g_logInitDone) {
        return;
    }
    g_logInitDone = true;

    g_logPath = std::getenv("LEAK_DETECTOR_LOG_PATH");
    if (!g_logPath || g_logPath[0] == '\0') {
        return;
    }

    g_logFile = std::fopen(g_logPath, "wb");
}

void json_write_string(std::FILE* out, const char* s) {
    std::fputc('"', out);
    if (!s) {
        std::fputc('"', out);
        return;
    }
    for (const char* p = s; *p; ++p) {
        if (*p == '\\' || *p == '"') {
            std::fputc('\\', out);
        }
        std::fputc(*p, out);
    }
    std::fputc('"', out);
}

void log_alloc_event(void* ptr, std::size_t size, const char* file, int line, bool isArray) {
    ensure_log_file_open();
    if (!g_logFile) {
        return;
    }

    std::fprintf(
        g_logFile,
        "{\"event\":\"alloc\",\"id\":%u,\"ptr\":\"%p\",\"size\":%zu,\"kind\":\"%s\",\"file\":",
        g_nextEventId++,
        ptr,
        size,
        isArray ? "new[]" : "new");
    json_write_string(g_logFile, file);
    std::fprintf(g_logFile, ",\"line\":%d}\n", line);
    std::fflush(g_logFile);
}

void log_free_event(void* ptr, bool isArray) {
    ensure_log_file_open();
    if (!g_logFile) {
        return;
    }

    std::fprintf(
        g_logFile,
        "{\"event\":\"free\",\"id\":%u,\"ptr\":\"%p\",\"kind\":\"%s\"}\n",
        g_nextEventId++,
        ptr,
        isArray ? "delete[]" : "delete");
    std::fflush(g_logFile);
}

void log_summary_event(std::size_t leakCount, std::size_t totalBytes) {
    ensure_log_file_open();
    if (!g_logFile) {
        return;
    }

    std::fprintf(
        g_logFile,
        "{\"event\":\"summary\",\"id\":%u,\"leak_count\":%zu,\"total_leak_bytes\":%zu}\n",
        g_nextEventId++,
        leakCount,
        totalBytes);
    std::fflush(g_logFile);
}

void* allocate_raw(std::size_t size, const char* file, int line) {
#if defined(_MSC_VER) && defined(_DEBUG)
    return _malloc_dbg(size, _NORMAL_BLOCK, file, line);
#else
    (void)file;
    (void)line;
    return std::malloc(size);
#endif
}

void deallocate_raw(void* ptr) noexcept {
#if defined(_MSC_VER) && defined(_DEBUG)
    _free_dbg(ptr, _NORMAL_BLOCK);
#else
    std::free(ptr);
#endif
}

void deallocate_untracked(void* ptr) noexcept {
#if defined(_MSC_VER) && defined(_DEBUG)
    _free_dbg(ptr, _NORMAL_BLOCK);
#else
    std::free(ptr);
#endif
}

void leak_detector_delete(void* ptr) noexcept {
    if (!ptr) {
        return;
    }

    bool tracked = false;
    bool isArray = false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        const auto it = g_allocMap.find(ptr);
        if (it != g_allocMap.end()) {
            isArray = it->second.isArray;
            log_free_event(ptr, isArray);
            g_allocMap.erase(it);
            tracked = true;
        }
    }

    if (tracked) {
        deallocate_raw(ptr);
    } else {
        deallocate_untracked(ptr);
    }
}

} // namespace

void leak_detector_record_alloc(void* ptr, std::size_t size, const char* file, int line, bool isArray) {
    if (!ptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    g_allocMap[ptr] = {size, file, line, isArray};
    log_alloc_event(ptr, size, file, line, isArray);
}

void leak_detector_record_free(void* ptr) noexcept {
    if (!ptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    g_allocMap.erase(ptr);
}

void* operator new(std::size_t size, const char* file, int line) {
    void* ptr = allocate_raw(size, file, line);
    if (!ptr) {
        throw std::bad_alloc();
    }
    leak_detector_record_alloc(ptr, size, file, line, false);
    return ptr;
}

void* operator new[](std::size_t size, const char* file, int line) {
    void* ptr = allocate_raw(size, file, line);
    if (!ptr) {
        throw std::bad_alloc();
    }
    leak_detector_record_alloc(ptr, size, file, line, true);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    leak_detector_delete(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept {
    leak_detector_delete(ptr);
}

void operator delete(void* ptr, const char*, int) noexcept {
    leak_detector_delete(ptr);
}

void operator delete(void* ptr, std::size_t, const char*, int) noexcept {
    leak_detector_delete(ptr);
}

void operator delete[](void* ptr) noexcept {
    leak_detector_delete(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept {
    leak_detector_delete(ptr);
}

void operator delete[](void* ptr, const char*, int) noexcept {
    leak_detector_delete(ptr);
}

void operator delete[](void* ptr, std::size_t, const char*, int) noexcept {
    leak_detector_delete(ptr);
}

void ReportLeaks() {
    std::lock_guard<std::mutex> lock(g_mutex);

    std::size_t totalBytes = 0;
    for (const auto& entry : g_allocMap) {
        totalBytes += entry.second.size;
    }
    log_summary_event(g_allocMap.size(), totalBytes);

    if (!console_report_enabled()) {
        return;
    }

    if (g_allocMap.empty()) {
        std::printf("[LeakDetector] No leaks detected.\n");
        return;
    }

    std::printf("[LeakDetector] FOUND %zu LEAK(S):\n", g_allocMap.size());
    for (const auto& entry : g_allocMap) {
        const void* ptr = entry.first;
        const AllocRecord& rec = entry.second;
        std::printf("  LEAK: %zu bytes at %p  [%s:%d]", rec.size, ptr, rec.file, rec.line);
        if (rec.isArray) {
            std::printf(" [array]");
        }
        std::printf("\n");
    }
}
