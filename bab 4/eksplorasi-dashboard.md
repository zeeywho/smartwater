# Eksplorasi Platform Dashboard — Smart Water Usage (MQTT + CoAP)

> Tujuan: memilih platform monitoring yang memenuhi tujuan skripsi.
> Konteks: 1 ESP32 kirim data IDENTIK via MQTT & CoAP → laptop (gateway) → dashboard.
> Catatan sumber: dirangkum dari dokumentasi & artikel publik; konten diparafrasekan untuk kepatuhan lisensi.

## Kebutuhan khusus skripsi ini (kriteria penilaian)

Dashboard HARUS mendukung:
1. **Menerima data dari DUA protokol: MQTT dan CoAP** (ini pembeda utama vs proyek IoT biasa yang cuma MQTT)
2. Menampilkan data sensor real-time: Flow1, Flow2, Pressure, Voltage/Current/Power
3. Menampilkan **status deteksi kebocoran** (normal vs bocor)
4. Mendukung **perbandingan kinerja MQTT vs CoAP** (latency, throughput, packet loss, energy, reliability)
5. Menyimpan data historis (time-series) untuk analisis & grafik skripsi
6. Realistis untuk dikerjakan mahasiswa (tidak terlalu rumit)

> KUNCI: CoAP adalah tantangan. Banyak dashboard native mendukung MQTT, tapi CoAP TIDAK didukung langsung. Perlu "jembatan" (bridge).

---

## Opsi A — Node-RED (+ dashboard) ⭐ REKOMENDASI UTAMA

**Apa itu:** platform low-code berbasis flow (drag-and-drop) dari IBM/OpenJS. Jalan di laptop (Node.js).

**Kenapa cocok untuk skripsi ini:**
- ✅ **Bisa terima MQTT DAN CoAP dalam satu tempat** — ada node `mqtt in` (bawaan) dan node `node-red-contrib-coap` untuk CoAP. Ini jawaban masalah "dua protokol".
- ✅ Punya dashboard sendiri (`@flowfuse/node-red-dashboard` / Dashboard 2.0) → gauge, chart, text, dll.
- ✅ Bisa parsing JSON, hitung threshold deteksi kebocoran, kirim notifikasi (email/Telegram).
- ✅ Bisa catat timestamp untuk ukur latency (data masuk) → berguna untuk parameter kinerja.
- ✅ Low-code → ramah untuk mahasiswa (tanpa banyak koding web).
- ✅ Bisa forward ke database (InfluxDB) kalau mau grafik historis via Grafana.

**Kekurangan:**
- Visualisasi tidak secantik Grafana untuk time-series panjang.
- Perlu install Node.js + Node-RED di laptop.

**Peran ideal:** sebagai **middleware/jembatan** yang menerima MQTT & CoAP, proses, tampilkan dashboard. Bisa berdiri sendiri ATAU jadi gerbang ke Grafana.

---

## Opsi B — Grafana (+ InfluxDB) — untuk visualisasi & analisis kuat

**Apa itu:** platform visualisasi time-series profesional. Butuh sumber data (database) seperti InfluxDB.

**Kenapa cocok:**
- ✅ **Grafik time-series sangat bagus** → ideal untuk grafik perbandingan MQTT vs CoAP di skripsi (latency, throughput dari waktu ke waktu).
- ✅ Cocok untuk analisis historis & statistik deskriptif (mean, dll).
- ✅ Tampilan profesional → bagus untuk lampiran skripsi.

**Kekurangan / tantangan:**
- ❌ TIDAK bisa terima MQTT/CoAP langsung → butuh pipeline: data → (Node-RED / Telegraf) → InfluxDB → Grafana.
- ❌ CoAP tetap butuh jembatan (Node-RED) karena tidak ada input CoAP native.
- ❌ Setup lebih berat (InfluxDB + Grafana + bridge).

**Peran ideal:** lapisan **visualisasi & analisis** di atas Node-RED+InfluxDB. Bukan penerima protokol langsung.

---

## Opsi C — Blynk — paling mudah, tapi kurang cocok untuk riset ini

**Kenapa KURANG cocok:**
- ❌ Berbasis MQTT/library sendiri; **CoAP tidak didukung** → gagal memenuhi syarat "dua protokol".
- ❌ Kurang fleksibel untuk logging & analisis kinerja protokol.
- ✅ (Hanya) gampang & cepat untuk monitoring sederhana.

**Kesimpulan:** tidak disarankan sebagai platform utama karena CoAP tidak terdukung — padahal CoAP inti skripsi.

---

## Opsi D — ThingsBoard — kuat tapi berat

- ✅ Platform IoT lengkap, dashboard bagus, mendukung MQTT.
- ⚠️ CoAP didukung (ThingsBoard punya CoAP transport) → menarik secara teori.
- ❌ Setup berat, kurva belajar curam untuk mahasiswa; mungkin berlebihan untuk skala rumah tangga 1 node.
- Catatan: bisa jadi alternatif, tapi kompleksitas tinggi.

---

## REKOMENDASI untuk skripsi ini

### Pilihan 1 (paling direkomendasikan): Node-RED sebagai pusat
```
ESP32 ──MQTT──┐
              ├──► Node-RED (laptop) ──► Dashboard Node-RED (real-time + status bocor)
ESP32 ──CoAP──┘                     └──► (opsional) InfluxDB ──► Grafana (grafik analisis)
```
- Node-RED jadi **jembatan dua protokol** (MQTT + CoAP) → paling pas dengan tujuan skripsi.
- Dashboard Node-RED cukup untuk monitoring real-time + status kebocoran.
- Untuk grafik perbandingan kinerja yang "cantik", tambahkan InfluxDB + Grafana (opsional, kalau mau nilai plus).

### Pilihan 2 (kalau mau visual maksimal): Node-RED + InfluxDB + Grafana
- Node-RED terima MQTT & CoAP → simpan ke InfluxDB → Grafana untuk grafik.
- Lebih banyak setup, tapi hasil visual terbaik untuk analisis BAB IV.

---

## Catatan teknis penting soal CoAP (jangan sampai terlewat)

- MQTT: didukung luas (Node-RED, Grafana via plugin, dll).
- **CoAP: hampir tidak ada dashboard yang terima langsung.** Solusi umum:
  - Node-RED + `node-red-contrib-coap` (CoAP server/client node), ATAU
  - CoAP server terpisah di laptop (Python `aiocoap` / Node.js `node-coap`) → teruskan ke dashboard.
- Jadi apa pun pilihan dashboard, **Node-RED (atau skrip CoAP server) tetap dibutuhkan sebagai penampung CoAP.**

## Untuk keadilan perbandingan (penting metodologi)
- Data MQTT & CoAP harus identik (sudah direncanakan: 1 sumber ESP32, payload JSON sama).
- Pengukuran kinerja (latency, dll) sebaiknya dicatat terpisah per protokol — Node-RED bisa catat timestamp terima; latency = waktu terima − waktu kirim (ESP32 kirim timestamp).
- Pertimbangkan logging ke file/CSV untuk analisis statistik (mean, dsb) di luar dashboard.

## Keputusan: [MENUNGGU PERSETUJUAN SUPERVISOR / USER]
- Rekomendasi penulis (draft): **Node-RED sebagai pusat (jembatan MQTT+CoAP) + opsional Grafana untuk grafik analisis.**
- Platform final belum dikunci — konfirmasi dulu.
