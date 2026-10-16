# Rencana Pengujian Sistem — Smart Water Usage (MQTT vs CoAP)

> Status: RENCANA (belum dieksekusi). Hasil akan diisi setelah pengujian → tandai `[ISI SETELAH PENGUJIAN]`.
> Scope: MQTT + CoAP saja (LoRa dihapus). 1 node ESP32, gateway laptop, jaringan Wi-Fi.
> Prinsip: MQTT & CoAP diuji dengan kondisi & data IDENTIK agar perbandingan adil.

---

## 0. Penyesuaian dari proposal lama (PENTING)

Skenario di proposal lama (BAB 1.3.4) sebagian tidak relevan setelah LoRa dihapus:
| Skenario lama | Status | Alasan |
|---------------|--------|--------|
| Variasi jarak 10–500 m | ❌ DIHAPUS/diganti | Relevan untuk LoRa; MQTT/CoAP via Wi-Fi rumah tidak diuji jarak jauh |
| Variasi beban (banyak node) | ⚠️ DISESUAIKAN | Hanya 1 node ESP32; bisa disimulasikan dgn menaikkan frekuensi kirim |
| Variasi frekuensi kirim (1/5/10 s) | ✅ DIPAKAI | Masih relevan |
| Variasi kondisi jaringan | ✅ DIPAKAI (versi Wi-Fi) | Normal vs terbebani/interferensi |

---

## A. RENCANA PENGUJIAN KINERJA PROTOKOL (inti skripsi)

### A.1 Parameter yang diukur (5)
| Parameter | Definisi | Cara ukur (rencana) | Satuan |
|-----------|----------|---------------------|--------|
| End-to-end latency | waktu kirim (ESP32) → diterima (laptop) | ESP32 sertakan timestamp kirim; laptop catat waktu terima; selisih | ms |
| Throughput | jumlah data berhasil per satuan waktu | hitung pesan/byte diterima per detik di laptop | msg/s atau kbps |
| Packet loss | % paket gagal sampai | (dikirim − diterima) / dikirim × 100 | % |
| Energy consumption | konsumsi daya ESP32 saat transmisi | INA219 ukur arus×tegangan saat kirim (Vin+/Vin− dipasang seri ke jalur daya ESP32) | mWh / mW |
| Reliability | konsistensi keberhasilan kirim | % keberhasilan transmisi selama durasi uji | % |

> Catatan latency: ESP32 & laptop idealnya tersinkron waktu (NTP) atau pakai metode round-trip. Metode final ditetapkan saat implementasi.

### A.2 Variabel yang dikontrol (harus SAMA untuk MQTT & CoAP)
- Payload JSON identik (isi & ukuran sama)
- Jumlah pesan uji sama (mis. 1000 pesan per skenario)
- Jaringan Wi-Fi sama, posisi perangkat sama
- Interval kirim sama per skenario
- Durasi uji sama

### A.3 Skenario pengujian kinerja
| Skenario | Variasi | MQTT | CoAP |
|----------|---------|------|------|
| S1 — Frekuensi kirim | interval 1 s | ✅ | ✅ |
|  | interval 5 s | ✅ | ✅ |
|  | interval 10 s | ✅ | ✅ |
| S2 — Kondisi jaringan | normal (idle) | ✅ | ✅ |
|  | jaringan terbebani (ada traffic lain) | ✅ | ✅ |
| S3 — QoS/mode | MQTT QoS 0 vs QoS 1; CoAP NON vs CON | ✅ | ✅ |
| S4 — Durasi/stabilitas | kirim kontinu (mis. 30 menit) | ✅ | ✅ |

> Tiap skenario dijalankan untuk MQTT dan CoAP dengan kondisi identik, diulang beberapa kali (mis. 3–5×) untuk rata-rata.

### A.4 Jumlah pengulangan & statistik
- Tiap kombinasi diulang N kali (mis. 5×) → ambil mean + standar deviasi.
- Statistik deskriptif (mean, min, max, std) untuk tiap parameter.

---

## B. RENCANA PENGUJIAN DETEKSI KEBOCORAN

### B.1 Prinsip deteksi
- 2 flow sensor (masuk & keluar) + 1 pressure sensor.
- Indikasi bocor: selisih Flow1 − Flow2 signifikan (air "hilang" di antara keduanya) DAN/ATAU penurunan tekanan.
- Threshold ditetapkan dari data kondisi normal (kalibrasi).

### B.2 Skenario
| Skenario | Kondisi | Ekspektasi |
|----------|---------|------------|
| Normal | semua air mengalir lewat kedua sensor, tidak ada cabang bocor | Flow1 ≈ Flow2, tekanan stabil → status NORMAL |
| Kebocoran | buka cabang/T-junction (air keluar di tengah) | Flow1 > Flow2, tekanan turun → status BOCOR |

### B.3 Cara simulasi kebocoran (fisik)
- Pipa prototipe punya T-junction di tengah (antara Flow1 dan Flow2).
- Buka keran/cabang di T-junction → simulasikan air bocor keluar sistem.
- Variasikan besar bukaan (bocor kecil vs besar) bila memungkinkan.

### B.4 Yang dicatat
- Nilai Flow1, Flow2, selisih, Pressure pada tiap kondisi.
- Apakah sistem benar mengidentifikasi NORMAL vs BOCOR (akurasi deteksi).
- `[ISI SETELAH PENGUJIAN]`

---

## C. RENCANA PENGUJIAN FUNGSIONAL (BLACK BOX)

| No | Fitur | Input/Aksi | Output diharapkan | Status |
|----|-------|-----------|-------------------|--------|
| F1 | ESP32 upload & serial | upload program | Serial Monitor jalan | ✅ (sudah) |
| F2 | Wi-Fi connect | nyalakan | dapat IP | ✅ (sudah) |
| F3 | Baca INA219 | nyalakan | V/I/P terbaca | ✅ (sudah) |
| F4 | Baca Flow 1 | alirkan air | pulsa naik | ✅ (sudah) |
| F5 | Baca Flow 2 | alirkan air | pulsa naik | ✅ (sudah) |
| F6 | Baca Pressure | beri tekanan | ADC berubah | ✅ (sudah, fungsi) |
| F7 | Baca semua sensor sekaligus | nyalakan | semua tampil + JSON | ⏳ (STEP 7, belum) |
| F8 | Kirim via MQTT | publish | broker terima | ⏸️ belum |
| F9 | Kirim via CoAP | request | server terima | ⏸️ belum |
| F10 | Tampil di dashboard | kirim data | muncul di dashboard | ⏸️ belum |
| F11 | Deteksi kebocoran | simulasi bocor | status BOCOR muncul | ⏸️ belum |

---

## D. ALAT UKUR & LOGGING
- **Latency/throughput/loss:** script di laptop (Node-RED / Python) catat timestamp + hitung.
- **Energy:** INA219 (Vin+/Vin− dipasang seri ke jalur daya ESP32) → ukur arus saat transmisi.
- **Logging:** simpan ke CSV untuk analisis statistik (mean, std) & grafik.
- **Dashboard:** Node-RED (rencana) untuk real-time + status; opsional Grafana untuk grafik historis.

## E. URUTAN EKSEKUSI PENGUJIAN (setelah hardware & protokol siap)
1. Selesaikan STEP 7 (gabungan sensor) ← sedang menuju ini
2. Implementasi MQTT → uji unit (F8)
3. Implementasi CoAP → uji unit (F9)
4. Integrasi dashboard (F10)
5. Pengujian deteksi kebocoran (B) — kalibrasi threshold dulu
6. Pengujian kinerja (A) — jalankan semua skenario, log ke CSV
7. Analisis & perbandingan (statistik deskriptif) → BAB IV 4.7

## Catatan untuk dikonfirmasi ke pembimbing
- Skenario jarak (dari proposal lama) dihapus karena LoRa dihapus — perlu persetujuan.
- Metode ukur latency (NTP sync vs round-trip) — tentukan & justifikasi.
- Jumlah pengulangan (N) & jumlah pesan per uji — tentukan angkanya.
