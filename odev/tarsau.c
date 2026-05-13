#include "tarsau.h"

/*
 * Parametre olarak verilen dosyanın içeriğini okuyarak
 * sadece ASCII karakterlerden oluşup oluşmadığını denetler.
 * ASCII dışı bir karakter (değeri 0'dan küçük veya 127'den büyük)
 * bulunursa 0, tamamen ASCII ise 1 döndürür.
 * */

int is_ascii(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0; // Dosya açılamazsa güvenli çıkış yap

    int c;
    // Dosya sonuna (EOF) gelene kadar bayt bayt oku
    while ((c = fgetc(f)) != EOF)
    {
        if (c < 0 || c > 127)
        {
            fclose(f);
            return 0; // Uyumsuz karakter bulundu
        }
    }
    fclose(f);
    return 1; // Dosya formata uygun
}

int main(int argc, char *argv[])
{
    // Yetersiz argüman kontrolü
    if (argc < 2)
    {
        printf("Kullanım: ./tarsau -b [dosyalar] -o [cikti.sau] veya ./tarsau -a [arsiv.sau] [dizin]\n");
        return 1;
    }

    // BİRLEŞTİRME (ARŞİVLEME) MODU (-b)
    // Birden fazla metin dosyasını sıkıştırma yapmadan tek dosyada toplar
    if (strcmp(argv[1], "-b") == 0)
    {
        char *output_name = "a.sau"; // Kullanıcı isim vermezse varsayılan arşiv adı
        int file_count = 0;
        char *input_files[MAX_FILES];
        long total_size = 0;

        // Komut satırı argümanlarını ayrıştırma
        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], "-o") == 0)
            {
                // -o parametresi geldiyse, bir sonraki argümanı çıktı adı olarak al
                if (i + 1 < argc)
                    output_name = argv[++i];
            }
            else
            {
                // Girilen dosya sayısı sınırı aşıyor mu kontrol et
                if (file_count >= MAX_FILES)
                {
                    printf("Hata: En fazla 32 giriş dosyası belirtilebilir!\n");
                    return 1;
                }
                input_files[file_count++] = argv[i];
            }
        }

        // Hiç giriş dosyası verilmemişse işlemi durdur
        if (file_count == 0)
        {
            printf("Hata: En az bir giriş dosyası belirtilmelidir!\n");
            return 1;
        }

        char header[10000] = ""; // Organizasyon (içerik) bilgilerini tutacak bellek alanı

        // Giriş dosyalarının doğrulanması ve başlık (header) verisinin oluşturulması
        for (int i = 0; i < file_count; i++)
        {
            // İster: Yalnızca ASCII metin dosyaları kabul edilmelidir
            if (!is_ascii(input_files[i]))
            {
                printf("%s giriş dosyasının formatı uyumsuzdur!\n", input_files[i]);
                return 0; // Sorunsuz bir şekilde programdan çık
            }

            // Dosya boyut ve izin bilgilerini işletim sisteminden çek
            struct stat st;
            if (stat(input_files[i], &st) != 0)
                return 1;

            // İster: Toplam boyut 200 MB'ı geçemez
            total_size += st.st_size;
            if (total_size > MAX_TOTAL_SIZE)
            {
                printf("Hata: Giriş dosyalarının toplam boyutu 200 MB'ı geçemez!\n");
                return 1;
            }

            char entry[512];

            // Dosya yolundan (örn: /home/user/t1.txt) sadece dosya adını (t1.txt) ayıkla
            char *filename = strrchr(input_files[i], '/');
            filename = (filename != NULL) ? filename + 1 : input_files[i];

            // Formata uygun kayıt oluştur: |Dosya adı, izinler(sekizli tabanda), boyut|
            sprintf(entry, "|%s,%04o,%ld|", filename, (unsigned int)(st.st_mode & 0777), (long)st.st_size);
            strcat(header, entry); // Ana başlığa ekle
        }

        // Çıktı arşiv dosyasını yazma modunda aç
        FILE *out = fopen(output_name, "w");
        if (!out)
            return 1;

        // İster: Organizasyon bölümünün ilk 10 baytı sayısal boyutu içermelidir
        fprintf(out, "%010ld", (long)(strlen(header) + 10));

        // Başlık bilgilerini dosyaya yaz
        fprintf(out, "%s", header);

        // İster: Arşivlenmiş dosyalar ayırıcı olmadan art arda yerleştirilir
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

    // AÇMA (ÇIKARMA) MODU (-a) 
    // Oluşturulan .sau arşivini okuyup dosyaları orijinal izinleriyle çıkarır

    else if (strcmp(argv[1], "-a") == 0)
    {
        if (argc < 3)
        {
            printf("Hata: Arşiv dosyası belirtilmedi!\n");
            return 1;
        }

        char *archive_name = argv[2];
        char *target_dir = (argc > 3) ? argv[3] : "."; // Dizin belirtilmezse mevcut dizin (.)

        // İster: Arşiv dosyası kesinlikle .sau uzantılı olmalıdır
        size_t len = strlen(archive_name);
        if (len < 4 || strcmp(archive_name + len - 4, ".sau") != 0)
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

        // Başlığın boyutunu öğrenmek için ilk 10 baytı oku
        char size_buf[11] = {0};
        if (fread(size_buf, 1, 10, src) < 10)
        {
            printf("Arşiv dosyası uygunsuz veya bozuk!\n");
            fclose(src);
            return 0;
        }
        long header_size = atol(size_buf);

        // Dinamik bellek tahsisi ile başlık verisini oku
        char *header_data = malloc(header_size - 10 + 1);
        fread(header_data, 1, header_size - 10, src);
        header_data[header_size - 10] = '\0'; // String sonlandırıcı ekle

        // Hedef dizin mevcut dizin değilse, yeni dizin oluştur (varsa hata vermez)
        if (strcmp(target_dir, ".") != 0)
        {
            mkdir(target_dir, 0777);
        }

        char extracted_files[2048] = ""; // Ekrana basılacak çıkarılan dosya isimleri
        int first_file = 1;

        // Başlık verisini '|' ayırıcısına göre parçala (Tokenization)
        char *token = strtok(header_data, "|");
        while (token != NULL)
        {
            char fname[256];
            unsigned int fmode;
            long fsize;

            // Kayıt alanlarını ayrıştır: Dosya adı, izinler, boyut
            if (sscanf(token, "%[^,],%o,%ld", fname, &fmode, &fsize) == 3)
            {
                char path[512];
                // Hedef dizin ile dosya adını birleştirerek tam yolu oluştur
                sprintf(path, "%s/%s", target_dir, fname);

                FILE *dest = fopen(path, "w");
                if (dest)
                {
                    // Dosya boyutu kadar bayt okuyup yeni dosyaya yaz
                    for (long i = 0; i < fsize; i++)
                    {
                        int c = fgetc(src);
                        if (c != EOF)
                            fputc(c, dest);
                    }
                    fclose(dest);

                    // İster: Açılan dosyalar orijinal izinlerine sahip olmalıdır
                    chmod(path, fmode);

                    // Ekrana yazdırılacak liste için string formatlaması
                    if (!first_file)
                        strcat(extracted_files, ", ");
                    strcat(extracted_files, fname);
                    first_file = 0;
                }
            }
            token = strtok(NULL, "|"); // Sonraki kayda geç
        }

        free(header_data); // Bellek sızıntısını önlemek için tahsis edilen alanı serbest bırak
        fclose(src);

        // Başarılı işlem sonucunu formatlı şekilde ekrana bas
        printf("%s dizininde %s dosyaları açıldı.\n", target_dir, extracted_files);
    }
    else
    {
        printf("Hatalı parametre!\n");
    }
    return 0; // Programın sorunsuz sonlanması
}