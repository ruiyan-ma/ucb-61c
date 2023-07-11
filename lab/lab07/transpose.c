#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src)
{
    for (int x = 0; x < n; x++)
    {
        for (int y = 0; y < n; y++)
        {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

int min(int a, int b)
{
    return a < b ? a : b;
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src)
{
    for (int xStart = 0; xStart < n; xStart += blocksize)
    {
        int xEnd = min(n, xStart + blocksize);
        for (int yStart = 0; yStart < n; yStart += blocksize)
        {
            int yEnd = min(n, yStart + blocksize);
            for (int x = xStart; x < xEnd; ++x)
            {
                for (int y = yStart; y < yEnd; ++y)
                {
                    dst[x * n + y] = src[y * n + x];
                }
            }
        }
    }
}
