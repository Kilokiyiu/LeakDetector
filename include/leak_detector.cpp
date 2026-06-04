#include "leak_detector.h"

#undef new

#include <cstdlib>
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
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        const auto it = g_allocMap.find(ptr);
        if (it != g_allocMap.end()) {
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
