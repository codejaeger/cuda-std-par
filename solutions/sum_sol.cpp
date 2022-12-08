#include <algorithm>
#include <functional>
#include <numeric>
#include <array>
#include <thread>
#include <iostream>
#include <execution>
#include <mutex>

template<typename T>
void naive_sum(T* A, const std::size_t& sz, int* dummy)
{
    for (int i=0; i<sz; i++)
        *dummy += A[i];
}

template<typename T>
void reduce_sum(T* A, const std::size_t& sz, int* dummy)
{
    std::for_each(
        // std::execution::par_unseq,
        std::execution::par,
        // std::execution::unseq,
        // std::execution::seq,
        A, 
        A+sz, 
        [=](int &ik)
        {
            *dummy += ik;
        }
    );
}

template<typename T>
void std_reduce_sum(T* A, const std::size_t& sz, int* dummy)
{
    *dummy = std::reduce(
        std::execution::par_unseq,
        // std::execution::par,
        // std::execution::unseq,
        // std::execution::seq,
        A, 
        A+sz
    );
}

int main()
{
    // long long i  nt N=8192, *A;
    // int N=8192, *A;
    // int N=819200, *A;
    int N=81920000, *A;
    // A = (int*)malloc(sizeof(long long int)*N);
    A = (int*)malloc(sizeof(int)*N);
    for (int i = 0; i < N; ++i) {
        // A[i * k + j] = rand() % 1024;
        A[i] = 1;
    }
    int* dummy = new int(0);
    
    auto start = std::chrono::high_resolution_clock::now();
    naive_sum(A, N, dummy);
    std::cout<<*dummy<<std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;

    *dummy = 0;
    start = std::chrono::high_resolution_clock::now();
    reduce_sum(A, N, dummy);
    std::cout<<*dummy<<std::endl;
    end = std::chrono::high_resolution_clock::now();
    elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;

    *dummy = 0;
    start = std::chrono::high_resolution_clock::now();
    std_reduce_sum(A, N, dummy);
    std::cout<<*dummy<<std::endl;
    end = std::chrono::high_resolution_clock::now();
    elapsed = end-start;
    std::cout<<elapsed.count() << " ms" <<std::endl;

    // 1. reduce_sum cannot work with par_unseq or par - due to data race problem
    // 2. On multicore no memory transfer access issue (since we do an initialisation on CPU the memory cannot
    //    be created directly on the GPU and unnecessary device to host memory transfer is required). 
    //    On multicore no concept of device memory, hence we see parallel to be fastest , then unseq and seq follow.
    // 3. On gpu if the previous 2 kernels executions are commented and initialisation code is commented to useless prevent memory transfers.
    //    then for any N size we see better performance.
    // 4. Otherwise, only for large N when the memory transfer operation are overshadowed the large compute time,
    //    we see the parallel execution on GPU performing better.


    // reduce_sum cannot work with par_unseq or par
    // on multicore no memory transfer access issue
    // hence we see true numbers parallel fastest then unseq and seq
    // on gpu if the previous 2 kernels executions are commented and initialisation code 
    // is also commented then for any N size we see better performance.
}