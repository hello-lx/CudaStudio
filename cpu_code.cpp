#include<iostream>
#include<math.h>
#include<sys/time.h>

// 普通加法gpu和cpu对比加速

// blog: https://mp.weixin.qq.com/s/h2XKth1bTujnrxyXTJ2fwg
// 编译： nvcc cpu_code.cu -o cpu_code
// 运行： ./cpu_code



/* 函数实现功能为30M次的for循环，每次循环进行一次加法 */
void add(int n, float *x, float *y)
{
    for(int i=0; i<n; i++)
        y[i] = x[i] + y[i];
}

int main(int argc, char const *argv[])
{
    int N = 1 << 20;
    float *x = new float[N];
    float *y = new float[N];

    for(int i=0; i<N; i++)
    {
        x[i] = 1.0f;
        y[i] = 2.0f;
    }

    struct timeval t1, t2;
    double timeuse;
    gettimeofday(&t1, NULL);
    // Run kernel on 30M elements on the CPU
    add(N, x, y);
    gettimeofday(&t2, NULL);

    timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec) / 1000.0;

    std::cout << "add(int, float*, float*) cpu time: " << timeuse << "ms" << std::endl;

    float maxError = 0.0f;
    for(int i=0; i<N; i++)
        maxError = fmax(maxError, fabs(y[i] - 3.0f));

    std::cout << "Max error: " << maxError << std::endl;

    delete []x;
    delete []y;
    
    return 0;
}

