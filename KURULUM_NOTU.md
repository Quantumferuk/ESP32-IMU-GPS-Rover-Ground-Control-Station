# Kurulum Notu

Bu paket Windows + Python ortamında çalışacak şekilde hazırlanmıştır.

## 1. Python kontrolü

PowerShell açıp şunu yaz:

```powershell
py --version
```

Python 3.12 kuruluysa devam edebilirsin.

## 2. Klasöre gir

ZIP dosyasını çıkardıktan sonra proje klasörüne gir:

```powershell
cd "C:\Users\Faruk Erdoğan\Downloads\eltagron_rover_gcs_preserve_user_changes_readme_only"
```

Klasör adı sende farklıysa kendi klasör adını kullan.

## 3. Gerekli paketleri kur

```powershell
pip install -r requirements.txt
```

## 4. Arayüzü çalıştır

```powershell
py -3.12 main.py
```

veya:

```powershell
.\run_ui.bat
```

## 5. ESP32 bağlantısı

Arayüz açıldıktan sonra COM port seçilir ve `CONNECT` butonuna basılır.

Beklenen veri formatı:

```text
GPS:lat,lon,speed_kmph
IMU:yaw,ax,ay,az
```

Kontrol komutları:

```text
F B L R S 1 2 3
```

## Sorun olursa

- COM port görünmüyorsa USB kablosu, sürücü ve kart bağlantısı kontrol edilir.
- Grafikler boşsa seri porttan gerçek IMU/GPS verisi gelmiyor olabilir.
- Harita açılmıyorsa internet bağlantısı veya `tkintermapview` kurulumu kontrol edilir.
