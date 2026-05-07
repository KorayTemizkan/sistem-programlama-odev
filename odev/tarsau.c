// Ana mantık ve kodlar

#include "tarsau.h"

int is_ascii(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;

    int c;
    while ((c = fgetc(f)) != EOF)
    {
        // ASCII karakterler 0-127 arasındadır
        if (c < 0 || c > 127)
        {
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Kullanım: ./tarsau -b [dosyalar] -o [cikti.sau] veya ./tarsau -a [arsiv.sau] [dizin]\n");
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0)
    {
        char *output_name = "a.sau"; // Varsayılan isim [cite: 17]
        int file_count = 0;
        char *input_files[32];

        // Argümanları ayıkla (-o parametresini ve dosya listesini bul)
        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], "-o") == 0)
            {
                if (i + 1 < argc)
                    output_name = argv[++i];
            }
            else
            {
                if (file_count < 32)
                    input_files[file_count++] = argv[i];
            }
        }

        // Kontroller
        if (file_count == 0)
        {
            printf("Hata: En az bir giriş dosyası belirtilmelidir!\n");
            return 1;
        }

        // Dosyaları ASCII kontrolünden geçir ve header hazırla [cite: 14, 20]
        char header[10000] = ""; // Başlık bilgisini biriktireceğimiz alan
        for (int i = 0; i < file_count; i++)
        {
            if (!is_ascii(input_files[i]))
            {
                printf("%s giriş dosyasının formatı uyumsuzdur!\n", input_files[i]);
                return 0; // Program sorunsuz bir şekilde sonlandırılır
            }

            struct stat st;
            stat(input_files[i], &st);

            // Format: |dosya_adı,izinler,boyut| [cite: 36]
            char entry[512];
            sprintf(entry, "|%s,%04o,%ld|", input_files[i], st.st_mode & 0777, st.st_size);
            strcat(header, entry);
        }

        // Header boyutunu en başa 10 bayt olarak yaz [cite: 34]
        FILE *out = fopen(output_name, "w");
        fprintf(out, "%010ld", strlen(header) + 10);
        fprintf(out, "%s", header);

        // Dosya içeriklerini ekle [cite: 39]
        for (int i = 0; i < file_count; i++)
        {
            FILE *in = fopen(input_files[i], "r");
            int c;
            while ((c = fgetc(in)) != EOF)
                fputc(c, out);
            fclose(in);
        }
        fclose(out);
        printf("Dosyalar birleştirildi: %s\n", output_name);
    }

    else if (strcmp(argv[1], "-a") == 0)
    {
        if (argc < 3)
        {
            printf("Hata: Arşiv dosyası belirtilmedi!\n");
            return 1;
        }

        char *archive_name = argv[2];
        char *target_dir = (argc > 3) ? argv[3] : "."; // Dizin yoksa mevcut dizin [cite: 26]

        FILE *src = fopen(archive_name, "r");
        if (!src)
        {
            printf("Arşiv dosyası uygunsuz veya bozuk!\n"); // [cite: 25]
            return 0;
        }

        // 1. Başlık boyutunu oku (İlk 10 bayt)
        char size_buf[11] = {0};
        if (fread(size_buf, 1, 10, src) < 10)
        {
            printf("Arşiv dosyası uygunsuz veya bozuk!\n");
            fclose(src);
            return 0;
        }
        long header_size = atol(size_buf);

        // 2. Başlığı oku [cite: 33]
        char *header_data = malloc(header_size - 10 + 1);
        fread(header_data, 1, header_size - 10, src);
        header_data[header_size - 10] = '\0';

        // Hedef dizin kontrolü ve oluşturma
        if (strcmp(target_dir, ".") != 0)
        {
            mkdir(target_dir, 0777);
        }

        // 3. Dosyaları geri oluştur [cite: 32]
        char *token = strtok(header_data, "|");
        while (token != NULL)
        {
            char fname[256];
            unsigned int fmode;
            long fsize;

            // Formatı parçala: isim, izin, boyut [cite: 36]
            if (sscanf(token, "%[^,],%o,%ld", fname, &fmode, &fsize) == 3)
            {
                char path[512];
                sprintf(path, "%s/%s", target_dir, fname);

                FILE *dest = fopen(path, "w");
                if (dest)
                {
                    // Dosya içeriğini ASCII formatında oku ve yaz [cite: 39]
                    for (long i = 0; i < fsize; i++)
                    {
                        int c = fgetc(src);
                        if (c != EOF)
                            fputc(c, dest);
                    }
                    fclose(dest);
                    // Orijinal izinleri uygula
                    chmod(path, fmode);
                }
            }
            token = strtok(NULL, "|");
        }

        free(header_data);
        fclose(src);
        printf("Arşiv başarıyla açıldı.\n");
    }

    else
    {
        printf("Hatalı parametre!\n");
    }

    return 0;
}