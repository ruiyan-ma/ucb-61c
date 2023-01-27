/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**              Ruiyan Ma
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

int R = 0, G = 1, B = 2;

int dir[] = {-1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1};

int wrap(int index, int bound)
{
    return (index + bound) % bound;
}

int *isAlive(Image *image, int row, int col)
{
    int *res = (int *) calloc(3, sizeof(int));
    Color *color = *(image->image + row * image->cols + col);

    if (color->R == 255)
    {
        res[R] = 1;
    }

    if (color->G == 255)
    {
        res[G] = 1;
    }

    if (color->B == 255)
    {
        res[B] = 1;
    }

    return res;
}

// Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
// Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat
// the top row as adjacent to the bottom row and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
    Color *next = (Color *) calloc(1, sizeof(Color));
    int* shift;

    // check whether (row, col) is alive or not
    shift = isAlive(image, row, col);
    for (int i = 0; i < 3; ++i)
    {
        shift[i] = shift[i] * 9;
    }

    // check neighbors
    for (int i = 0; i < 8; i += 2)
    {
        int x = wrap(row + dir[i], image->rows);
        int y = wrap(col + dir[i + 1], image->cols);
        int *alive = isAlive(image, x, y);
        for (int i = 0; i < 3; ++i)
        {
            shift[i] += alive[i];
        }
        free(alive);
    }

    if (rule & (1 << shift[R]))
    {
        next->R = 255;
    }

    if (rule & (1 << shift[G]))
    {
        next->G = 255;
    }

    if (rule & (1 << shift[B]))
    {
        next->B = 255;
    }

    free(shift);
    return next;
}

// The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
// You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
{
    Image *next = (Image *) malloc(sizeof(Image));
    int rows = next->rows = image->rows;
    int cols = next->cols = image->cols;
    next->image = (Color **) malloc(sizeof(Color *) * rows * cols);

    Color **p = next->image;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            *p = evaluateOneCell(image, i, j, rule);
            ++p;
        }
    }

    return next;
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("usage: ./gameOfLife filename rule\n");
        printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
        printf("rule is a hex number beginning with 0x; Life is 0x1808.");
        return -1;
    }

    Image *image = readData(argv[1]);
    uint32_t rule = strtol(argv[2], NULL, 16);

    Image *next = life(image, rule);
    writeData(next);

    freeImage(image);
    freeImage(next);
    return 0;
}
