#include <array>
#include <cstdio>
#include <cstdlib>
#include <thread>

struct WorkerArgs
{
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int *output;
    int threadId;
    int numThreads;
    float* x_pos;
    float* y_pos;
};

static inline int mandel(float c_re, float c_im, int count)
{
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < count; ++i)
  {
    if (z_re * z_re + z_im * z_im > 4.f)
        return i;
    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }
  return i;
}

static inline void mandelbrotRow(
    float x0, float y0, float x1, float y1,
    float* xPos, float* yPos,
    int width, int startRow,
    int maxIterations,
    int output[])
{
  int index = startRow * width;
  for (int i = 0; i < width; ++i)
  {
    output[index] = mandel(xPos[i], yPos[startRow], maxIterations);
    index++;
  }
}

//
// worker_thread_start --
//
// Thread entrypoint.
void worker_thread_start(WorkerArgs *const args)
{

    // TODO FOR PP STUDENTS: Implement the body of the worker
    // thread here. Each thread could make a call to mandelbrot_serial()
    // to compute a part of the output image. For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.
    // Of course, you can copy mandelbrot_serial() to this file and
    // modify it to pursue a better performance.

    const int img_width = args->width;
    const int img_height = args->height;
    const int num_threads = args->numThreads;
    const int tid = args->threadId;

    // Thread i do rows i, i+T, i+2T, ...
    for (int row = tid; row < img_height; row += num_threads) {
        mandelbrotRow(
            args->x0,
            args->y0,
            args->x1,
            args->y1,
            args->x_pos,
            args->y_pos,
            img_width,
            row,
            args->maxIterations,
            args->output
        );
    };
}

//
// mandelbrot_thread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrot_thread(int num_threads,
                       float x0,
                       float y0,
                       float x1,
                       float y1,
                       int width,
                       int height,
                       int max_iterations,
                       int *output)
{
    static constexpr int max_threads = 32;

    if (num_threads > max_threads)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", max_threads);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::array<std::thread, max_threads> workers;
    std::array<WorkerArgs, max_threads> args = {};
    // Precompute coordinate arrays
    const float dx = (x1 - x0) / (float)width;
    const float dy = (y1 - y0) / (float)height;
    float* xPos = new float[width];
    float* yPos = new float[height];
    for (int i = 0; i < width; ++i) xPos[i] = x0 + (float)i * dx;
    for (int j = 0; j < height; ++j) yPos[j] = y0 + (float)j * dy;

    for (int i = 0; i < num_threads; i++)
    {
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = max_iterations;
        args[i].numThreads = num_threads;
        args[i].output = output;
        args[i].threadId = i;
        args[i].x_pos = xPos;
        args[i].y_pos = yPos;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i = 1; i < num_threads; i++)
    {
        workers[i] = std::thread(worker_thread_start, &args[i]);
    }

    worker_thread_start(&args[0]);

    // join worker threads
    for (int i = 1; i < num_threads; i++)
    {
        workers[i].join();
    }
    delete[] xPos;
    delete[] yPos;
}
