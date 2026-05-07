// Yapı tanımları ve kütüphaneler

#ifndef TARSAU_H
#define TARSAU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILES 32                     // Ödevde belirtilen sınır [cite: 19]
#define MAX_TOTAL_SIZE 200 * 1024 * 1024 // 200 MB sınırı [cite: 18]

// Her bir dosyanın bilgisini tutacak yapı [cite: 36]
typedef struct
{
    char filename[256];
    mode_t permissions;
    long size;
} FileRecord;

#endif