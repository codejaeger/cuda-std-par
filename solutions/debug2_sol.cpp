#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <numeric>
#include <utility>
#include <functional>
#include <execution>
#include <thread>
#include <vector>

template<typename T>
void debug2(T* dummy, T* temp)
{
    std::for_each(
        std::execution::par_unseq,
        dummy,
        dummy+10,
        [&](int &index)
        // [=](int &index)
        {
            index += *temp;
        }
    );
}

int main()
{
    int* temp = new int(0);
    int* dummy = new int[10];
    debug2(dummy, temp);
    std::cout<<"Print"<<std::endl;
    return 0;
}