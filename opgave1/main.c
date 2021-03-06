#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#ifdef __APPLE__
# include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include "time_utils.h"
#include "ocl_utils.h"

#include <opencv2/imgcodecs/imgcodecs_c.h>

#define WIDTH (640 * 10)
#define HEIGHT (480 * 10)

#define MIN_REAL -2.0
#define MAX_REAL 1.0
#define MIN_IMAGINARY -1.1f
#define MAX_IMAGINARY (MIN_IMAGINARY + (MAX_REAL - MIN_REAL) *\
(HEIGHT) / (WIDTH))
//#define MAX_IMAGINARY 1.2
#define MAX_ITERATIONS 200

#define IMAGINARY_POS(y)\
(float)(MAX_IMAGINARY -\
(y) * ((MAX_IMAGINARY - MIN_IMAGINARY) / (float)((HEIGHT) - 1)))

#define REAL_POS(x)\
(float)(MIN_REAL + (x) * ((MAX_REAL - MIN_REAL) / (float)((WIDTH) - 1)))


const int W = (640 * 10);
const int H =  (480 * 10);
const cl_int SIZE = WIDTH*HEIGHT;

const int VECTOR_SIZE = 1024;

cl_mem create_result_buffer(void)
{
    cl_int error;
    float *host_vec = malloc(sizeof(cl_float3) * WIDTH * HEIGHT);
    for (int i = 0; i < WIDTH*HEIGHT; ++i)
        host_vec[i] = 0;

    cl_mem dev_vec = clCreateBuffer(g_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(cl_float3) *  WIDTH*HEIGHT, host_vec, &error);
    ocl_err(error);
    return dev_vec;
}

/*cl_float *add_numbers(void)
{
    cl_int error;
    // Create device buffers.
    cl_mem dev_vec_a = create_and_init_vector();
    cl_mem dev_vec_b = create_and_init_vector();
    cl_mem dev_result = create_result_buffer();
    cl_float *host_result = malloc(sizeof(cl_float3) * VECTOR_SIZE);

    // Create kernel
    cl_kernel kernel = clCreateKernel(g_program, "add_numbers", &error);
    ocl_err(error);

    // Set kernel arguments
    int arg_num = 0;
    ocl_err(clSetKernelArg(kernel, arg_num++, sizeof(cl_mem), &dev_vec_a));
    ocl_err(clSetKernelArg(kernel, arg_num++, sizeof(cl_mem), &dev_vec_b));
    ocl_err(clSetKernelArg(kernel, arg_num++, sizeof(cl_int), &VECTOR_SIZE));
    ocl_err(clSetKernelArg(kernel, arg_num++, sizeof(cl_mem), &dev_result));

    // Call kernel
    size_t global_work_sizes[] = {VECTOR_SIZE};
    time_measure_start("computation");
    ocl_err(clEnqueueNDRangeKernel(g_command_queue, kernel, 1, NULL,
                global_work_sizes, NULL, 0, NULL, NULL));
    ocl_err(clFinish(g_command_queue));
    time_measure_stop_and_print("computation");

    // Read result
    time_measure_start("data_transfer");
    ocl_err(clEnqueueReadBuffer(g_command_queue, dev_result, CL_TRUE,
                0, sizeof(cl_float3) * VECTOR_SIZE, host_result, 0, NULL, NULL));
    time_measure_stop_and_print("data_transfer");
    return host_result;
}*/

void print_result(cl_float *numbers)
{
    for (int i = 0; i < VECTOR_SIZE; ++i)
    {
        printf("%d: %f, ", i, numbers[i]);
    }
    printf("\n");
}

cl_float * render_mandelbrot(CvMat * output_image)
{
	cl_int error;
	// Create device buffers.
    cl_mem dev_result = create_result_buffer();
    cl_float *host_result = malloc(sizeof(cl_float3) * W*H);
    
    // Create kernel
    cl_kernel kernel = clCreateKernel(g_program, "mandelbrot", &error);
    ocl_err(error);
    
    // Set kernel arguments
    int arg_num = 0;
    ocl_err(clSetKernelArg(kernel, arg_num++, sizeof(int), &W));
    ocl_err(clSetKernelArg(kernel, arg_num++, sizeof(int), &H));
    ocl_err(clSetKernelArg(kernel, arg_num++, sizeof(cl_mem), &dev_result));
    
    // Call kernel
    size_t global_work_sizes[] = {WIDTH,HEIGHT};
    time_measure_start("computation");
    ocl_err(clEnqueueNDRangeKernel(g_command_queue, kernel, 1, NULL,
                global_work_sizes, NULL, 0, NULL, NULL));
    ocl_err(clFinish(g_command_queue));
    time_measure_stop_and_print("computation");

    // Read result
    time_measure_start("data_transfer");
    ocl_err(clEnqueueReadBuffer(g_command_queue, dev_result, CL_TRUE,
                0, sizeof(cl_float3) * VECTOR_SIZE, host_result, 0, NULL, NULL));
    time_measure_stop_and_print("data_transfer");
    return host_result;
}

int main(int argc, char *argv[])
{
    CvMat * output_image = cvCreateMat(HEIGHT, WIDTH, CV_32FC1);
 
 
    cl_platform_id platform = ocl_select_platform();
    cl_device_id device = ocl_select_device(platform);
    init_ocl(device);
    create_program("kernel.cl", "");

    time_measure_start("total");
    //cl_float *numbers = add_numbers();
    render_mandelbrot(output_image);   
    time_measure_stop_and_print("total");
    //print_result(numbers);
     cvSaveImage("mandelbrot.png", output_image, 0);
}
