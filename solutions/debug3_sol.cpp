#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <numeric>
#include <utility>
#include <functional>
#include <execution>
#include <thread>

template<typename T>
void debug3(T* dummy)
{
    int n=10;
    std::for_each(
        std::execution::par_unseq,
        dummy,
        dummy+10,
        [&](int &index)
        // [=](int &index)
        {
            index += n;
        }
    );
}

int main()
{
    int* dummy = new int[10];
    debug3(dummy);
    std::cout<<"Print"<<std::endl;
    return 0;
}
