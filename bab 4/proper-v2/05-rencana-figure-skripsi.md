# Rencana Figure untuk Skripsi (biar tidak kerja dua kali) — v2

> Daftar figure yang perlu ada di BAB IV, meniru GAYA penyajian [R2] (figure yang kamu kirim)
> dan jurnal lain. Untuk tiap figure: sumber datanya dari mana + status.
> Tujuan: saat menulis, semua gambar sudah jelas asal-usulnya → tidak mengulang pengujian.

> Catatan kepatuhan: figure [R2]/[M9]/[M-TL] hanya REFERENSI GAYA. Figure skripsi dibuat
> sendiri dari DATA & SISTEM milik penelitian ini. Jangan menyalin gambar sumber.

---

## A. Figure ARSITEKTUR & ALUR (diagram — bisa dibuat kapan saja)

| Fig | Isi | Acuan gaya | Sumber/Status |
|-----|-----|------------|---------------|
| A1 | Arsitektur sistem (ESP32 → WiFi → laptop broker/CoAP → dashboard) | [R2] Fig.1,4 | sudah ada draf: `4.1.1-arsitektur-sistem.md`, `diagrams/arsitektur-sistem.drawio` ✅ |
| A2 | Alur MQTT (publish/subscribe + Mosquitto) | [R2] Fig.4 | buat dari setup MQTT ✅ data ada |
| A3 | Alur CoAP (PUT/response + aiocoap) | [R2] Fig.1 | buat dari setup CoAP ✅ data ada |
| A4 | Wiring/pin mapping sensor (Flow1/2, Pressure, INA219) | — | `diagrams/wiring-pinmapping.drawio` ✅ |
| A5 | Diagram pipa deteksi kebocoran (galon→Flow1→cabang→Flow2→pressure) | [M9] Fig.3 | PERLU dibuat (susunan sudah fix) ⏳ |

## B. Figure HASIL UJI KINERJA (DATA SUDAH ADA ✅ — tinggal bikin grafik)

| Fig | Isi | Acuan gaya | Sumber data |
|-----|-----|------------|-------------|
| B1 | Latency MQTT vs CoAP per payload (bar/line) | [R2] Fig.16 | `hasil-pengujian-kinerja.md` ✅ |
| B2 | Throughput MQTT vs CoAP per payload (bps) | [R2] Fig.13 | `hasil-pengujian-throughput.md` ✅ |
| B3 | Throughput msg/s per iterasi (bar) | [R2] Fig.10,12 | `hasil-pengujian-throughput.md` ✅ |
| B4 | Packet Delivery Rate (%) MQTT vs CoAP | [R2] Fig.14 | `hasil-pengujian-kinerja.md` / reliability ✅ |
| B5 | Packet Loss Rate MQTT vs CoAP | [R2] Fig.15 | turunan dari success rate ✅ |

> B1–B5: datanya SUDAH lengkap & real. Tinggal plot di Excel/Sheets. Tidak perlu uji ulang.

## C. Figure DETEKSI KEBOCORAN (DATA BELUM lengkap — dari uji galon-penuh) ⏳

| Fig | Isi | Acuan gaya | Sumber data |
|-----|-----|------------|-------------|
| C1 | Grafik flow1 & flow2 vs waktu — NORMAL (dua garis berdekatan) | [M9] Fig.12 | CSV uji NORMAL ⏳ |
| C2 | Grafik flow1 & flow2 vs waktu — BOCOR (flow1 ≫ flow2) | [M9] Fig.13 | CSV uji BOCOR ⏳ |
| C3 | Confusion matrix NORMAL/BOCOR | [M-TL] Fig.13 | hasil evaluasi ⏳ |
| C4 | Bar chart metrik (Accuracy/Precision/Recall/F1) | [M-TL] Tabel 6/7 | hasil evaluasi ⏳ |
| C5 | Screenshot dashboard NORMAL (hijau) & BOCOR (merah) | [M9] Fig.15 | screenshot lab ⏳ |

## D. Figure ENERGY (DATA BELUM — setelah pasang INA219 seri) ⏳

| Fig | Isi | Acuan gaya | Sumber data |
|-----|-----|------------|-------------|
| D1 | Konsumsi daya (mW) MQTT vs CoAP per payload | [M8] Jara Ochoa | uji INA219 seri ⏳ |
| D2 | Grafik arus/tegangan saat transmisi | [M16] INA219 datasheet | uji INA219 seri ⏳ |

---

## Prioritas pembuatan figure (saran)
1. **Sekarang bisa:** B1–B5 (data kinerja sudah ada) + A1–A4 (diagram). → langsung isi BAB IV.
2. **Setelah uji galon-penuh:** C1–C5 (deteksi kebocoran).
3. **Setelah pasang INA219:** D1–D2 (energy).

## Alat bantu bikin grafik
- Excel / Google Sheets (dari CSV) — paling cepat, cukup untuk skripsi.
- (opsional) Python matplotlib bila ingin grafik lebih rapi/seragam.
