#ifndef LEAK_DETECTOR_H
#define LEAK_DETECTOR_H

#include <cstddef>
#include <cstdio>

struct AllocRecord {
    std::size_t size;
    const char* file;
    int         line;
    bool        isArray;
};

void leak_detector_record_alloc(void* ptr, std::size_t size, const char* file, int line, bool isArray);
void leak_detector_record_free(void* ptr) noexcept;

void* operator new(std::size_t size, const char* file, int line);
void* operator new[](std::size_t size, const char* file, int line);

void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, std::size_t size) noexcept;
void operator delete(void* ptr, const char* file, int line) noexcept;
void operator delete(void* ptr, std::size_t size, const char* file, int line) noexcept;

void operator delete[](void* ptr) noexcept;
void operator delete[](void* ptr, std::size_t size) noexcept;
void operator delete[](void* ptr, const char* file, int line) noexcept;
void operator delete[](void* ptr, std::size_t size, const char* file, int line) noexcept;

void ReportLeaks();

// leak_detector.cpp must #undef new after including this header.
#define new new(__FILE__, __LINE__)

#endif
