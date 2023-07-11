/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**              Ruiyan Ma
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object.
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s\n", filename);
        return NULL;
    }

    // check format
    Image *image = (Image *) malloc(sizeof(Image));
    char format[3];
    fscanf(fp, "%s", format);

    if (strcmp(format, "P3") != 0)
    {
        printf("wrong ppm format.\n");
        return NULL;
    }

    // get row number and column number
    int range;
    fscanf(fp, "%u %u", &image->cols, &image->rows);
    fscanf(fp, "%u", &range);

    if (image->cols < 0 || image->rows < 0 || range != 255)
    {
        printf("wrong ppm format.\n");
        return NULL;
    }

    // get image
    int total = image->rows * image->cols;
    image->image = (Color **) malloc(sizeof(Color *) * total);
    for (int i = 0; i < total; ++i)
    {
        Color* color = (Color *) malloc(sizeof(Color));
        image->image[i] = color;
        fscanf(fp, "%hhu %hhu %hhu", &color->R, &color->G, &color->B);
    }

    fclose(fp);
    return image;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
    printf("P3\n");
    printf("%u %u\n", image->cols, image->rows);
    printf("255\n");

    Color **p = image->image;
    for (int i = 0; i < image->rows; ++i)
    {
        for (int j = 0; j < image->cols; ++j)
        {
            if (j == image->cols - 1)
            {
                printf("%3hhu %3hhu %3hhu\n", (*p)->R, (*p)->G, (*p)->B);
            }
            else
            {
                printf("%3hhu %3hhu %3hhu   ", (*p)->R, (*p)->G, (*p)->B);
            }
            ++p;
        }
    }
}

//Frees an image
void freeImage(Image *image)
{
    int total = image->rows * image->cols;
    for (int i = 0; i < total; ++i)
    {
        Color *p = *(image->image + i);
        free(p);
    }
    free(image->image);
    free(image);
}
