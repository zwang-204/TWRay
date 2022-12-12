#include <iostream>

#include "parallel.h"
#include "stats.h"

using namespace pbrt;

int main(){
    ParallelInit();
    InitProfiler();
    
    return 0;
}