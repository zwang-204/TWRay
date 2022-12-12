#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <cassert>

template <typename T>
inline void Check_Eq(T a, T b){
    if (a!=b)
        std::cerr<< "NOT EQUAL" << std::endl;
}

#endif