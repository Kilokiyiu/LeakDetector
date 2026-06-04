#include <cstdio>

#include "leak_detector.h"

int main() {
    int* p = new int(42);

    std::printf("value = %d\n", *p);

    delete p;

    ReportLeaks();
    return 0;
}
