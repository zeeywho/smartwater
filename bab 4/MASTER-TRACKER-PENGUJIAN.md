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

> Fokus hari ini: selesaikan yang BISA tanpa nunggu, lalu yang butuh alat/air.

1. [ ] **Throughput** — hitung dari data latency+payload yang SUDAH ADA (tanpa alat).
       → langsung bisa isi 4.6.3.2 & 4.7.2.
2. [ ] **Energy** — pasang INA219 seri → uji ulang MQTT vs CoAP singkat → isi 4.6.3.4 & 4.7.4.
       (atau konfirmasi dosen apakah di-drop dari scope)
3. [ ] **Deteksi kebocoran** — alirkan air, kalibrasi normal, simulasi bocor →
       isi 4.3.5 lengkap, 4.5.3, 4.6.2, F11.
4. [ ] **Jembatan CoAP ke dashboard** — node coap-in / bridge → isi 4.5.4, F10 (CoAP).
5. [ ] **(Opsional) Uji reliability durasi panjang** — bila dosen minta.
6. [ ] **Finalisasi BAB IV** — lengkapi 4.7, 4.8, 4.9; konfirmasi statistik; sitasi rujukan.
7. [ ] **Konfirmasi dosen:** metode throughput, energy in/out scope, N=30 & payload, success-rate.

---

## 📎 Peta dokumen (biar cepat cari)
| Butuh | File |
|-------|------|
| Lembar catat saat uji | `bab 4/lembar-pengujian.md` |
| Metode + rujukan jurnal | `bab 4/rancangan-pengujian-berujukan.md` |
| Data & statistik kinerja | `bab 4/hasil-pengujian-kinerja.md` |
| Draft narasi 4.6–4.7 | `bab 4/4.6-4.7-draft-kinerja.md` |
| Outline 4.1.3–4.5 | `bab 4/4.1.3-4.5-outline.md` |
| Tutorial setup uji | `firmware/setup-pengujian-kinerja.md` |
| Log hardware/MQTT/CoAP | `firmware/log-pengujian.md`, `setup-*.md` |

---

_Terakhir diperbarui: setelah pengujian kinerja latency+success rate (6 kombinasi) selesai._
