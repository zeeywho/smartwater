[bab4-kerangka-revised.md](https://github.com/user-attachments/files/33565146/bab4-kerangka-revised.md)
# BAB IV — HASIL DAN PEMBAHASAN (Versi Revisi — Gaya Naratif)

> Versi ini mengikuti gaya layout contoh BAB IV kakak tingkat: pengelompokan subbab lebih sederhana & mengalir,
> banyak narasi paragraf, tabel alat/library, prosedur bernomor (1,2,3 lalu a,b,c), tiap gambar/tabel dirujuk di teks.
> Scope: MQTT + CoAP (LoRa dihapus), gateway laptop.
> Aturan: jangan mengarang hasil. Hasil eksperimen = `[ISI SETELAH PENGUJIAN]`. Rancangan belum terverifikasi = `[RANCANGAN — BELUM DIVERIFIKASI]`.
> Penomoran gambar/tabel bersifat sementara, disesuaikan saat finalisasi.

---

## 4.1 Implementasi Sistem

Paragraf pembuka: ringkas metode/alur pengembangan sistem Smart Water Usage — dari analisis kebutuhan, perancangan, hingga implementasi, dengan fokus perbandingan MQTT dan CoAP.

### 4.1.1 Tahap Analisis
Narasi: kebutuhan sistem deteksi kebocoran pipa rumah tangga; faktor yang jadi indikator kebocoran (selisih debit masuk–keluar, penurunan tekanan); kebutuhan membandingkan protokol komunikasi (MQTT vs CoAP) sebagai inti penelitian; hasil studi pustaka singkat yang mendasari pemilihan parameter kinerja.

### 4.1.2 Tahap Perancangan (Design)
Narasi: penetapan variabel yang dimonitor (Flow1, Flow2, Pressure, konsumsi daya); perancangan arsitektur IoT 3-lapisan; pemilihan ESP32 sebagai node sensor; pemilihan sensor (2× YF-S201, WPT-83G, INA219); keputusan 1 sumber data dikirim via 2 protokol dengan payload identik agar perbandingan adil.
- Rujuk: Gambar 4.x Arsitektur Sistem (`diagrams/arsitektur-sistem.drawio`)

### 4.1.3 Tahap Implementasi
Narasi: realisasi rancangan — perakitan sensor ke pipa prototipe, penyambungan ke ESP32, konfigurasi power (adaptor 5V), penyiapan gateway laptop (broker MQTT + CoAP server), lalu sistem dijalankan untuk mengumpulkan data awal, dilanjutkan evaluasi (fungsional/black box + pengukuran kinerja protokol).
- Status: sebagian sudah berjalan (sensor diuji), sebagian `[ISI SETELAH PENGUJIAN]`

---

## 4.2 Implementasi dan Hasil Perangkat Keras (IoT)

Paragraf pembuka: uraikan 3 lapisan IoT dalam konteks sistem ini — perception layer (ESP32 + sensor), network layer (Wi-Fi, MQTT & CoAP), application layer (laptop: broker/server + dashboard). Jelaskan singkat alur data.

### Tabel 4.1 Alat dan Bahan yang Digunakan
| Alat | Deskripsi |
|------|-----------|
| ESP32 DOIT DevKit V1 | Mikrokontroler 38-pin, Wi-Fi, sebagai node sensor & agregator; menjalankan firmware MQTT & CoAP |
| Water Flow Sensor YF-S201 (×2) | Sensor aliran efek Hall, range 1–30 L/min, output pulsa; mengukur debit masuk & keluar |
| Pressure Sensor WISNER WPT-83G | Sensor tekanan 0–12 Bar, supply 5–12VDC, output 0.5–4.5V; indikator penurunan tekanan |
| INA219 (CJMCU-219) | Sensor arus/tegangan I2C; mengukur konsumsi daya ESP32 (parameter energy consumption) |
| Adaptor 5V 2A | Catu daya 5V untuk sensor (via rail breadboard) |
| Breadboard, kabel jumper, resistor | Media perakitan prototipe; resistor 20K/10K untuk voltage divider pressure |
| Pipa PVC + T-junction | Prototipe jalur air dengan titik simulasi kebocoran |

### Tabel 4.2 Library/Perangkat Lunak yang Digunakan
| Nama | Deskripsi |
|------|-----------|
| Arduino IDE | Lingkungan pengembangan firmware ESP32 |
| Adafruit INA219 | Library membaca INA219 via I2C |
| (library MQTT, mis. PubSubClient) | `[ISI SAAT IMPLEMENTASI MQTT]` |
| (library CoAP, mis. CoAP-simple-library) | `[ISI SAAT IMPLEMENTASI CoAP]` |
| Node-RED (+ node CoAP) | Middleware gateway: terima MQTT & CoAP, dashboard (rencana) |

### 4.2.1 Prosedur Perakitan Perangkat Keras
Format bernomor + sub-poin + gambar tiap komponen (mengikuti gaya contoh):
1. Menyiapkan seluruh komponen:
   a. Water Flow Sensor 1 (YF-S201) — Gambar 4.x
   b. Water Flow Sensor 2 (YF-S201) — Gambar 4.x
   c. Pressure Sensor WPT-83G — Gambar 4.x
   d. INA219 — Gambar 4.x
   e. ESP32 DOIT V1 — Gambar 4.x
   f. Adaptor 5V 2A + konektor — Gambar 4.x
2. Menyusun jalur daya: adaptor 5V → rail + / − breadboard; common ground ke ESP32.
3. Menyambung sensor ke ESP32:
   a. Flow 1 → GPIO25 (langsung, INPUT_PULLUP)
   b. Flow 2 → GPIO27 (langsung, INPUT_PULLUP)
   c. Pressure → voltage divider 20K/10K → GPIO34 (ADC)
   d. INA219 → SDA/SCL (GPIO21/22), VCC 3V3
4. Memasang sensor pada pipa prototipe (Flow1 → T-junction → Pressure → Flow2).
5. Menghubungkan ESP32 ke laptop via USB; menyalakan sistem.
6. Memeriksa sistem berjalan (lihat 4.3 pengujian sensor).
- Rujuk: Gambar 4.x Diagram Wiring (`diagrams/wiring-pinmapping.drawio`), Gambar 4.x Rail 5V (`diagrams/rail-5v-breadboard.drawio`)

### 4.2.2 Prosedur Pengembangan Firmware ESP32
Format bernomor:
1. Menyiapkan Arduino IDE + board ESP32.
2. Membaca sensor (pulsa flow via interrupt, pressure via ADC, INA219 via I2C).
3. Menyusun data menjadi payload JSON tunggal (lihat 4.3.x format data).
4. Mengirim payload via MQTT (publish) — `[ISI SAAT IMPLEMENTASI]`
5. Mengirim payload identik via CoAP (request) — `[ISI SAAT IMPLEMENTASI]`
6. Menyertakan timestamp untuk pengukuran latency.

### 4.2.3 Prosedur Penyiapan Gateway & Dashboard (Laptop)
Format bernomor:
1. Menjalankan broker MQTT (mis. Mosquitto) di laptop — `[ISI SAAT IMPLEMENTASI]`
2. Menjalankan CoAP server (Node-RED node CoAP / skrip) — `[ISI SAAT IMPLEMENTASI]`
3. Menyiapkan Node-RED: node MQTT-in + node CoAP-in → parsing JSON → dashboard.
4. Mencatat data ke penyimpanan (CSV / InfluxDB) untuk analisis.
- Rujuk: `bab4/eksplorasi-dashboard.md`

---

## 4.3 Hasil Pengujian Sensor

Paragraf pembuka: jelaskan pengujian dilakukan per komponen lebih dulu (satu per satu) sebelum gabungan, untuk memastikan tiap sensor valid. Lingkungan: prototipe rumah tangga, catu daya 5V, ESP32 via USB.

### 4.3.1 Pengujian INA219
Narasi + hasil nyata (sudah ada): komunikasi I2C terdeteksi, pembacaan V/I/P. Rujuk `firmware/log-pengujian.md` STEP 3.
- Rujuk: Gambar 4.x Serial Monitor INA219

### 4.3.2 Pengujian Water Flow Sensor 1
Narasi + hasil nyata (sudah ada): pulsa 0 saat diam, naik saat ada aliran (Level pin berubah). Catatan teknis: dihubungkan langsung ke GPIO ber-pull-up (bukan divider) karena open-collector.
- Rujuk: Gambar 4.x Serial Monitor Flow 1

### 4.3.3 Pengujian Water Flow Sensor 2
Narasi + hasil nyata (sudah ada): sama seperti Flow 1 di GPIO27.
- Rujuk: Gambar 4.x Serial Monitor Flow 2

### 4.3.4 Pengujian Pressure Sensor
Narasi + hasil nyata (sudah ada): ADC merespons perubahan tekanan via voltage divider. Catatan: kabel serabut perlu diperkuat; kalibrasi ke Bar menyusul.
- Rujuk: Gambar 4.x Serial Monitor Pressure

### 4.3.5 Pengujian Gabungan Sensor & Format Data
Narasi + hasil: keempat sensor terbaca bersamaan + preview payload JSON.
- Status: `[ISI SETELAH PENGUJIAN]` (STEP 7 belum diuji)
- Format data (contoh struktur, nilai hanya ilustrasi):
  ```json
  {"timestamp":"...","flow1":5.20,"flow2":4.85,"pressure":1.72,"voltage":4.98,"current":85.2}
  ```

---

## 4.4 Implementasi Komunikasi MQTT dan CoAP

Paragraf pembuka: data sensor identik dikirim via dua protokol untuk dibandingkan.

### 4.4.1 Implementasi MQTT
Narasi: broker, topik, QoS, mekanisme publish. `[ISI SETELAH PENGUJIAN]`

### 4.4.2 Implementasi CoAP
Narasi: CoAP server, resource, tipe pesan CON/NON. `[ISI SETELAH PENGUJIAN]`

### 4.4.3 Integrasi Data Sensor dengan Kedua Protokol
Narasi: memastikan payload identik; alur data ke gateway. `[ISI SETELAH PENGUJIAN]`

---

## 4.5 Implementasi Platform Monitoring (Dashboard)

Paragraf pembuka: platform dirancang menerima data dari MQTT & CoAP (Node-RED sebagai pusat; opsional Grafana). Rujuk `bab4/eksplorasi-dashboard.md`.

### 4.5.1 Perancangan Dashboard — `[ISI SETELAH PENGUJIAN]`
### 4.5.2 Tampilan Monitoring Data Sensor — `[ISI SETELAH PENGUJIAN]` (rujuk Gambar 4.x)
### 4.5.3 Tampilan Status Deteksi Kebocoran — `[ISI SETELAH PENGUJIAN]` (rujuk Gambar 4.x)

---

## 4.6 Pengujian Sistem

Paragraf pembuka: jelaskan lingkungan, durasi, interval sampling, perangkat yang dipakai (mengikuti gaya contoh: sebutkan kondisi uji konkret). Rujuk `bab4/rencana-pengujian.md`.

### 4.6.1 Pengujian Fungsional (Black Box)
Format: tabel modul–pengujian–ekspektasi–hasil (mengikuti gaya contoh Tabel 4.3).
| Modul | Pengujian | Ekspektasi | Hasil |
|-------|-----------|------------|-------|
| Hardware | Baca INA219 | V/I/P terbaca | PASS |
| Hardware | Baca Flow 1 | pulsa saat ada aliran | PASS |
| Hardware | Baca Flow 2 | pulsa saat ada aliran | PASS |
| Hardware | Baca Pressure | ADC berubah saat ditekan | PASS |
| Hardware | Baca semua sensor | semua tampil + JSON | `[ISI SETELAH PENGUJIAN]` |
| Komunikasi | Publish MQTT | broker terima | `[ISI SETELAH PENGUJIAN]` |
| Komunikasi | Request CoAP | server terima | `[ISI SETELAH PENGUJIAN]` |
| Dashboard | Tampil data | muncul di dashboard | `[ISI SETELAH PENGUJIAN]` |
| Deteksi | Simulasi bocor | status BOCOR | `[ISI SETELAH PENGUJIAN]` |

### 4.6.2 Pengujian Deteksi Kebocoran
Narasi: skenario Normal vs Bocor; cara simulasi (buka T-junction); kriteria (selisih Flow1−Flow2 + penurunan tekanan).
- 4.6.2.1 Skenario Kondisi Normal — `[ISI SETELAH PENGUJIAN]`
- 4.6.2.2 Skenario Kondisi Kebocoran — `[ISI SETELAH PENGUJIAN]`
- 4.6.2.3 Hasil Deteksi Kebocoran — `[ISI SETELAH PENGUJIAN]`

### 4.6.3 Pengujian Kinerja Protokol MQTT dan CoAP
Narasi: metode ukur tiap parameter; skenario (frekuensi kirim, kondisi jaringan, QoS/mode); jumlah pengulangan.
- 4.6.3.1 End-to-End Latency — `[ISI SETELAH PENGUJIAN]`
- 4.6.3.2 Throughput — `[ISI SETELAH PENGUJIAN]`
- 4.6.3.3 Packet Loss — `[ISI SETELAH PENGUJIAN]`
- 4.6.3.4 Energy Consumption — `[ISI SETELAH PENGUJIAN]`
- 4.6.3.5 Reliability — `[ISI SETELAH PENGUJIAN]`

---

## 4.7 Hasil dan Analisis Perbandingan MQTT dan CoAP

Paragraf pembuka: sajikan hasil komparatif dalam tabel + grafik; analisis statistik deskriptif (mean, std).

### Tabel 4.x Perbandingan Kinerja MQTT vs CoAP (ringkasan)
| Parameter | MQTT | CoAP | Keterangan |
|-----------|------|------|------------|
| Latency (ms) | `[ISI]` | `[ISI]` | |
| Throughput | `[ISI]` | `[ISI]` | |
| Packet loss (%) | `[ISI]` | `[ISI]` | |
| Energy (mWh) | `[ISI]` | `[ISI]` | |
| Reliability (%) | `[ISI]` | `[ISI]` | |

### 4.7.1 Analisis Latency — `[ISI SETELAH PENGUJIAN]`
### 4.7.2 Analisis Throughput — `[ISI SETELAH PENGUJIAN]`
### 4.7.3 Analisis Packet Loss — `[ISI SETELAH PENGUJIAN]`
### 4.7.4 Analisis Energy Consumption — `[ISI SETELAH PENGUJIAN]`
### 4.7.5 Analisis Reliability — `[ISI SETELAH PENGUJIAN]`

---

## 4.8 Pembahasan
Narasi: interpretasi menyeluruh — kaitkan hasil dengan karakteristik protokol (MQTT/TCP vs CoAP/UDP) dalam konteks deteksi kebocoran rumah tangga; keterbatasan; temuan menarik (mis. pelajaran teknis dari proses perakitan). `[ISI SETELAH PENGUJIAN]`

## 4.9 Ringkasan Hasil Penelitian
Narasi: rangkuman temuan utama + rekomendasi protokol optimal untuk skala rumah tangga. `[ISI SETELAH PENGUJIAN]`

---

## Perbedaan versi revisi ini vs kerangka awal (bab4-kerangka.md)
- Pengelompokan subbab lebih sederhana & mengalir (4.1 tahap Analisis/Design/Implementasi; 4.2 hardware + tabel alat & library + prosedur bernomor; 4.3 pengujian sensor; dst) — mengikuti gaya contoh kakak tingkat.
- Ditambah Tabel Alat & Bahan dan Tabel Library (gaya contoh).
- Prosedur perakitan & pengembangan ditulis bernomor (1,2,3 + a,b,c).
- Pengujian fungsional pakai format tabel modul–pengujian–ekspektasi–hasil.
- Isi tetap lengkap & sesuai scope MQTT+CoAP; semua hasil yang belum ada tetap `[ISI SETELAH PENGUJIAN]`.
