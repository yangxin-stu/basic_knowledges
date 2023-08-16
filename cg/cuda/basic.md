### kernel function and grid, block, thread
```cpp
#include <cstdio>
#include <format>
#include <cuda.h>
#include <cudart_platform.h>
#include <cuda_runtime.h>
#include <cmath>


using namespace std;

__global__ void my_first_kernel() {
	int tidx = threadIdx.x;
	int bidx = blockIdx.x;
	int tidy{ threadIdx.y }, bidy{ blockIdx.y };
	printf("block index:(%d, %d), thread index:(%d, %d)\n", bidx, bidy, tidx, tidy);
}



int main()
{
	cout << format("Hello CMake from CPU.") << endl;

	int grid_size{ 2 }, block_size{ 3 };
	dim3 grid(3, 3), block(3, 3);
	// thread->block->grid
	my_first_kernel<<<grid, block>>>(); // 一个grid里有2个block，每个block3个thread
	
	// 同步cpu和gpu
	cudaDeviceSynchronize();
	return 0;
}

```


### 数据 data
1. memory allocation
2. memory copy (from host to device)
3. device operation (kernel function)
4. copy back to host

#### 向量加法
```cpp
#include <cstdio>
#include <format>
#include <cuda.h>
#include <cudart_platform.h>
#include <cuda_runtime.h>
#include <cmath>

using namespace std;

__global__ void vecAdd(const double* x, const double *y, double*z, const int n) {
	const unsigned int idx{ blockDim.x * blockIdx.x + threadIdx.x };
	if (idx < n) {
		z[idx] = x[idx] + y[idx];
	}
}



int main()
{
	const int N = 1000, M = sizeof(double) * N;

	// cpu memory
	double* h_x = (double*)malloc(M), * h_y = (double*)malloc(M), * h_z = (double*)malloc(M);

	// init arrays
	for (size_t i{ 0 }; i < N; ++i) {
		h_x[i] = 1.0;
		h_y[i] = 2.0;
	}

	// device memory
	double* d_x, * d_y, * d_z;
	cudaMalloc((void**)&d_x, M);
	cudaMalloc((void**)&d_y, M);
	cudaMalloc((void**)&d_z, M);

	// copy (host to device)
	cudaMemcpy(d_x, h_x, M, cudaMemcpyHostToDevice);
	cudaMemcpy(d_y, h_y, M, cudaMemcpyHostToDevice);


	// kernel function
	const int block_size{ 128 }, grid_size{ (N + block_size - 1) / block_size };
	vecAdd << <grid_size, block_size >> > (d_x, d_y, d_z, N);

	// copy device to host
	cudaMemcpy(h_z, d_z, M, cudaMemcpyDeviceToHost);


	// 同步cpu和gpu  复制应该包含了隐式的同步
	cudaDeviceSynchronize();

	// print the result
	for (size_t i{ 0 }; i < N; ++i) {
		printf("%lf\n", h_z[i]);
	}

	free(h_x), free(h_y), free(h_z);
	cudaFree(d_x), cudaFree(d_y), cudaFree(d_z);
	return 0;
}

```