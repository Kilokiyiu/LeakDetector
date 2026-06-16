#include <cstdio>

#include "leak_detector.h"

int main() {
    int* a = new int(1);     // 泄漏 1
    int* b = new int(2);     // 泄漏 2
    int* c = new int(3);     // 泄漏 3

    std::printf("sum = %d\n", *a + *b + *c);

    // 故意都不 delete

    ReportLeaks();
    return 0;
}