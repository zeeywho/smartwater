# 📌 STATUS & KEPUTUSAN PENTING — acuan lanjutan (handover)

> Ringkasan kondisi terkini + keputusan yang sudah diambil, agar sesi berikutnya langsung nyambung.
> Prinsip dosen: angka = PERSIS dari Serial/CSV, TIDAK boleh dikarang.

## Branch & PR
- Branch kerja: `docs/rancangan-pengujian-berujukan` → semua commit masuk **PR #3**.

## Judul skripsi (fokus)
"Analisis Kinerja Protokol Komunikasi IoT (MQTT & CoAP) pada Sistem Smart Water Usage untuk
Deteksi Kebocoran Pipa." → FOKUS UTAMA = perbandingan MQTT vs CoAP. Deteksi = konteks aplikasi.

---

## ✅ SUDAH SELESAI (data real, di PR #3)
- Uji kinerja protokol: LATENCY, PACKET LOSS, THROUGHPUT, RELIABILITY.
  - File: `hasil-pengujian-kinerja.md`, `-throughput.md`, `-reliability.md`.
  - ⚠️ DICATAT: diukur **TANPA AIR**, pakai **payload sintetis** (16/64/256 B). Lihat ISU di bawah.
- Setup penuh TERBUKTI jalan: MQTT ✅, CoAP ✅, jembatan CoAP→dashboard ✅
  (`coap/coap_server_bridge.py`), dashboard Node-RED ✅, logging CSV otomatis ✅.
- Deteksi kebocoran: FUNGSIONAL terbukti (keran buka → BOCOR; tutup → NORMAL; pressure drop).
  Firmware final: `firmware/deteksi-kebocoran-v2.ino` (logika NORMAL/BOCOR + filter anti-noise).

---

## 🚨 ISU BESAR #1 — "DATA LAPANGAN" (prioritas, harus ke dosen)
- Ketentuan BAB 4 kampus (poin 4.3): simulasi harus merepresentasikan KONDISI NYATA dengan
  DATA LAPANGAN.
- Kenyataan: data kinerja protokol diukur TANPA AIR (payload sintetis) → ADA GAP.
- **KEPUTUSAN (disepakati dgn user): pakai OPSI B** = ULANG uji kinerja protokol SAAT SISTEM
  BEROPERASI NYATA (air mengalir, sistem deteksi aktif), sehingga payload = data sensor REAL.
  Ini yang paling sesuai ketentuan "data lapangan".
- TODO: rancang ulang uji kinerja (latency/throughput/loss/reliability) dengan kondisi air
  mengalir. Metode statistik tetap mengacu [R1] (N, cold transaction, dll) — hanya payload
  yang jadi data sensor real. KONFIRMASI DOSEN soal detail.

## 🚨 ISU #2 — METODE DETEKSI
- Konsep "selisih Flow1−Flow2" TIDAK cocok dgn alat (pipa ujung TERTUTUP → saat NORMAL air
  tak mengalir → flow 0; metode selisih butuh aliran seri kontinu).
- Lokalisasi titik TIDAK ANDAL: GPIO27 & GPIO26 unit ESP32 ini tak baca pulsa (Flow2 dipindah
  ke GPIO33); Flow2 under-read pada debit gravitasi rendah; pin kadang noise (>30 L/min = pulsa
  hantu, sudah difilter di kode).
- **KEPUTUSAN: deteksi BINER (NORMAL vs BOCOR), tanpa lokalisasi titik, tanpa selisih.**
  Acceptable untuk skripsi ASAL BAB 1-3 KONSISTEN (lihat di bawah).

## 🚨 ISU #3 — BAB 1-3 MASIH ADA LoRa (belum direvisi)
- Scope final: MQTT & CoAP SAJA (LoRa DIHAPUS). BAB 1-3 user masih memuat LoRa → WAJIB revisi.
- Saat revisi BAB 1-3: selaraskan rumusan masalah & tujuan DETEKSI dengan metode BINER
  (jangan janjikan "lokalisasi titik" atau "selisih debit"). Hapus LoRa.

---

## HARDWARE (kondisi terkini)
- Pin: Flow1=GPIO25, Flow2=GPIO33 (GPIO27 & 26 rusak/tak baca pulsa di unit ini), Pressure=GPIO34.
- INA219 sering "not detected" (koneksi longgar) — perlu dirapikan sebelum uji ENERGY.
- Pipa SERI + ujung TERTUTUP (pressure sensor nutup ujung, bukan outlet). User TIDAK MAU bongkar pipa.
- YF-S201 spec maks 30 L/min → pembacaan >35 = noise (sudah difilter di kode).
- Debit galon gravitasi LEMAH → Flow2 (hilir) sering under-read. Hotspot "zee" TIDAK stabil
  (sering rc=-2 / exceeded timeout) → dekatkan ESP32 ke HP, HP jangan sleep.

## SETUP JARINGAN
- Hotspot "zee" pwd "satuduatiga"; laptop 172.20.10.3; ESP32 172.20.10.2.
- Broker Mosquitto :1883 (`C:\mqtt\mosquitto.conf`, listener 1883 0.0.0.0, allow_anonymous).
- CoAP: `python coap_server_bridge.py` (aiocoap :5683 + re-publish ke MQTT topik smartwater/data-coap).
- Node-RED :1880 (dashboard + csv logger ke `C:\Users\HP\Documents\smartwater\log-mqtt.csv`).
- `mosquitto_sub -h localhost -t smartwater/data -v` untuk cek data.

---

## SISA PENGUJIAN (urutan)
1. [ ] **ULANG uji kinerja protokol OPSI B** (air mengalir, payload data sensor real) — PRIORITAS,
       konfirmasi dosen dulu soal metode.
2. [ ] **Deteksi kebocoran biner** — uji NORMAL (keran tutup) ×10 + BOCOR (keran buka) ×10,
       pakai `deteksi-kebocoran-v2.ino`, hitung akurasi + confusion matrix. Screenshot dashboard.
3. [ ] **Energy (INA219 seri)** — belum ada kode `uji-energy.ino`; INA219 harus dipasang seri &
       dirapikan dulu. Dosen sudah setuju energy masuk scope.
4. [ ] Finalisasi BAB IV + revisi BAB 1-3 (hapus LoRa, selaraskan metode deteksi).

## FILE PENTING di repo
- Firmware deteksi final: `firmware/deteksi-kebocoran-v2.ino`
- Metodologi deteksi per-titik (arsip, konsep diubah ke biner): `proper-v2/01b-...`
- Lembar uji: `proper-v2/04b-lembar-pengujian-PERTITIK.md`
- Panduan uji final + setup: `proper-v2/06-PANDUAN-UJI-FINAL.md`
- Narasi BAB IV kinerja (data sintetis — perlu ditinjau ulang bila Opsi B dijalankan):
  `4.6-4.7-draft-kinerja.md`
