#ifndef TARSAU_H
#define TARSAU_H

// String Kütüphaneleri
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> // İşletim sistemi veri tipleri (mode_t vb.)
#include <sys/stat.h>  // Dosya durumu ve izinleri okumak için (stat fonksiyonu)
#include <unistd.h>    // POSIX işletim sistemi çağrıları (okuma/yazma erişimleri)

// Proje isterlerine göre belirlenen kısıtlar
#define MAX_FILES 32                     // Maksimum giriş dosyası sayısı
#define MAX_TOTAL_SIZE 200 * 1024 * 1024 // Maksimum toplam boyut (200 MB)

// Dosya başlık (header) bilgilerini tutmak için yapı
typedef struct
{
    char filename[256];
    mode_t permissions;
    long size;
} FileRecord;

#endif