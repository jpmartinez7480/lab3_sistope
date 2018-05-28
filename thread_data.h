#include "bmp.h"

typedef struct{
    bmp_image *image;
    int *row;
}thread_data;