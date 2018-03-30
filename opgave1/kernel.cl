__kernel void mandelbrot(int width, int height, __global float *result)
{
    const int gid = get_global_id(1);//tussen de haakjes is de dimentie 1-->2d

	return;

    /*if (gid >= size)
        return;

    result[gid] = vector_a[gid] + vector_b[gid];*/
}

