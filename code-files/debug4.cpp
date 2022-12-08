#include <iostream>
#include <algorithm>
#include <iterator>

#include <numeric>
#include <utility>
#include <functional>
#include <execution>
#include <thread>

void incrementor(int& index)
{
    index = index + 10;
}

template<typename T>
void debug4(T* dummy)
{
    std::for_each(
        std::execution::par_unseq,
        dummy,
        dummy+10,
        &incrementor // In the solution make sure you are still calling this exact function to do the computation.
    );
}

int main()
{
    int* dummy = new int[10];
    debug4(dummy);
    return 0;
}