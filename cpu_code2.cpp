#include<iostream>
#include<math.h>
#include<sys/time.h>

// 矩阵加法gpu和cpu对比加速

// blog: https://zhuanlan.zhihu.com/p/34587739
// 编译： nvcc cpu_code.cu -o cpu_code
// 运行： ./cpu_code

struct Matrix
{
    int width;
    int height;
    float *elements;
};

// 获取矩阵A的(row, col)元素
float getElement(Matrix *A, int row, int col)
{
	return A->elements[row * A->width + col];
}

void setElement(Matrix *A, int row, int col, float value)
{
	A->elements[row * A->width + col] = value;
}


/* 函数实现功能为30M次的for循环，每次循环进行一次加法 */
void add(int n, float *x, float *y)
{
    for(int i=0; i<n; i++)
        y[i] = x[i] + y[i];
}

void matMulKernel(Matrix *A, Matrix *B, Matrix *C)
{
    for (int row=0; row<A->height; row++)
    {
        for (int col=0; col < A->width; col++)
        {
        	float Cvalue = 0.0;
            for(int k=0; k<A->width; k++)
                Cvalue += getElement(A, row, k) * getElement(B, k, col);
        	setElement(C, row, col, Cvalue);
            // std::cout << C->elements[row * A->width + col] << std::endl;
        }
    }
}

int main(int argc, char const *argv[])
{
    int width = 1 << 10;
    int height = 1 << 10;
    int N = width * height;
    Matrix *A = new Matrix;
    A->width = width;
    A->height = height;
    Matrix *B = new Matrix;
    B->width = width;
    B->height = height;
    Matrix *C = new Matrix;
    C->width = width;
    C->height = height;

    A->elements = new float[N];
    B->elements = new float[N];
    C->elements = new float[N];
    
    for(int i=0; i<N; i++)
    {
        A->elements[i] = 1.0f;
        B->elements[i] = 2.0f;
        C->elements[i] = 0.0f;
    }

    struct timeval t1, t2;
    double timeuse;
    gettimeofday(&t1, NULL);

    // Run kernel on 30M elements on the CPU
    matMulKernel(A, B, C);

    gettimeofday(&t2, NULL);

    timeuse = (t2.tv_sec, t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec) / 1000.0;

    std::cout << "cpu cost time: " << timeuse << "ms" << std::endl;

    float maxError = 0.0f;
    for(int i=0; i<N; i++)
        maxError = fmax(maxError, fabs(C->elements[i] - 2*width));

    std::cout << "Max error: " << maxError << std::endl;

    free(A);
    free(B);
    free(C);
    
    return 0;
}

