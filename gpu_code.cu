#include<iostream>
#include<math.h>
#include<sys/time.h>

// 普通加法gpu和cpu对比加速

// 编译： nvcc gpu_code.cu -o gpu_code
// 运行： ./gpu_code


__global__
void add(int n, float *x, float *y)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for(int i=index; i<n; i+=stride)
        y[i] = x[i] + y[i];
}

int main(int argc, char const *argv[])
{
    int N = 1 << 20;
    float *x;
    float *y;

    // Allocate Unified Memory – accessible from CPU or GPU
    // 内存分配，在GPU或者CPU上统一分配内存
    cudaMallocManaged(&x, N*sizeof(float));
    cudaMallocManaged(&y, N*sizeof(float));

    for(int i=0; i<N; i++)
    {
        x[i] = 1.0f;
        y[i] = 2.0f;
    }

    struct timeval t1, t2;
    double timeuse;
    gettimeofday(&t1, NULL);


    dim3 blockSize(256);
    dim3 gridSize((N + blockSize.x - 1) / blockSize.x);
    add<<<gridSize, blockSize>>>(N, x, y);  // 单线程运行

    // 实例化CUDA event
	cudaEvent_t e_start, e_stop;
	//创建事件
	cudaEventCreate(&e_start);
	cudaEventCreate(&e_stop);
	
	//记录事件，开始计算时间
	cudaEventRecord(e_start, 0);

    // Wait for GPU to finish before accessing on host
    // CPU需要等待cuda上的代码运行完毕，才能对数据进行读取
    // cudaDeviceSynchronize();

    //记录结束时事件
	cudaEventRecord(e_stop, 0);// 0 代表CUDA流0
	//等待事件同步后
	cudaEventSynchronize(e_stop);
	//计算对应的时间，评估代码性能
	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, e_start, e_stop);

    std::cout << "add(int, float*, float*) gpu time: " << elapsedTime << "ms" << std::endl;


    // gettimeofday(&t2, NULL);
    // timeuse = (t2.tv_sec, t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec) / 1000.0;
    // std::cout << "add(int, float*, float*) time: " << timeuse << "ms" << std::endl;

    float maxError = 0.0f;
    for(int i=0; i<N; i++)
        maxError = fmax(maxError, fabs(3.0f - y[i]));
    std::cout << "Max error: " << maxError << std::endl;
    
    // free memory
    cudaFree(x);
    cudaFree(y);

    return 0;
}

