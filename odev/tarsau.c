#include "tarsau.h"

// Dosyanın ASCII olup olmadığını kontrol eder [cite: 12]
int is_ascii(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;

    int c;
    while ((c = fgetc(f)) != EOF)
    {
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

    // --- BİRLEŞTİRME MODU (-b) --- [cite: 11]
    if (strcmp(argv[1], "-b") == 0)
    {
        char *output_name = "a.sau"; // Varsayılan isim [cite: 17]
        int file_count = 0;
        char *input_files[MAX_FILES];
        long total_size = 0;

        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], "-o") == 0)
            {
                if (i + 1 < argc)
                    output_name = argv[++i];
            }
            else
            {
                if (file_count < MAX_FILES)
                    input_files[file_count++] = argv[i];
            }
        }

        if (file_count == 0)
        {
            printf("Hata: En az bir giriş dosyası belirtilmelidir!\n");
            return 1;
        }

        char header[10000] = "";
        for (int i = 0; i < file_count; i++)
        {
            // ASCII Kontrolü [cite: 20]
            if (!is_ascii(input_files[i]))
            {
                printf("%s giriş dosyasının formatı uyumsuzdur!\n", input_files[i]);
                return 0;
            }

            struct stat st;
            if (stat(input_files[i], &st) != 0)
                return 1;

            // Toplam Boyut Kontrolü [cite: 18]
            total_size += st.st_size;
            if (total_size > MAX_TOTAL_SIZE)
            {
                printf("Hata: Giriş dosyalarının toplam boyutu 200 MB'ı geçemez!\n");
                return 1;
            }

            // Header Kaydı Oluşturma [cite: 36]
            char entry[512];
            sprintf(entry, "|%s,%04o,%ld|", input_files[i], (unsigned int)(st.st_mode & 0777), (long)st.st_size);
            strcat(header, entry);
        }

        FILE *out = fopen(output_name, "w");
        if (!out)
            return 1;

        // İlk 10 bayt header boyutu [cite: 34]
        fprintf(out, "%010ld", (long)(strlen(header) + 10));
        fprintf(out, "%s", header);

        // İçerikleri ekle [cite: 39]
        for (int i = 0; i < file_count; i++)
        {
            FILE *in = fopen(input_files[i], "r");
            int c;
            while ((c = fgetc(in)) != EOF)
                fputc(c, out);
            fclose(in);
        }
        fclose(out);
        printf("Dosyalar birleştirildi.\n");
    }

    // --- AÇMA MODU (-a) --- [cite: 22]
    else if (strcmp(argv[1], "-a") == 0)
    {
        if (argc < 3)
        {
            printf("Hata: Arşiv dosyası belirtilmedi!\n");
            return 1;
        }

        char *archive_name = argv[2];
        char *target_dir = (argc > 3) ? argv[3] : ".";

        // Uzantı Kontrolü [cite: 24]
        if (strstr(archive_name, ".sau") == NULL)
        {
            printf("Arşiv dosyası uygunsuz veya bozuk!\n");
            return 0;
        }

        FILE *src = fopen(archive_name, "r");
        if (!src)
        {
            printf("Arşiv dosyası uygunsuz veya bozuk!\n");
            return 0;
        }

        char size_buf[11] = {0};
        if (fread(size_buf, 1, 10, src) < 10)
        {
            printf("Arşiv dosyası uygunsuz veya bozuk!\n");
            fclose(src);
            return 0;
        }
        long header_size = atol(size_buf);

        char *header_data = malloc(header_size - 10 + 1);
        fread(header_data, 1, header_size - 10, src);
        header_data[header_size - 10] = '\0';

        if (strcmp(target_dir, ".") != 0)
        {
            mkdir(target_dir, 0777);
        }

        char *token = strtok(header_data, "|");
        while (token != NULL)
        {
            char fname[256];
            unsigned int fmode;
            long fsize;

            if (sscanf(token, "%[^,],%o,%ld", fname, &fmode, &fsize) == 3)
            {
                char path[512];
                sprintf(path, "%s/%s", target_dir, fname);

                FILE *dest = fopen(path, "w");
                if (dest)
                {
                    for (long i = 0; i < fsize; i++)
                    {
                        int c = fgetc(src);
                        if (c != EOF)
                            fputc(c, dest);
                    }
                    fclose(dest);
                    chmod(path, fmode);
                }
            }
            token = strtok(NULL, "|");
        }
        free(header_data);
        fclose(src);
        printf("%s dizininde dosyalar açıldı.\n", target_dir);
    }
    else
    {
        printf("Hatalı parametre!\n");
    }
    return 0;
}