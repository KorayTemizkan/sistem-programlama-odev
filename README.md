# tarsau Arşivleme Programı

`tarsau`, ASCII metin dosyalarını sıkıştırma yapmadan tek bir `.sau` arşiv dosyasında birleştiren ve bu arşivi tekrar açabilen C tabanlı bir komut satırı programıdır.

## Proje Tanımı

Bu proje, Sistem Programlama dersi kapsamında geliştirilmiştir. Program, `tar`, `rar` veya `zip` benzeri şekilde çalışır; ancak dosyaları sıkıştırmaz. Bunun yerine birden fazla metin dosyasını özel `.sau` formatında tek dosyada toplar.

Program iki temel işlem yapar:

- Dosyaları birleştirerek `.sau` arşivi oluşturur.
- Oluşturulan `.sau` arşivini açarak dosyaları geri çıkarır.

Arşivleme sırasında dosyaların adı, boyutu ve izin bilgileri kaydedilir. Arşiv açıldığında dosyalar bu bilgilere göre yeniden oluşturulur ve dosya izinleri korunur.

Bu projede C dili kullanılmasının sebebi, dosya işlemleri, bellek yönetimi ve işletim sistemi çağrıları gibi Sistem Programlama konularını doğrudan uygulayabilmektir.

## Kurulum ve Çalıştırma

Projeyi çalıştırmak için Linux/Unix tabanlı bir ortamda `gcc` ve `make` kurulu olmalıdır.

Derleme adımları:

```bash
cd odev
make
```

Derleme tamamlandığında `tarsau` adlı çalıştırılabilir dosya oluşur.

Derleme çıktısını temizlemek için:

```bash
make clean
```

## Proje Nasıl Kullanılır?

### Arşiv Oluşturma

Aşağıdaki komut, verilen dosyaları `s1.sau` adlı arşiv dosyasında birleştirir:

```bash
./tarsau -b t1 t2 t3 -o s1.sau
```

Başarılı çıktı:

```text
Dosyalar birleştirildi.
```

Eğer arşiv adı belirtilmezse varsayılan olarak `a.sau` oluşturulur:

```bash
./tarsau -b t1 t2 t3
```

### Arşiv Açma

Aşağıdaki komut, `s1.sau` arşivini açar ve dosyaları `d1` dizinine çıkarır:

```bash
./tarsau -a s1.sau d1
```

Başarılı çıktı örneği:

```text
d1 dizininde t1, t2, t3 dosyaları açıldı.
```

Hedef dizin belirtilmezse dosyalar mevcut dizine çıkarılır:

```bash
./tarsau -a s1.sau
```

## Temel Kurallar

- Giriş dosyaları ASCII metin dosyası olmalıdır.
- En fazla 32 dosya arşivlenebilir.
- Toplam dosya boyutu 200 MB'ı geçemez.
- Arşiv dosyası `.sau` uzantılı olmalıdır.
- Arşiv açılırken dosya izinleri korunur.

Örnek hata mesajları:

```text
t7 giriş dosyasının formatı uyumsuzdur!
Arşiv dosyası uygunsuz veya bozuk!
```

## Kullanılan Teknolojiler

- C programlama dili
- GCC
- Makefile
- Linux/Unix dosya sistemi işlemleri
- POSIX sistem çağrıları
