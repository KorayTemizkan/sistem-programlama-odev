#ifndef TARSAU_H
#define TARSAU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> // mode_t tanımı için bu satırı ekledik
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILES 32                  
#define MAX_TOTAL_SIZE 200 * 1024 * 1024 

typedef struct
{
    char filename[256];
    mode_t permissions;
    long size;
} FileRecord;

#endif