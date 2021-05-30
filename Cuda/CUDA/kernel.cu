#include "kernel.cuh"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include<iostream>
#include <stdio.h>
#include<sstream>
#include<string>
#include <time.h>
using namespace std;
cudaError_t addWithCuda(int* c, const int* a, const int* b, unsigned int size);

__global__ void addKernel(int* c, const int* a, const int* b)
{
	int i = threadIdx.x;
	c[i] = a[i] + b[i];
}

__global__ void GateKernel(int* NewNodeStatus, const int* NodeStatus, const int* GateType)
{
	int i = threadIdx.x;
	if (GateType[i] == 0) {
		//or
		NewNodeStatus[i] = (NodeStatus[i] & 1) | (NodeStatus[i] & 2 >> 1);
	}
	else if (GateType[i] == 1) {
		//and
		NewNodeStatus[i] = (NodeStatus[i] & 1) & (NodeStatus[i] & 2 >> 1);
	}
	else if (GateType[i] == 2) {
		//not
		NewNodeStatus[i] = ~(NodeStatus[i] & 1);
	}
}

__global__ void AndGateKernel(int* NewNodeStatus, const int* NodeStatus)
{
	int i = blockIdx.x*blockDim.x+threadIdx.x;
	NewNodeStatus[i] = (NodeStatus[i] & 1) & (NodeStatus[i] & 2 >> 1);
}
__global__ void OrGateKernel(int* NewNodeStatus, const int* NodeStatus)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	NewNodeStatus[i] = (NodeStatus[i] & 1) | (NodeStatus[i] & 2 >> 1);
}
__global__ void NotGateKernel(int* NewNodeStatus, const int* NodeStatus)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	NewNodeStatus[i] = ~(NodeStatus[i] & 1);
}

void initCuda() {
	int dev = 0;
	cudaDeviceProp devProp;
	cudaGetDeviceProperties(&devProp, dev);


}
int baseblock = 1024;
cudaError_t GateWithCuda(int* OldStatus, int* NewStatus,int arraySize,int GateType,int* dev_Old,int* dev_New) {
	cudaError_t cudaStatus;
	cudaStatus = cudaSetDevice(0);
	if (GateType == 0) {
		AndGateKernel <<< arraySize/ baseblock, baseblock >>> (dev_New, dev_Old);
	}
	else if (GateType == 1) {
		OrGateKernel <<< arraySize / baseblock, baseblock >>> (dev_New, dev_Old);
	}
	else if (GateType == 2) {
		NotGateKernel <<< arraySize / baseblock, baseblock >>> (dev_New, dev_Old);
	}
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		cout <<(int) dev_New << endl;
		cout <<(int)dev_Old << endl;
		fprintf(stderr, "GateWithCuda launch failed: %s\n", cudaGetErrorString(cudaStatus));
		return cudaStatus;
	}

	return cudaStatus;

}
const char* doPreformanceTest(int* AndOldStatus, int* AndNewStatus,
	int* OrOldStatus, int* OrNewStatus,
	int* NotOldStatus, int* NotNewStatus, int arraySize, int testTimes) {
	int* Old[3] = { AndOldStatus,OrOldStatus,NotOldStatus };
	int* New[3] = { AndNewStatus,OrNewStatus,NotNewStatus };
	
	
	
	
	int** devon= new int* [6];
	for (int a = 0; a <= 2; ++a) {
		devon[a*2] = 0;
		devon[a*2+1] = 0;
		cudaMalloc((void**)&devon[a * 2], arraySize * sizeof(int));
		cudaMalloc((void**)&devon[a * 2 + 1], arraySize * sizeof(int));
		cudaError_t cudaStatus;
		cudaStatus=cudaMemcpy(devon[a * 2], Old[a], arraySize * sizeof(int), cudaMemcpyHostToDevice);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMemcpy failed: %s\n", cudaGetErrorString(cudaStatus));
		}
	}
	clock_t start, end;
	start = clock();

	for (int a = 0; a < testTimes; ++a) {
		//async kernel
		for (int gate = 0; gate <= 2; ++gate) {
			GateWithCuda(Old[gate], New[gate], arraySize, gate, devon[gate * 2], devon[gate * 2 + 1]);
		}
		//fprintf(stderr, "完成第%d轮 \n", a);
		for (int gate = 0; gate <= 2; ++gate) {
			cudaMemcpy(New[gate], devon[gate *2+1], arraySize * sizeof(int), cudaMemcpyDeviceToHost);
			//write back
			memcpy(Old[gate], New[gate], arraySize * sizeof(int));
			//swap old and new
			swap(devon[gate * 2], devon[gate * 2 + 1]);

		}
	}

	end = clock();
	printf("time=%f\n", (double)(end - start)*1.0 / CLK_TCK*1000);
	for (int a = 0; a <= 2; ++a) {
		cudaFree(devon[a * 2]);
		cudaFree(devon[a * 2 + 1]);
	}
	
	{
		cudaError_t cudaStatus;
		cudaStatus = cudaDeviceReset();
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaDeviceReset failed! %s", cudaGetErrorString(cudaStatus));
		}
	}
	
	return "";
}

const char* doAdd() {
	const int arraySize = 5;
	const int a[arraySize] = { 1, 2, 3, 4, 5 };
	const int b[arraySize] = { 10, 20, 30, 40, 50 };
	int c[arraySize] = { 0 };
	string sout = "";
	// Add vectors in parallel.
	cudaError_t cudaStatus = addWithCuda(c, a, b, arraySize);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addWithCuda failed!");
		return sout.c_str();
	}
	char file_path_buffer[100];
	sprintf(file_path_buffer, "{1,2,3,4,5} + {10,20,30,40,50} = {%d,%d,%d,%d,%d}\n",
		c[0], c[1], c[2], c[3], c[4]);
	sout += file_path_buffer;
	// cudaDeviceReset must be called before exiting in order for profiling and
	// tracing tools such as Nsight and Visual Profiler to show complete traces.
	cudaStatus = cudaDeviceReset();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceReset failed!");
		return sout.c_str();
	}
	char* temp = new char[sout.length() + 2];
	sprintf(temp, "%s", sout.c_str());
	return temp;
}
int main()
{


	initCuda();
	int** xx = new int* [6];
	int arraySize = 0;
	std::cout << "start!\n" << endl;
	cin >> arraySize;
	arraySize *= baseblock;
	
	for (int a = 0; a <= 5; ++a) {
		xx[a] = new int[arraySize];
		for (int z = 0; z < arraySize;++z) {
			xx[a][z] = rand();
		}
	}
	long long anss = 0;
	const char* ss = doPreformanceTest(xx[0], xx[1], xx[2], xx[3], xx[4], xx[5], arraySize, 100);
	for (int a = 1; a <= 5; a+=2) {
		for (int z = 0; z < arraySize; ++z) {
			anss += xx[a][z];
		}
	}
	
	cout << "ans is " << anss << " !" << endl;
	std::cout << "Hello World!\n" << ss;
	for (int a = 0; a <= 5; ++a) {
		delete xx[a];
	}


	return 0;
}

// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCuda(int* c, const int* a, const int* b, unsigned int size)
{
	int* dev_a = 0;
	int* dev_b = 0;
	int* dev_c = 0;
	cudaError_t cudaStatus;

	// Choose which GPU to run on, change this on a multi-GPU system.
	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	// Allocate GPU buffers for three vectors (two input, one output)    .
	cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	// Copy input vectors from host memory to GPU buffers.
	cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Launch a kernel on the GPU with one thread for each element.
	addKernel <<<1, size >>> (dev_c, dev_a, dev_b);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	// Copy output vector from GPU buffer to host memory.
	cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(dev_c);
	cudaFree(dev_a);
	cudaFree(dev_b);

	return cudaStatus;
}
