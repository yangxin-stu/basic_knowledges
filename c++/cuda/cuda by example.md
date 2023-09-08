### kernel function and grid, block, thread
1.  we refer to the CPU and the system’s memory as the host and refer to the GPU and its memory as the device. 

2. To remedy that sinking feeling that you’ve invested in nothing more than an expensive collection of trivialities

```cpp
#include <cstdio>
#include <format>
#include <cuda.h>
#include <cudart_platform.h>
#include <cuda_runtime.h>
#include <cmath>


using namespace std;
// kernel function must return void!
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

#### example: print CUDA device properties
```cpp
int count{ 0 };
cudaGetDeviceCount(&count);
for (int i = 0; i < count; ++i) {
	cudaDeviceProp prop;
	cudaGetDeviceProperties(&prop, i);
	printf(" --- General Information for device %d ---\n", i);
	printf("Name: %s\n", prop.name);
	printf("Compute capability: %d.%d\n", prop.major, prop.minor);
	printf("Clock rate: %d\n", prop.clockRate);
	printf("Device copy overlap: ");
	if (prop.deviceOverlap)
		printf("Enabled\n");
	else
		printf("Disabled\n");
	printf("Kernel execition timeout : ");
	if (prop.kernelExecTimeoutEnabled)
		printf("Enabled\n");
	else
		printf("Disabled\n");
	printf(" --- Memory Information for device %d ---\n", i);
	printf("Total global mem: %ld\n", prop.totalGlobalMem);
	printf("Total constant Mem: %ld\n", prop.totalConstMem);
	printf("Max mem pitch: %ld\n", prop.memPitch);
	printf("Texture Alignment: %ld\n", prop.textureAlignment);
	printf(" --- MP Information for device %d ---\n", i);
	printf("Multiprocessor count: %d\n",
		prop.multiProcessorCount);
	printf("Shared mem per mp: %ld\n", prop.sharedMemPerBlock);
	printf("Registers per mp: %d\n", prop.regsPerBlock);
	printf("Threads in warp: %d\n", prop.warpSize);
	printf("Max threads per block: %d\n",
		prop.maxThreadsPerBlock);
	printf("Max thread dimensions: (%d, %d, %d)\n",
		prop.maxThreadsDim[0], prop.maxThreadsDim[1],
		prop.maxThreadsDim[2]);
	printf("Max grid dimensions: (%d, %d, %d)\n",
		prop.maxGridSize[0], prop.maxGridSize[1],
		prop.maxGridSize[2]);
	printf("\n");
}
```

#### example: choose GPU
```cpp
// query GPU property
int dev;
HANDLE_ERROR(cudaGetDevice(&dev));
std::cout << "current ID of CUDA device: " << dev << '\n';

cudaDeviceProp prop2;
memset(&prop2, 0, sizeof(cudaDeviceProp)); // reset to zero.
prop2.major = 1;
prop2.minor = 3;

HANDLE_ERROR(cudaChooseDevice(&dev, &prop2));
HANDLE_ERROR(cudaSetDevice(dev));
```


### 数据 data
1. memory allocation
2. memory copy (from host to device)
3. device operation (kernel function)
4. copy back to host
5. free memory

#### 向量加法
```cpp
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include "../common/book.h"
#include <iostream>


const int n{ 1 << 8 };


// a, b, c 都是device 上的指针！
__global__ void add(int a[], int b[], int c[]) {
	int t = blockDim.x * blockIdx.x + threadIdx.x;
	if (t < n) {
		c[t] = a[t] + b[t];
	}
}

int main() {	
	
	int a[n], b[n], c[n];
	int* dev_a, * dev_b, * dev_c;

	// init a[] and b[]
	for (int i = 0; i < n; ++i) {
		a[i] = -i;
		b[i] = i * i;
	}

	// allocate device memory
	HANDLE_ERROR(cudaMalloc((void**) & dev_a, n * sizeof(int)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_b, n * sizeof(int)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_c, n * sizeof(int)));

	// copy data from cpu to gpu
	HANDLE_ERROR(cudaMemcpy(dev_a, a, n * sizeof(int), cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(dev_b, b, n * sizeof(int), cudaMemcpyHostToDevice));

	// compute the result
	dim3 threadsPerBlock{ 32 };
	dim3 blocksPerGrid{ (n + 31) / 32 };
	add << <blocksPerGrid, threadsPerBlock >> > (dev_a, dev_b, dev_c);

	// copy data from gpu to cpu
	HANDLE_ERROR(cudaMemcpy(c, dev_c, n * sizeof(int), cudaMemcpyDeviceToHost));

	// print the result
	for (int i = 0; i < n; ++i) {
		if (i % 8 == 0 && i) {
			printf("\n");
		}
		printf("c[%d]=%d\t", i, c[i]);
	}

	// free memory
	HANDLE_ERROR(cudaFree(dev_a));
	HANDLE_ERROR(cudaFree(dev_b));
	HANDLE_ERROR(cudaFree(dev_c));
	return 0;
}

```

### example: cpu julia set
```cpp
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include "../common/book.h"
#include "../common/cpu_bitmap.h"
#include <iostream>

const int DIM = 800;

struct cuComplex {
	float r;
	float i;
	cuComplex(float a, float b): r(a), i(b) {}
	// the rule of zero!

	float magnitude2(void) { return r * r + i * i; }
	cuComplex operator*(const cuComplex& rhs) {
		return cuComplex(r * rhs.r - i * rhs.i, i * rhs.r + r * rhs.i);
	}

	cuComplex operator+(const cuComplex& rhs) {
		return cuComplex(r + rhs.r, i + rhs.i);
	}
};

int julia(int x, int y) {
	// translate pixel coordinate to complex space coordinate [-1, 1] * [-1, 1].
	const float scale = 1.5;
	float jx = scale * (float)(DIM / 2 - x) / (DIM / 2);
	float jy = scale * (float)(DIM / 2 - y) / (DIM / 2);

	cuComplex c(-0.8, 0.156);
	cuComplex a(jx, jy);

	for (int i = 0; i < 200; ++i) {
		a = a * a + c;
		if (a.magnitude2() > 1000) {
			return 0;

		}
	}
	return 1;
}

void kernel(unsigned char* pointer) {
	for (int y = 0; y < DIM; ++y) {
		for (int x = 0; x < DIM; ++x) {
			int offset = x + y * DIM;

			int juliaValue = julia(x, y);
			// rgba
			pointer[offset * 4 + 0] = 255 * juliaValue;
			pointer[offset * 4 + 1] = 0;
			pointer[offset * 4 + 2] = 0;
			pointer[offset * 4 + 3] = 255;
		}
	}
}

int main() {	
	CPUBitmap bitmap(DIM, DIM); // RAII?? 
	unsigned char* ptr = bitmap.get_ptr();

	kernel(ptr);
	bitmap.display_and_exit();

	return 0;
}
```


### example: gpu julia set
```cpp
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include "../common/book.h"
#include "../common/cpu_bitmap.h"
#include <iostream>

const int DIM = 1000;

struct cuComplex {
	float r;
	float i;
	__device__ cuComplex(float a, float b): r(a), i(b) {}
	// the rule of zero!

	__device__ float magnitude2(void) { return r * r + i * i; }
	__device__ cuComplex operator*(const cuComplex& rhs) {
		return cuComplex(r * rhs.r - i * rhs.i, i * rhs.r + r * rhs.i);
	}

	__device__ cuComplex operator+(const cuComplex& rhs) {
		return cuComplex(r + rhs.r, i + rhs.i);
	}
};

__device__ int julia(int x, int y) {
	// translate pixel coordinate to complex space coordinate [-1, 1] * [-1, 1].
	const float scale = 1.5;
	float jx = scale * (float)(DIM / 2 - x) / (DIM / 2);
	float jy = scale * (float)(DIM / 2 - y) / (DIM / 2);

	cuComplex c(-0.8, 0.156);
	cuComplex a(jx, jy);

	for (int i = 0; i < 200; ++i) {
		a = a * a + c;
		if (a.magnitude2() > 1000) {
			return 0;

		}
	}
	return 1;
}

__global__ void kernel(unsigned char* pointer) {
	int x = blockIdx.x;
	int y = blockIdx.y;
	int offset = x + y * gridDim.x;

	int juliaValue = julia(x, y);
	// rgba
	pointer[offset * 4 + 0] = 255 * juliaValue;
	pointer[offset * 4 + 1] = 0;
	pointer[offset * 4 + 2] = 0;
	pointer[offset * 4 + 3] = 255;

}

int main() {	
	CPUBitmap bitmap(DIM, DIM); // RAII?? 
	unsigned char* dev_bitmap;

	HANDLE_ERROR(cudaMalloc((void**)&dev_bitmap, bitmap.image_size()));

	dim3 grid(DIM, DIM);
	kernel << <grid, 1 >> > (dev_bitmap);
	HANDLE_ERROR(cudaMemcpy(bitmap.get_ptr(), dev_bitmap, bitmap.image_size(), cudaMemcpyDeviceToHost));

	bitmap.display_and_exit();
	cudaFree(dev_bitmap);

	return 0;
}
```

### thread cooperate
1. what is thread?
2. communication and synchronization
3. the motivation for splitting blocks into threads was simply one of working around hardware limitations to the number of blocks we can have in flight.
4. `__shared__` makes variable resident in shared memory.It creates a copy of the variable for each block that you launch on the GPU. Every thread in that block shares the memory, but threads cannot see or modify the copy of this variable that is seen within other blocks.
5. hreads within a block can communicate and collaborate on computations. Furthermore, shared memory buffers reside physically on the GPU as opposed to residing in off-chip DRAM. low latency.

#### example: ripple
```cpp
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include "../common/book.h"
#include "../common/cpu_bitmap.h"
#include "../common/cpu_anim.h"
#include "../common/gpu_anim.h"
#include <iostream>

const int N = 1 << 10;
const int DIM = 1024;

__global__ void add(int* a, int* b, int* c) {
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	while (tid < N) {
		c[tid] = a[tid] + b[tid];
		tid += gridDim.x * gridDim.x;
	}
}

struct DataBlock {
	unsigned char* dev_bitmap;
	CPUAnimBitmap* bitmap;
};

__global__ void kernel(unsigned char* ptr, int ticks) {
	// map from threadIdx / blockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float fx = x - DIM / 2;
	float fy = y - DIM / 2;
	float d = sqrtf(fx * fx + fy * fy);
	unsigned char grey = (unsigned char)(128.0f + 127.0f *
		cos(d / 10.0f - ticks / 7.0f) /
		(d / 10.0f + 1.0f));
	ptr[offset * 4 + 0] = grey;
	ptr[offset * 4 + 1] = grey;
	ptr[offset * 4 + 2] = grey;
	ptr[offset * 4 + 3] = 255;
}


// host function
__host__ void generate_frame(DataBlock* d, int ticks) {
	dim3 blocks((DIM + 15) / 16, (DIM + 15) / 16);
	dim3 threads(16, 16);
	kernel << <blocks, threads >> > (d->dev_bitmap, ticks);
	HANDLE_ERROR(cudaMemcpy(d->bitmap->get_ptr(), d->dev_bitmap, d->bitmap->image_size(), cudaMemcpyDeviceToHost));
}


void cleanup(DataBlock* d) {
	cudaFree(d->dev_bitmap);
}

int main() {	
	DataBlock data;
	CPUAnimBitmap bitmap(DIM, DIM, &data);
	data.bitmap = &bitmap;
	HANDLE_ERROR(cudaMalloc((void**)&data.dev_bitmap, bitmap.image_size()));
	bitmap.anim_and_exit((void(*)(void*, int))generate_frame, (void (*)(void*))cleanup);


	return 0;
}
```

#### example: dot multiply
1. When some of the threads need to execute an instruction while others don’t, this situation is known as `thread divergence`. Under normal circumstances, divergent branches simply result in some threads remaining idle, while the other threads actually execute the instructions in the branch. espacially in conditional expression. 
2. The CUDA Architecture guarantees that no thread will advance to an instruction beyond the __syncthreads() until `every thread in the block` has executed the `__syncthreads()`.

```cpp
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include "../common/book.h"
#include "../common/cpu_bitmap.h"
#include "../common/cpu_anim.h"
#include "../common/gpu_anim.h"
#include <iostream>



const int N = 33*1024;
const int threadsPerBlock = 256;
const int blocksPerGrid = std::min({ 32, (N+threadsPerBlock-1)/threadsPerBlock});
const int DIM = 1024;


__global__ void dot(float* a, float* b, float* c) {
	__shared__ float cache[threadsPerBlock];
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	int cacheIndex = threadIdx.x;

	float temp = 0;
	while (tid < N) {
		temp += a[tid] * b[tid];
		tid += blockDim.x * gridDim.x;
	}
	cache[cacheIndex] = temp;
	// 同步一个block中的所有线程，确保计算完成
	__syncthreads();
	// for reduction
	int i = blockDim.x / 2;
	while (i != 0) {
		if (cacheIndex < i) {
			cache[cacheIndex] += cache[cacheIndex + i];
		}
		__syncthreads();
		i /= 2;
	}
	if (cacheIndex == 0) {
		c[blockIdx.x] = cache[0];
	}

}


int main() {	
	float* a, * b, c, * partial_c;
	float* dev_a, * dev_b, * dev_partial_c;
	// allocate memory on the CPU side
	a = new float[N];
	b = new float[N];
	partial_c = new float[blocksPerGrid];
	// allocate the memory on the GPU
	HANDLE_ERROR(cudaMalloc((void**)&dev_a, N * sizeof(float)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_b, N * sizeof(float)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_partial_c, blocksPerGrid * sizeof(float)));
	// fill in the host memory with data
	for (int i = 0; i < N; i++) {
		a[i] = i;
		b[i] = i * 2;
	}
	// copy the arrays 'a' and 'b' to the GPU
	HANDLE_ERROR(cudaMemcpy(dev_a, a, N * sizeof(float), cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(dev_b, b, N * sizeof(float), cudaMemcpyHostToDevice));
	dot << <blocksPerGrid, threadsPerBlock >> > (dev_a, dev_b, dev_partial_c);
	// copy the array 'c' back from the GPU to the CPU
	HANDLE_ERROR(cudaMemcpy(partial_c, dev_partial_c, blocksPerGrid * sizeof(float), cudaMemcpyDeviceToHost));

	// finish up on the CPU side
	c = 0;
	for (int i = 0; i < blocksPerGrid; i++) {
		c += partial_c[i];
	}

#define sum_squares(x) (x*(x+1)*(2*x+1)/6)
	printf("Does GPU value % .6g = % .6g ? \n", c, 2 * sum_squares((float)(N - 1)));

	// free memory on the GPU side
	cudaFree(dev_a);
	cudaFree(dev_b);
	cudaFree(dev_partial_c);
	// free memory on the CPU side
	delete[] a;
	delete[] b;
	delete[] partial_c;

	return 0;
}
```


#### example: green blobs

```cpp
#include "cuda.h"
#include "../common/book.h"
#include "../common/cpu_bitmap.h"

#define DIM 1024
#define PI 3.1415926535897932f

__global__ void kernel( unsigned char *ptr ) {
    // map from threadIdx/BlockIdx to pixel position
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;
    int offset = x + y * blockDim.x * gridDim.x;

    __shared__ float    shared[16][16];

    // now calculate the value at that position
    const float period = 128.0f;

    shared[threadIdx.x][threadIdx.y] =
            255 * (sinf(x*2.0f*PI/ period) + 1.0f) *
                  (sinf(y*2.0f*PI/ period) + 1.0f) / 4.0f;

    // removing this syncthreads shows graphically what happens
    // when it doesn't exist.  this is an example of why we need it.
    __syncthreads();

    ptr[offset*4 + 0] = 0;
    ptr[offset*4 + 1] = shared[15-threadIdx.x][15-threadIdx.y];
    ptr[offset*4 + 2] = 0;
    ptr[offset*4 + 3] = 255;
}

// globals needed by the update routine
struct DataBlock {
    unsigned char   *dev_bitmap;
};

int main( void ) {
    DataBlock   data;
    CPUBitmap bitmap( DIM, DIM, &data );
    unsigned char    *dev_bitmap;

    HANDLE_ERROR( cudaMalloc( (void**)&dev_bitmap,
                              bitmap.image_size() ) );
    data.dev_bitmap = dev_bitmap;

    dim3    grids(DIM/16,DIM/16);
    dim3    threads(16,16);
    kernel<<<grids,threads>>>( dev_bitmap );

    HANDLE_ERROR( cudaMemcpy( bitmap.get_ptr(), dev_bitmap,
                              bitmap.image_size(),
                              cudaMemcpyDeviceToHost ) );
                              
    HANDLE_ERROR( cudaFree( dev_bitmap ) );
                              
    bitmap.display_and_exit();
}

```

## const memory and events
1. `constant memory`: data that will not change over the course of a kernel execution. In some situations, using constant memory rather than global memory will reduce the required memory bandwidth
2. A single read from constant memory can be broadcast to other "nearby" threads, effectively saving up to 15 reads.
3. Constant memory is cached, so consecutive reads of the same address will not incur any additional memory traffic.
4. In the CUDA Architecture, a `warp` refers to a collection of 32 threads that are “woven together” and get executed in lockstep. When it comes to handling constant memory, NVIDIA hardware can broadcast a single constant memory read to each half-warp. 
5. Although it can dramatically accelerate performance when all 16 threads are reading the same address, it actually slows performance to a crawl when all 16 threads read different addresses.
6. An `event` in CUDA is essentially a GPU time stamp that is recorded at a user-specified point in time. used in `kernel executions` and `memory copies` involving the device.
7. `cudaEventSynchronize(cudaEvent_t)` 保证在该事件前的所有device端事件已完成

```cpp
#include "cuda.h"
#include "../common/book.h"
#include "../common/cpu_bitmap.h"

#define DIM 1024

#define rnd( x ) (x * rand() / RAND_MAX)
#define INF     2e10f

struct Sphere {
    float   r,b,g;
    float   radius;
    float   x,y,z;
    __device__ float hit( float ox, float oy, float *n ) {
        float dx = ox - x;
        float dy = oy - y;
        if (dx*dx + dy*dy < radius*radius) {
            float dz = sqrtf( radius*radius - dx*dx - dy*dy );
            *n = dz / sqrtf( radius * radius );
            return dz + z;
        }
        return -INF;
    }
};
#define SPHERES 20

__constant__ Sphere s[SPHERES];

__global__ void kernel( unsigned char *ptr ) {
    // map from threadIdx/BlockIdx to pixel position
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;
    int offset = x + y * blockDim.x * gridDim.x;
    float   ox = (x - DIM/2);
    float   oy = (y - DIM/2);

    float   r=0, g=0, b=0;
    float   maxz = -INF;
    for(int i=0; i<SPHERES; i++) {
        float   n;
        float   t = s[i].hit( ox, oy, &n );
        if (t > maxz) {
            float fscale = n;
            r = s[i].r * fscale;
            g = s[i].g * fscale;
            b = s[i].b * fscale;
            maxz = t;
        }
    } 

    ptr[offset*4 + 0] = (int)(r * 255);
    ptr[offset*4 + 1] = (int)(g * 255);
    ptr[offset*4 + 2] = (int)(b * 255);
    ptr[offset*4 + 3] = 255;
}

// globals needed by the update routine
struct DataBlock {
    unsigned char   *dev_bitmap;
};

int main( void ) {
    DataBlock   data;
    // capture the start time
    cudaEvent_t     start, stop;
    HANDLE_ERROR( cudaEventCreate( &start ) );
    HANDLE_ERROR( cudaEventCreate( &stop ) );
    HANDLE_ERROR( cudaEventRecord( start, 0 ) );

    CPUBitmap bitmap( DIM, DIM, &data );
    unsigned char   *dev_bitmap;

    // allocate memory on the GPU for the output bitmap
    HANDLE_ERROR( cudaMalloc( (void**)&dev_bitmap,
                              bitmap.image_size() ) );

    // allocate temp memory, initialize it, copy to constant
    // memory on the GPU, then free our temp memory
    Sphere *temp_s = (Sphere*)malloc( sizeof(Sphere) * SPHERES );
    for (int i=0; i<SPHERES; i++) {
        temp_s[i].r = rnd( 1.0f );
        temp_s[i].g = rnd( 1.0f );
        temp_s[i].b = rnd( 1.0f );
        temp_s[i].x = rnd( 1000.0f ) - 500;
        temp_s[i].y = rnd( 1000.0f ) - 500;
        temp_s[i].z = rnd( 1000.0f ) - 500;
        temp_s[i].radius = rnd( 100.0f ) + 20;
    }
    HANDLE_ERROR( cudaMemcpyToSymbol( s, temp_s, 
                                sizeof(Sphere) * SPHERES) );
    free( temp_s );

    // generate a bitmap from our sphere data
    dim3    grids(DIM/16,DIM/16);
    dim3    threads(16,16);
    kernel<<<grids,threads>>>( dev_bitmap );

    // copy our bitmap back from the GPU for display
    HANDLE_ERROR( cudaMemcpy( bitmap.get_ptr(), dev_bitmap,
                              bitmap.image_size(),
                              cudaMemcpyDeviceToHost ) );

    // get stop time, and display the timing results
    HANDLE_ERROR( cudaEventRecord( stop, 0 ) );
    HANDLE_ERROR( cudaEventSynchronize( stop ) );
    float   elapsedTime;
    HANDLE_ERROR( cudaEventElapsedTime( &elapsedTime,
                                        start, stop ) );
    printf( "Time to generate:  %3.1f ms\n", elapsedTime );

    HANDLE_ERROR( cudaEventDestroy( start ) );
    HANDLE_ERROR( cudaEventDestroy( stop ) );

    HANDLE_ERROR( cudaFree( dev_bitmap ) );

    // display
    bitmap.display_and_exit();
}
```

## texture memory
1. Like `constant memory`, `texture memory` is another variety of read-only memory that can improve performance and reduce memory traffic when reads have certain access patterns. designed for traditional graphics applications; can still be used in computing applications.
2. cached on chips. in some situations it  will provide higher effective bandwidth by reducing memory requests to off-chip DRAM. 
3. texture caches are designed for graphics applications where memory access patterns exhibit a great deal of spatial locality.
4. This is especially noticeable in applications such as our heat transfer simulation: applications that have some spatial coherence to their data access patterns. 
5. Texture memory can provide additional speedups if we utilize some of the conversions that texture samplers can perform automatically, such as unpacking packed data into separate variables or converting 8- and 16-bit integers to normalized floating-point numbers. 

### example: simulating heat transfer
```cpp

```

## atomics
1. NVIDIA refers to the supported features of a GPU as its compute capability.
2. To ensure atomicity of the increment operations `atomicAdd(addr, 1)`, the hardware needs to serialize operations to the same memory location. This can result in a long queue of pending operations, and any performance gain we might have had will vanish. 



## streams
1. Rather than simultaneously computing the same function on lots of data elements as one does with data parallelism, task parallelism involves doing two or more completely different tasks in parallel.
2. In the context of parallelism, a task could be any number of things. For example, an application could be executing two tasks: redrawing its GUI with one thread while downloading an update over the network with another thread. 
3. The C library function `malloc()` allocates standard, pageable host memory, while `cudaHostAlloc()` allocates a buffer of page-locked host memory. Sometimes called pinned memory, page-locked buffers have an important property: The operating system guarantees us that it will never page this memory out to disk, which ensures its residency in physical memory. `cudaHostAllocDefault`
4. `cudaFreeHost()`
5. CUDA streams can play an important role in accelerating your applications. A cudA stream represents a queue of GPU operations that get executed in a specific order. We can add operations such as `kernel launches`, `memory copies`, and `event` starts and stops into a stream. The order in which operations are added to the stream specifies the order in which they will be executed. 
6. A GPU supporting `device overlap` possesses the capacity to simultaneously execute a CUDA C kernel while performing a copy between device and host memory. 
7. NVIDIA GPUs support simultaneous kernel execution and two memory copies. one to the device and one from the device


### other topics
1. Typically, our GPU accesses only GPU memory, and our CPU accesses only host memory. But in some circumstances, it’s better to break these rules.
2. `cudaHostAllocWriteCombined`. This flag indicates that the runtime should allocate the buffer as write-combined with respect to the CPU cache. This flag will not change functionality in our application but represents an important performance enhancement for buffers that will be read only by the GPU. However, write-combined memory can be extremely inefficient in scenarios where the CPU also needs to perform reads from the buffer
3.  we’ve allocated our host memory with the flag `cudaHostAllocMapped`, the buffers can be accessed from the GPU.

