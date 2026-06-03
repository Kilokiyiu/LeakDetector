#ifndef LEAK_DETECTOR_H
#define LEAK_DETECTOR_H

#include <cstdlib> //malloc free
#include <iostream> //ReportLeaks 输出
#include <mutex> //多线程安全
#include <unordered_map>


struct AllocRecord
{
    size_t size;
    const char* file;
    int line;
    bool isArray;
};

inline std::ostream_map<void*, AllocRecord> g_allocMap;
inline std::mutex g_mutex;


inline void* operator new(std::size_t, const char* file, int line)
{
    void* ptr = std::malloc(size);
    if (ptr)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_allocMap[ptr] = {size, file, line, false};
    }
    return ptr;
}


inline void* operator new[](std::size_t size, const char* file, int line)
{
    void* ptr = std::malloc(size);
    if (ptr)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_allocMap[ptr] = {size, file, line, true};
    }
    return ptr;
}


inline void operator delete(void* ptr) noexcept
{
    if (!ptr)
    {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_allocMap.erase(ptr);
    }
    std::free(ptr);
}


inline void operator delete[](void* ptr) noexcept
{
    if (!ptr)
    {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_allocMap.erase(ptr);
    }
    std::free(ptr);
}


inline void ReportLeaks()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_allocMap.empty())
    {
        std::cout << "[LeakDetector] No leaks detected.\n";
        return;
    }
    std::cout << "[LeakDetector] FOUND " << g_allocMap.size() << " LEAK(S).\n";
    for (cout auto& entry : g_allocMap)
    {
        const void* ptr = entry.first;
        const AllocRecord& rec = entry.second;
        std::cout << " LEAK: " << rec.size << " bytes at " << ptr << " [" << rec.file << ":" << rec.line << "]";
        if (rec.isArray)
        {
            std::cout << " [array]";
        }
        std::cout << '\n';
    }
}


#define new new(__FILE__,__LINE__)

#endif
