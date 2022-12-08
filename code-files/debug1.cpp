#include <iostream>
#include <algorithm>
#include <iterator>

#include <numeric>
#include <utility>
#include <functional>
#include <execution>
#include <thread>
#include <array>

#define SIZE 10

template<typename T>
void debug1(std::array<T, SIZE> &A)
{
    std::for_each(
        std::execution::par_unseq,
        A.begin(),
        A.end(),
        [=](int &index)
        {
            index = index + 1;
        }
    );
}

int main()
{
    std::array<int, SIZE> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    debug1(a);
    return 0;
}