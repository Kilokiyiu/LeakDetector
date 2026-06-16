#include <cstdio>
#include <stdexcept>

#include "leak_detector.h"

int main() 
{
    int* p = new int(42);
    
    try
    {
        std::printf("value = %d\n", *p);
        throw std::runtime_error("simulated error");
        delete p;
    }
    catch (const std::runtime_error& e)
    {
        std::printf("caught: %s\n", e.what());
    }
    
    ReportLeaks();
    return 0;
}
