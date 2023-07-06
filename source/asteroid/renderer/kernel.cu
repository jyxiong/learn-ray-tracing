#include "cuda_runtime.h"
#include "device_launch_parameters.h"

// clamp x to range [a, b]
__device__ float clamp(float x, float a, float b) { return max(a, min(b, x)); }

__device__ int clamp(int x, int a, int b) { return max(a, min(b, x)); }

// convert floating point rgb color to 8-bit integer
__device__ int rgbToInt(float r, float g, float b) {
    r = clamp(r, 0.0f, 255.0f);
    g = clamp(g, 0.0f, 255.0f);
    b = clamp(b, 0.0f, 255.0f);
    return (int(b) << 16) | (int(g) << 8) | int(r);
}

__global__ void cudaProcess(uchar4* g_odata, int imgw) {
    extern __shared__ uchar4 sdata[];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bw = blockDim.x;
    int bh = blockDim.y;
    int x = blockIdx.x * bw + tx;
    int y = blockIdx.y * bh + ty;

    uchar4 c4 = make_uchar4((x & 0x20) ? 100 : 0, 0, (y & 0x20) ? 100 : 0, 0);
    g_odata[y * imgw + x] = c4;// rgbToInt(c4.z, c4.y, c4.x);
}

extern "C" void launch_cudaProcess(dim3 grid, dim3 block, int sbytes,
    uchar4* g_odata, int imgw) {
    cudaProcess << <grid, block, sbytes >> > (g_odata, imgw);
}