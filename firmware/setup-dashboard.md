# Setup & Panduan Dashboard Node-RED — Smart Water Usage (STEP 10)

> Status: ✅ BERHASIL (17 Oktober 2026). Pipeline end-to-end terbukti:
> ESP32 → MQTT → Node-RED → parse JSON → gauge/chart bergerak dgn data nyata
> (Gauge Pressure menampilkan nilai ADC nyata, mis. 305/2112/4095).
> Dokumen ini melanjutkan `setup-mqtt.md` (STEP 8) & `setup-coap.md` (STEP 9).
> Tujuan: tampilkan data sensor real-time + status kebocoran di dashboard, dan
> siapkan jembatan agar MQTT **dan** CoAP tampil di satu dashboard (kriteria utama skripsi).

---

## 0. Posisi saat ini (checkpoint)

- ✅ `mqtt in` tersambung ke broker, topik `smartwater/data`
- ✅ Node `debug` menampilkan payload JSON di sidebar Debug
  - (Tadi sempat tidak muncul karena **wire/line antar node belum tersambung**. Setelah disambung, JSON muncul.)
- ✅ `nan`/`inf` dari INA219 diperbaiki di kode ESP32 → JSON jadi valid → ter-parse jadi **object**
- ✅ Semua node gauge/chart valid (Type, Value, min/max, Segment diisi)
- ✅ Dashboard live di `http://localhost:1880/dashboard` — Gauge Pressure bergerak dgn data nyata
- 🚧 Selanjutnya: alirkan air (flow bergerak), rapikan INA219 (voltage/current), jembatani CoAP (dua protokol)

---

## 1. Konsep dasar: bagaimana data mengalir di Node-RED

Node-RED berbasis **flow** (aliran). Data mengalir kiri → kanan lewat node yang
disambung dengan **wire** (garis). Tiap node melakukan satu tugas, lalu meneruskan
objek pesan bernama **`msg`** ke node berikutnya.

### Objek `msg` (konsep paling penting)
Semua data dibungkus di `msg`, isinya utama di **`msg.payload`**.

| Tahap | Isi `msg.payload` | Tipe |
|-------|-------------------|------|
| Setelah `mqtt in` | `'{"flow1":0.00,...}'` | **string** (teks) |
| Setelah `json` | `{flow1: 0, flow2: 0, ...}` | **object** (bisa diambil per-field) |

➡️ Karena itu node **`json` WAJIB** ada di antara `mqtt in` dan node lain. Tanpa `json`,
`msg.payload.flow1` akan bernilai `undefined`.

### Tipe data tiap field (setelah parse)
Semua field bertipe **number**. Pembedaan di bawah hanya untuk mengatur jumlah desimal di gauge:

| Field | Contoh | Jenis number | Desimal gauge |
|-------|--------|--------------|---------------|
| `flow1` | `0.00` | desimal (float) | 2 |
| `flow2` | `0.00` | desimal (float) | 2 |
| `pressure_adc` | `461` | bulat (integer), rentang 0–4095 | 0 |
| `voltage` | `0.00` | desimal (float) | 2 |
| `current` | `0.0` | desimal (float) | 2 |

### ⚠️ Jebakan `nan` (WAJIB diperbaiki)
JSON **tidak mengenal** `nan`. Jika INA219 longgar dan payload berisi `"current":nan`,
maka node `json` **gagal parse** → semua gauge kosong. Perbaiki di kode ESP32:
```cpp
float current = ina219.getCurrent_mA();
if (isnan(current)) current = 0;   // cegah nan masuk ke JSON
```

---

## 2. Diagram flow yang dibangun

```
┌──────────┐   ┌──────┐   ┌──────────┐   ┌─────────┐
│ mqtt in  │──▶│ json │──▶│ function │──▶│ gauge   │  Flow1, Flow2, Pressure
│          │   │      │   │(deteksi  │   ├─────────┤
└──────────┘   └──────┘   │ bocor)   │──▶│ chart   │  grafik tren
                   │       └──────────┘   ├─────────┤
                   │           │          │ text    │  status NORMAL/BOCOR
                   │           └─────────▶└─────────┘
                   └─────────▶┌─────────┐
                              │ debug   │  cek di sidebar
                              └─────────┘
```

| Node | Tugas |
|------|-------|
| `mqtt in` | Terima pesan dari broker (topik `smartwater/data`) — pintu masuk data |
| `json` | Ubah string JSON → object agar field bisa dibaca |
| `function` | Hitung selisih flow → tentukan status NORMAL/BOCOR |
| `gauge` | Tampilkan angka tiap sensor sebagai meteran |
| `chart` | Grafik garis tren dari waktu ke waktu |
| `text` | Tampilkan teks status (hijau NORMAL / merah BOCOR) |
| `debug` | Cetak `msg` ke sidebar — alat cek saat bermasalah |

---

## 3. OPSI 1 — Import flow siap pakai (paling cepat) ⭐

File: `dashboard/flow-dashboard.json`

1. Install **Dashboard 2.0** dulu (sekali saja):
   - Menu ☰ (kanan atas) → **Manage palette** → tab **Install**
   - Cari **`@flowfuse/node-red-dashboard`** → **Install**
2. Import flow:
   - Menu ☰ → **Import**
   - Pilih **select a file to import** → ambil `dashboard/flow-dashboard.json`
     (atau salin-tempel isinya) → **Import**
3. Pastikan broker benar: buka node `mqtt in` → config broker **localhost:1883**
   (sama seperti saat tes `mosquitto_sub`).
4. Klik **Deploy** (tombol merah kanan atas).
5. Buka dashboard: **http://localhost:1880/dashboard**

> Catatan: flow sudah berisi node `function` deteksi bocor dengan
> `THRESHOLD_SELISIH = 0.5`. Nilai ini **sementara** — kalibrasi pakai data kondisi
> normal (lihat `rencana-pengujian.md` bagian B).

---

## 4. OPSI 2 — Manual drag node (biar paham prosesnya)

Lakukan ini kalau mau benar-benar memahami tiap langkah. Hasil akhirnya sama dengan Opsi 1.

### Langkah 4.1 — Siapkan input (sudah ✅)
1. Drag node **`mqtt in`** dari palette (kiri) ke kanvas.
2. Double-click → set **Server** = broker localhost (Add new → Server `localhost`, Port `1883`).
3. Set **Topic** = `smartwater/data` → **Done**.

### Langkah 4.2 — Tambah node `json`
1. Drag node **`json`** ke kanvas (di kanan `mqtt in`).
2. Biarkan default (**Action: Convert between JSON String & Object** / "obj").
3. **Sambungkan wire:** tarik dari titik keluar `mqtt in` ke titik masuk `json`.
   - 👉 Kalau wire tidak disambung, data tidak mengalir (ini penyebab debug sempat kosong).

### Langkah 4.3 — Cek dengan `debug`
1. Drag node **`debug`** → sambungkan dari `json`.
2. Set **Output** = `msg.payload` → Done.
3. **Deploy** → buka sidebar **Debug** (ikon kutu) → data JSON harus muncul tiap detik.

### Langkah 4.4 — Tambah logika deteksi bocor (`function`)
1. Drag node **`function`** → sambungkan dari `json`.
2. Isi kode:
```javascript
const flow1 = Number(msg.payload.flow1) || 0;
const flow2 = Number(msg.payload.flow2) || 0;

const THRESHOLD_SELISIH = 0.5;   // kalibrasi nanti
const selisih = flow1 - flow2;

let status = "NORMAL";
let warna = "#4caf50";           // hijau
if (selisih > THRESHOLD_SELISIH) {
    status = "BOCOR";
    warna = "#f44336";           // merah
}

msg.status = status;
msg.warna = warna;
msg.selisih = selisih;
return msg;
```

### Langkah 4.5 — Tambah visual (gauge / chart / text)
1. **Gauge Flow1:** drag `ui-gauge` → sambung dari `function`.
   - Buat **Group** & **Page** baru (ui-group "Sensor", ui-page "Monitoring").
   - **Value format:** `{{msg.payload.flow1 | number:2}}`  → Min `0`, Max `30`.
2. **Gauge Flow2:** salin gauge, ganti value jadi `{{msg.payload.flow2 | number:2}}`.
3. **Gauge Pressure:** value `{{msg.payload.pressure_adc | number:0}}`, Min `0`, Max `4095`.
4. **Chart:** drag `ui-chart` → sambung dari `function` → tipe **Line**.
5. **Text status:** drag `ui-text` → sambung dari `function`.
   - **Value format:** `{{msg.status}}`  (nanti bisa diberi warna dari `msg.warna`).
6. **Deploy** → buka **http://localhost:1880/dashboard**.

> Tip: untuk gauge, kamu TIDAK perlu memisah field dengan node `change`. Gauge Dashboard 2.0
> bisa baca sub-field langsung lewat **Value format** (`{{msg.payload.flow1}}`).

---

## 5. STEP berikutnya — Jembatani CoAP ke dashboard yang sama ⭐ (pembeda skripsi)

Dashboard harus menerima **dua protokol**. CoAP tidak didukung native, jadi perlu jembatan.
Dua opsi (pilih salah satu, konfirmasi ke pembimbing):

- **Opsi A — node CoAP di Node-RED:** install `node-red-contrib-coap`, pakai node `coap in`
  (Node-RED jadi CoAP server). Sambungkan ke `json` → `function` → visual yang SAMA.
- **Opsi B — teruskan dari server Python:** `coap_server.py` (sudah jalan di STEP 9)
  meneruskan tiap payload ke Node-RED (mis. publish ulang ke MQTT lokal topik
  `smartwater/data_coap`, atau HTTP POST ke endpoint Node-RED). Lalu masuk ke flow dashboard.

> Agar perbandingan adil, bedakan sumber (mis. `msg.topic = "mqtt"` vs `"coap"`) supaya
> bisa dibandingkan di chart/log terpisah.

---

## 6. STEP logging ke CSV (untuk analisis BAB IV)

Tambah node **`file`** setelah `function`:
- Format tiap baris: `timestamp,protokol,flow1,flow2,pressure_adc,voltage,current,selisih,status`
- Simpan ke `smartwater_log.csv` → dipakai hitung latency/throughput/loss & statistik (mean, std).

---

## 7. Hasil Uji (BUKTI NYATA)

### Debug Node-RED (setelah fix nan/inf):
```
node: Debug (lihat payload)
smartwater/data : msg.payload : Object
{ flow1: 0, flow2: 0, pressure_adc: 4095, voltage: 0, current: 0 }
```
→ payload sudah **object** (bukan `string[76]`), tanpa `nan`/`inf`. ✅

### Serial Monitor ESP32:
```
Connecting MQTT broker... terhubung!
Published: {"flow1":0.00,"flow2":0.00,"pressure_adc":4095,"voltage":0.00,"current":0.0}
```
→ `current` jadi `0.0` (bukan `nan`/`inf`) = JSON valid. ✅

### Dashboard (http://localhost:1880/dashboard):
- Gauge Pressure (ADC) bergerak dgn nilai nyata (mis. 305, 2112, 4095). ✅
- Gauge Flow1/Flow2 = 0 (wajar, belum ada air mengalir).
- Chart & Status Kebocoran tampil tanpa error.

---

## 8. Troubleshooting (ISU NYATA yang sudah diselesaikan)

| Isu | Penyebab | Solusi |
|-----|----------|--------|
| Debug kosong | wire antar node belum tersambung | Sambungkan garis dari output ke input node berikutnya |
| Payload `string[76]`, gauge diam di 0 | payload berisi `"current":nan` / `inf` → JSON invalid → `json` gagal parse | **Fix di ESP32:** `if (isnan(x)||isinf(x)) x=0;` untuk `busV` & `cur` sebelum `snprintf` JSON |
| `msg.payload.flow1` undefined | lupa node `json` | Tambah node `json` sebelum `function`/gauge |
| Node gauge invalid: "segments / Invalid number" | gauge butuh Type, min/max angka, dan ≥1 Segment dgn `from` = min | Set Type=Gauge, Value=`payload.flowX`, min=0, max, tambah Segment `from=0` |
| Node chart invalid / "Invalid property expression" (INVALID_EXPR) | field Y masih `key: property` (default) — tidak ada di payload | Set Y = `key` + nama field nyata (mis. `flow1`); X=timestamp, Series=`msg.topic` |
| mqtt in "disconnected" | broker tidak jalan / IP salah | Jalankan Mosquitto; broker = `localhost:1883` di Node-RED (laptop yg sama) |
| ESP32 `rc=-4` connect broker gagal (padahal IP benar) | broker Mosquitto hanya listen `localhost` → tolak koneksi dari IP ESP32 | Jalankan dgn config `listener 1883 0.0.0.0`; buka firewall port 1883 TCP; `mosquitto -c C:\mqtt\mosquitto.conf -v` |
| ESP32 `Association refused` / `CCMP replay detected` | ESP32 nyangkut state koneksi lama / Wi-Fi goyang | RESET BERSIH (cabut USB 5 dtk, reconnect Wi-Fi laptop, colok lagi, tekan EN); opsional `WiFi.disconnect(true); delay(1000);` sebelum `WiFi.begin()` |
| Dashboard 404 | Dashboard 2.0 belum diinstall / belum Deploy | Install `@flowfuse/node-red-dashboard`, Deploy, buka `/dashboard` |
| "unused configuration nodes" saat deploy | ada config node (broker/group lama) tak terpakai | Info saja, bukan error — abaikan atau hapus via "Search unused config nodes" |

---

## 9. Checklist STEP 10 (Dashboard)

1. [x] Install Dashboard 2.0 (`@flowfuse/node-red-dashboard`)
2. [x] Import `dashboard/flow-dashboard.json` ATAU bangun manual (Opsi 2)
3. [x] Broker `mqtt in` = localhost:1883, Deploy
4. [x] Buka `http://localhost:1880/dashboard` → gauge bergerak saat data masuk (Pressure terbukti)
5. [ ] Alirkan air → cek nilai nyata (flow/pressure berubah)
6. [ ] Tambah jembatan CoAP (Opsi A/B) → dua protokol di satu dashboard
7. [ ] Tambah logging CSV
8. [ ] Kalibrasi `THRESHOLD_SELISIH` dari data normal → uji deteksi bocor

---

## 10. Yang BELUM dilakukan (lanjutan)

- [ ] Jembatan CoAP → dashboard (kriteria utama: dua protokol)
- [ ] Logging CSV + pengukuran kinerja (latency, throughput, loss, energy, reliability)
- [ ] Kalibrasi threshold deteksi bocor dengan data nyata
- [ ] Perbaiki `current: nan` (rapikan INA219) untuk pengukuran energy
- [ ] Isi hasil nyata ke BAB IV → tandai `[ISI SETELAH PENGUJIAN]`
