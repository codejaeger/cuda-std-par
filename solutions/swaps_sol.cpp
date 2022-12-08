#include <algorithm>
#include <functional>
#include <array>
#include <thread>
#include <iostream>
#include <execution>

template<typename T>
void swaps(T* A, const std::size_t& n)
{
    for(int index=1; index<int(n/2); index++)
    {
        T* ind1 = (A + index - 1);         // Access the previous element.
        T* ind2 = (A + index);             // Access the current element.
        T* ind3 = (A + (index + (n/2))%n); // Access the n/2th element from this index in a circular fashion.
        T tmp = *ind2;
        *ind2 = *ind1;
        *ind1 = *ind3;
        *ind3 = tmp;
    }
}

template<typename T>
void std_swaps(T* A, const std::size_t& n)
{
    auto* iterator = new std::uintptr_t[n];
    std::generate(iterator, iterator+n, [i = 0] () mutable { return i++; });
    std::for_each(
        // std::execution::par_unseq,
        // std::execution::par,
        // std::execution::unseq,
        std::execution::seq, 
        iterator+1, 
        iterator+int(n/2), 
        [=](std::uintptr_t &index)
        {
            T* ind1 = (A + index - 1);         // Access the previous element.
            T* ind2 = (A + index);             // Access the current element.
            T* ind3 = (A + (index + (n/2))%n); // Access the n/2th element from this index in a circular fashion.
            T tmp = *ind2;
            *ind2 = *ind1;
            *ind1 = *ind3;
            *ind3 = tmp;
        }
    );
}

int main()
{
    int N=8192, *A;
    // int N=819200, *A;
    A = (int*)malloc(sizeof(int)*N);
    for (int i = 0; i < N; ++i) {
        A[i] = i;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    // swaps(A, N);
    std_swaps(A, N); 
    // Parallel performs poor compared to sequential due too many random memory accesses.
    // This holds for multicore and gpu both and even for larger N sizes.
    // Even after commenting out matrix initialisaion code (to reduce unnecessary device to host memory
    // transfers "sequential execution on the GPU" out performs "parallel execution 
    // on the GPU". On CPU, both policies lead to similar execution times due to no concept of device memory.
    // GPU execution always outperforms CPU execution for any policy.
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;
    std::cout<<A[0]<<" "<<A[N-1]<<" "<<std::endl;
}