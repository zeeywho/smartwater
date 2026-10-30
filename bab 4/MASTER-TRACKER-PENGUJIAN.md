# 📍 MASTER TRACKER PENGUJIAN — Smart Water Usage (MQTT vs CoAP)

> SATU halaman untuk melacak SELURUH progres pengujian dari awal sampai tuntas
> (end-to-end sesuai tujuan alat: deteksi kebocoran + perbandingan kinerja protokol).
> Centang `[x]` yang selesai. Update tiap habis nguji.
> Legenda: ✅ selesai · 🚧 sebagian · ⏳ belum · ❌ blok (butuh sesuatu)

---

## 🎯 TUJUAN AKHIR (definisi "TUNTAS")
Sistem terbukti **end-to-end**: sensor membaca → data dikirim via MQTT & CoAP →
tampil di dashboard → **mendeteksi kebocoran** (NORMAL vs BOCOR), DAN
**kinerja MQTT vs CoAP terukur & teranalisis** (5 parameter). Semua terdokumentasi di BAB IV.

---

## 📊 RINGKASAN PROGRES KESELURUHAN

```
Hardware & Sensor  ██████████ 100%  ✅ (STEP 1–7)
Komunikasi MQTT    ██████████ 100%  ✅ (STEP 8)
Komunikasi CoAP    ██████████ 100%  ✅ (STEP 9)
Dashboard (MQTT)   ███████░░░  70%  🚧 (STEP 10; jembatan CoAP belum)
Uji Kinerja        █████████░  90%  🚧 (latency+loss+throughput+reliability ✅; energy ⏳)
Deteksi Kebocoran  ░░░░░░░░░░   0%  ⏳ (butuh air)
Analisis & BAB IV  ██████░░░░  60%  🚧 (draft ada; lengkapi stlh uji sisa)
```

**POSISI TERAKHIR:** Uji kinerja protokol — **LATENCY, PACKET LOSS, THROUGHPUT, RELIABILITY
sudah SELESAI** (data asli, 3 dari kinerja + reliability tuntas). Tinggal ENERGY (butuh
INA219 seri) + deteksi kebocoran (butuh air).

**SISA MENUJU TUNTAS:** (1) Energy (pasang INA219 seri) — bisa di-drop bila dosen setuju,
(2) Deteksi kebocoran (pakai air), (3) Jembatan CoAP ke dashboard, (4) finalisasi BAB IV.

---

## ✅ BAGIAN 1 — HARDWARE & SENSOR (STEP 1–7) — SELESAI
| Step | Item | Status |
|------|------|--------|
| 1 | ESP32 upload + Serial | ✅ |
| 2 | Wi-Fi connect | ✅ |
| 3 | INA219 (I2C) terbaca | ✅ |
| 4 | Flow Sensor 1 (GPIO25) | ✅ |
| 5 | Flow Sensor 2 (GPIO27) | ✅ |
| 6 | Pressure (GPIO34 + divider) | ✅ (koneksi serabut perlu diperkuat) |
| 7 | Gabungan 4 sensor + JSON | ✅ |
> Bukti: `firmware/log-pengujian.md`.

---

## ✅ BAGIAN 2 — KOMUNIKASI PROTOKOL — SELESAI
| Step | Item | Status |
|------|------|--------|
| 8 | MQTT publish → broker Mosquitto | ✅ end-to-end terbukti |
| 9 | CoAP PUT → server aiocoap | ✅ end-to-end terbukti |
> Bukti: `firmware/setup-mqtt.md`, `setup-coap.md`.

---

## 🚧 BAGIAN 3 — DASHBOARD (STEP 10)
| Item | Status |
|------|--------|
| Dashboard Node-RED (gauge/chart/text) | ✅ |
| Data MQTT tampil real-time (gauge Pressure gerak) | ✅ |
| Parse JSON (fix nan/inf) | ✅ |
| Logika deteksi bocor (function selisih flow) | ✅ (logika siap; hasil nunggu air) |
| **Jembatan CoAP → dashboard (2 protokol)** | ⏳ BELUM (butuh alat; node coap-in / bridge) |
| Logging CSV | ⏳ opsional |
> Bukti: `firmware/setup-dashboard.md`, `dashboard/flow-dashboard.json`.

---

## 🚧 BAGIAN 4 — PENGUJIAN KINERJA PROTOKOL (INTI) — 5 PARAMETER

### 4a. Latency (End-to-End / RTT) — ✅ SELESAI
| Payload | MQTT (median) | CoAP (median) | Status |
|---------|---------------|---------------|--------|
| 16 B | 32.0 ms | 11.0 ms | ✅ |
| 64 B | 19.0 ms | 11.0 ms | ✅ |
| 256 B | 26.0 ms | 12.0 ms | ✅ |
> 180 transaksi, data asli. Detail: `hasil-pengujian-kinerja.md`.

### 4b. Packet Loss / Success Rate — ✅ SELESAI
| Payload | MQTT | CoAP |
|---------|------|------|
| 16 B | 96.67% | 100% |
| 64 B | 96.67% | 100% |
| 256 B | 96.67% | 96.67% |
> Diambil dari run yang sama (success rate). ✅

### 4c. Reliability — ✅ SELESAI (dari PDR)
| Item | Status |
|------|--------|
| PDR/success rate 6 kombinasi (latency + throughput) | ✅ dirangkum (`hasil-pengujian-reliability.md`) |
| Reliability: MQTT 96.67–100%, CoAP 98.89–100% | ✅ |
| Uji durasi panjang (mis. 30 menit kontinu) | ⏳ OPSIONAL (bila dosen minta) |

### 4d. Throughput — ✅ SELESAI (uji khusus, data baru)
| Payload | MQTT (bps) | CoAP (bps) |
|---------|-----------|-----------|
| 16 B | 3.812,94 | 12.283,21 |
| 64 B | 14.797,70 | 60.686,54 |
| 256 B | 52.932,25 | 214.183,37 |
> N=100 beruntun × 3 ulangan. CoAP ~3–4× lebih tinggi. Detail: `hasil-pengujian-throughput.md`

### 4e. Energy Consumption — ⏳ BELUM (BLOK)
| Item | Status |
|------|--------|
| Pasang INA219 SERI ke jalur daya ESP32 (Vin+/Vin−) | ❌ belum dipasang |
| Ukur arus×tegangan saat transmisi MQTT vs CoAP | ⏳ setelah INA219 seri |
| Rata-rata per protokol/payload | ⏳ |
> Rujukan metode energy: [R5]. Konfirmasi dosen apakah energy masuk scope final.

### 4f. Uji Kinerja PAYLOAD JSON SENSOR ASLI (STEP 13) — ⏳ KODE SIAP, run di AKHIR
> Pelengkap operasional nyata: payload = JSON sensor sistem (~75 B), bukan sintetis.
> Tujuan: kondisi nyata + validasi KORELASI dgn tren payload sintetis (penguat hasil).
> Dikerjakan SETELAH deteksi kebocoran end-to-end (urutan sesuai keputusan).
| Item | Status |
|------|--------|
| Kode latency JSON (MQTT+CoAP) | ✅ siap: `*-uji-latency-json.ino` |
| Kode throughput JSON (MQTT+CoAP) | ✅ siap: `*-uji-throughput-json.ino` |
| Run latency JSON (30 iterasi) | ⏳ di akhir |
| Run throughput JSON (100×3) | ⏳ di akhir |
| Lembar catat | ✅ Lembar 3C (`lembar-pengujian.md`) |
| Metode = SAMA dgn sintetis (hanya payload beda) | ✅ agar sebanding |

---

## ⏳ BAGIAN 5 — PENGUJIAN DETEKSI KEBOCORAN (BUTUH AIR)
| Item | Status |
|------|--------|
| Kalibrasi kondisi NORMAL (Flow1≈Flow2) → tentukan threshold | ⏳ |
| Skenario NORMAL (air lewat kedua sensor) | ⏳ |
| Skenario BOCOR (buka T-junction → Flow1>Flow2 + tekanan turun) | ⏳ |
| Validasi sistem tampilkan status NORMAL/BOCOR benar | ⏳ |
| Hitung akurasi deteksi | ⏳ |
> Lembar catat: `lembar-pengujian.md` (Lembar 2). Rujukan: [R-leak] YF-S201, flow in/out.

---

## ⏳ BAGIAN 6 — PENGUJIAN FUNGSIONAL (BLACK BOX) — lengkapi F7–F11
| No | Fitur | Status |
|----|-------|--------|
| F1–F6 | Sensor individual | ✅ |
| F7 | Baca semua sensor + JSON | ✅ |
| F8 | Kirim MQTT | ✅ |
| F9 | Request CoAP | ✅ |
| F10 | Tampil dashboard | 🚧 (MQTT ✅; CoAP ⏳) |
| F11 | Deteksi kebocoran | ⏳ (butuh air) |

---

## 🚧 BAGIAN 7 — ANALISIS & PENULISAN BAB IV
| Sub-bab | Status |
|---------|--------|
| 4.1 Implementasi sistem / arsitektur | ✅ draft |
| 4.1.3–4.5 (hardware, sensor, MQTT/CoAP, dashboard) | ✅ outline |
| 4.6.3.1 Latency | ✅ draft + tabel |
| 4.6.3.3 Packet Loss / Success Rate | ✅ draft |
| 4.6.3.2 Throughput | ⏳ |
| 4.6.3.4 Energy | ⏳ |
| 4.6.3.5 Reliability | 🚧 |
| 4.6.2 Deteksi kebocoran | ⏳ |
| 4.7 Analisis perbandingan | 🚧 (latency/loss ✅; throughput/energy ⏳) |
| 4.8 Pembahasan | 🚧 (temuan buffer & jaringan ✅; lengkapi) |
| 4.9 Ringkasan | ⏳ |

---

## 🗺️ ROADMAP SISA (urutan disarankan menuju TUNTAS)

> Urutan: selesaikan uji inti dulu, uji payload JSON asli (STEP 13) dikerjakan DI AKHIR
> setelah deteksi kebocoran (sesuai keputusan — biar fokus ke fungsi utama dulu).

1. [x] **Throughput** — SELESAI (uji khusus N=100, 6 kombinasi). → 4.6.3.2 & 4.7.2.
2. [x] **Reliability** — SELESAI (rangkum PDR dari latency+throughput). → 4.6.3.5 & 4.7.5.
3. [ ] **Deteksi kebocoran (FUNGSI UTAMA)** — alirkan air, kalibrasi normal, simulasi bocor →
       isi 4.3.5 lengkap, 4.5.3, 4.6.2, F11. ← PRIORITAS BERIKUTNYA
4. [ ] **Energy** — pasang INA219 seri → uji MQTT vs CoAP → 4.6.3.4 & 4.7.4.
       (atau konfirmasi dosen apakah di-drop dari scope)
5. [ ] **Jembatan CoAP ke dashboard** — node coap-in / bridge → 4.5.4, F10 (CoAP).
6. [ ] **Uji kinerja PAYLOAD JSON ASLI (STEP 13)** — kode SIAP; RUN DI AKHIR (setelah
       deteksi kebocoran) → pelengkap operasional nyata + validasi korelasi.
7. [ ] **(Opsional) Uji reliability durasi panjang** — bila dosen minta.
8. [ ] **Finalisasi BAB IV** — lengkapi 4.7, 4.8, 4.9; konfirmasi statistik; sitasi rujukan.
9. [ ] **Konfirmasi dosen:** energy in/out scope; payload JSON asli (tambah skenario);
       metode success-rate.

---

## 📎 Peta dokumen (biar cepat cari)
| Butuh | File |
|-------|------|
| Lembar catat saat uji | `bab 4/lembar-pengujian.md` |
| Metode + rujukan jurnal | `bab 4/rancangan-pengujian-berujukan.md` |
| Data latency/loss | `bab 4/hasil-pengujian-kinerja.md` |
| Data throughput | `bab 4/hasil-pengujian-throughput.md` |
| Data reliability (PDR) | `bab 4/hasil-pengujian-reliability.md` |
| Draft narasi 4.6–4.7 | `bab 4/4.6-4.7-draft-kinerja.md` |
| Outline 4.1.3–4.5 | `bab 4/4.1.3-4.5-outline.md` |
| Tutorial setup uji | `firmware/setup-pengujian-kinerja.md` |
| Log hardware/MQTT/CoAP | `firmware/log-pengujian.md`, `setup-*.md` |

---

_Terakhir diperbarui: latency+packet loss+throughput+reliability SELESAI. Kode uji payload
JSON asli (STEP 13) SIAP (run di akhir). Berikutnya: deteksi kebocoran (fungsi utama)._
