# Setup Logging CSV OTOMATIS (Arduino → CSV) — v2

> Tujuan: data sensor dari ESP32 TERSIMPAN OTOMATIS ke file CSV di laptop, tanpa perlu
> copy-paste manual dari Serial Monitor. Mengikuti praktik [M9] Ayamga (data direkam ke
> penyimpanan untuk dianalisis) & [M-arXiv] Krikorian (Node-RED menulis ke .csv).
>
> Ada 2 cara. PILIH SALAH SATU. Cara A (Node-RED) PALING disarankan karena dashboard sudah ada.

---

## CARA A — Node-RED menulis CSV (DISARANKAN) ⭐

Dashboard-mu (`dashboard/flow-dashboard.json`) sudah punya alur:
`mqtt in → json (parse) → function (deteksi) → gauge/chart/text`.
Kita tinggal menambah 2 node (csv + file) yang MENCABANG dari node `json`.

### Langkah di editor Node-RED (http://localhost:1880)

1. Seret node **`csv`** (kategori "parser") ke kanvas.
2. Seret node **`file`** (kategori "storage") ke kanvas.
3. **Hubungkan:** output node **"Parse JSON -> object"** → node **csv** → node **file**.
   (Node json sudah mengeluarkan object; kita ubah ke baris CSV lalu tulis ke file.)

### Konfigurasi node `csv`
- **Columns (urutan kolom):**
  ```
  waktu,flow1,flow2,selisih,pressure_adc,voltage,current,status
  ```
- Mode: **"JSON object/array to CSV"** (object → string CSV).
- Centang **"include column header row"** HANYA jika file masih kosong (biar header 1×).
  (Praktisnya: kosongkan dulu, lalu tambahkan header manual sekali di file.)
- Output: **"a line per message"** (satu baris per pesan).

### Konfigurasi node `file`
- **Filename:** `C:\smartwater\log-deteksi.csv`  (buat folder `C:\smartwater` dulu)
- **Action:** **"append to file"** (tambah baris, jangan overwrite).
- Centang **"Add newline (\n) to each payload"**.

### Tambahan penting — agar ada kolom `waktu` & `selisih` & `status`
Node `function` deteksi perlu menaruh field itu ke `msg.payload` sebelum CSV.
Tambahkan node **function kecil** ("Siapkan baris CSV") SEBELUM node csv, isi:
```javascript
// pakai detik sejak Node-RED start, atau timestamp nyata
const p = msg.payload;
msg.payload = {
  waktu: new Date().toISOString(),        // timestamp nyata (lebih baik dari millis ESP32)
  flow1: p.flow1, flow2: p.flow2,
  selisih: (Number(p.flow1)||0) - (Number(p.flow2)||0),
  pressure_adc: p.pressure_adc,
  voltage: p.voltage, current: p.current,
  status: p.status
};
return msg;
```

### Hasil
Tiap pesan MQTT dari ESP32 otomatis jadi 1 baris di `C:\smartwater\log-deteksi.csv`.
Buka file itu langsung di Excel/Google Sheets → sudah jadi tabel rapi + bisa dibuat grafik.

> Tips sesi uji: sebelum tiap sesi, rename file lama (mis. `log-deteksi-2026-10-30.csv`)
> biar tiap sesi punya file sendiri (tidak tercampur). Tulis di kolom catatan: kondisi keran.

---

## CARA B — Skrip Python subscriber → CSV (alternatif)

Kalau lebih suka lewat skrip (tanpa Node-RED), jalankan ini di laptop (butuh `paho-mqtt`):
`pip install paho-mqtt`

Simpan sebagai `mqtt_to_csv.py`, jalankan saat uji:
```python
import csv, json, time
import paho.mqtt.client as mqtt

BROKER = "127.0.0.1"   # localhost (broker & skrip di laptop yang sama)
TOPIC  = "smartwater/data"
OUT    = "log-deteksi.csv"

f = open(OUT, "a", newline="")
w = csv.writer(f)
# tulis header sekali kalau file baru
if f.tell() == 0:
    w.writerow(["waktu","flow1","flow2","selisih","pressure_adc","voltage","current","status"])

def on_message(client, userdata, msg):
    try:
        d = json.loads(msg.payload.decode())
        f1 = float(d.get("flow1",0)); f2 = float(d.get("flow2",0))
        w.writerow([time.strftime("%Y-%m-%d %H:%M:%S"), f1, f2, round(f1-f2,2),
                    d.get("pressure_adc"), d.get("voltage"), d.get("current"), d.get("status")])
        f.flush()
        print("tersimpan:", d)
    except Exception as e:
        print("skip:", e)

c = mqtt.Client()
c.on_message = on_message
c.connect(BROKER, 1883, 60)
c.subscribe(TOPIC)
print("Logging ke", OUT, "... Ctrl+C untuk berhenti")
c.loop_forever()
```
> Catatan: skrip ini TIDAK mengubah firmware — hanya "mendengarkan" MQTT yang sudah ada.
> Angka yang tersimpan = PERSIS yang dikirim ESP32 (tidak diolah).

---

## Verifikasi
1. Jalankan broker + (Node-RED ATAU skrip Python).
2. Alirkan air sebentar → cek file CSV bertambah barisnya.
3. Buka di Excel → pastikan kolom rapi & angka sama dengan Serial Monitor.
