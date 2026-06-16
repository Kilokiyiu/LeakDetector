#include <cstdio>

#include "leak_detector.h"

int main() 
{
    int* arr = new int[10];

    arr[0] = 42;
    std::printf("value = %d\n", arr[0]);
    
    // delete[] arr;
    
    ReportLeaks();
    return 0;
}
