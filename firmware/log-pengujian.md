# Log Pengujian Hardware — Smart Water Usage (MQTT + CoAP)

> Catatan: log ini mencatat hasil pengujian NYATA, bukan karangan. Dipakai sebagai bahan BAB IV (4.2 Pengujian Sensor).

## Komponen terkonfirmasi (dari pembelian)
- ESP32: **DOIT DevKit V1, 38 pin**
- INA219: **CJMCU-219 (versi V329)**
- Flow sensor: **YF-S201 × 2**
- Pressure sensor: **WISNER WPT-83G, 0–12 Bar**
- Regulator 5V: 7805 (×2) + MP1584 (opsi) — rencana final: **adaptor 5V 2A fixed (akan dibeli)**
- Adaptor lama: 12V 1A (ZK-1210) — tidak dipakai di scope baru
- LoRa module: **DIHAPUS dari scope** (sudah dilepas dari breadboard)

## Catatan upload ESP32
- Board DOIT V1 TIDAK auto-masuk download mode.
- Solusi: tekan & tahan tombol **BOOT** saat muncul "Connecting...", lepas setelah "Writing".
- Upload speed stabil di 115200 jika perlu.

---

## STEP C — Pengujian INA219 ✅ LULUS
- **Tanggal:** 1 Oktober 2026
- **Tujuan:** konfirmasi komunikasi I2C (sensor terdeteksi atau tidak)
- **Wiring:** Vcc→3V3, Gnd→GND, SDA→GPIO21, SCL→GPIO22; Vin+/Vin− dibiarkan kosong
- **Penempatan:** INA219 melintang di breadboard, deretan pin bawah mulai Vcc di baris 16 kolom H
- **Hasil Serial Monitor (115200 baud):**
  ```
  BERHASIL: INA219 TERDETEKSI (komunikasi I2C OK).
  Bus Voltage:   0.00 V
  Current:       0.00–0.20 mA
  Power:         0.00 mW
  ```
- **Kesimpulan:** INA219 terdeteksi, komunikasi I2C berfungsi. Nilai 0 wajar karena Vin+/Vin− belum terhubung beban. **Wiring benar, tidak perlu disolder.**
- **Status:** ✅ LULUS

### Catatan lanjutan INA219
- Pengukuran konsumsi daya sebenarnya (parameter *energy consumption* BAB IV) dilakukan nanti dengan memasang Vin+/Vin− secara seri pada jalur daya ESP32 saat transmisi MQTT/CoAP.

---

---

## STEP 2 — Pengujian Wi-Fi ✅ LULUS
- **Tanggal:** 1 Oktober 2026
- **Tujuan:** konfirmasi ESP32 dapat terhubung ke jaringan Wi-Fi
- **Jaringan:** hotspot (2.4GHz), SSID "zee"
- **Hasil Serial Monitor (115200 baud):**
  ```
  BERHASIL: ESP32 TERHUBUNG ke Wi-Fi!
  IP Address ESP32: 172.20.10.2
  RSSI: -35 dBm (sangat kuat)
  Status: TERHUBUNG (konsisten, tidak terputus)
  ```
- **Kesimpulan:** ESP32 berhasil konek Wi-Fi, dapat IP, sinyal sangat kuat (−35 dBm), koneksi stabil.
- **Catatan:** jaringan via hotspot HP (range 172.20.10.x). Saat implementasi MQTT/CoAP, laptop (broker/server) HARUS di hotspot yang sama agar bisa komunikasi dengan ESP32.
- **Status:** ✅ LULUS

---

---

## STEP 4 — Pengujian Flow Sensor 1 (YF-S201) ✅ LULUS
- **Tanggal:** 1 Oktober 2026
- **Model terkonfirmasi:** YF-S201, working range 1–30 L/min, ≤1.75 MPa
- **Wiring FINAL:** VCC(merah)→rail+ , GND(hitam)→rail− , SIGNAL(kuning)→**GPIO25 LANGSUNG** (tanpa divider)
  - pinMode INPUT_PULLUP, interrupt CHANGE
  - Catatan warna: urutan posisi kabel berubah di sepanjang kabel; fungsi mengikuti WARNA.
- **Hasil Serial Monitor (115200) — pembacaan ASLI:**
  ```
  Diam (tuas tertutup): Pulsa 0, Level pin 1 (stabil, tanpa noise)
  Ada aliran (tuas dibuka): Pulsa 178, 161, 156... Level pin berubah 1<->0
  Berhenti: Pulsa kembali 0
  ```
- **Kesimpulan:** Pulsa merespons aliran nyata. Level pin berubah 1<->0 = sinyal asli masuk. Diam = 0 (tidak ada noise). VALID.
- **Status:** ✅ LULUS (deteksi pulsa; kalibrasi ke L/min menyusul)

## STEP 5 — Pengujian Flow Sensor 2 (YF-S201) ✅ LULUS
- **Tanggal:** 1 Oktober 2026
- **Wiring:** VCC(merah)→rail+ , GND(hitam)→rail− , SIGNAL(kuning)→GPIO27 LANGSUNG (tanpa divider)
  - pinMode INPUT_PULLUP, interrupt CHANGE (sama seperti Flow 1)
- **Hasil Serial Monitor (115200):**
  ```
  Diam: Pulsa 0, Level 1 (bersih)
  Ada aliran: Pulsa 175, 206, Level berubah 0<->1
  Berhenti: Pulsa 0
  ```
- **Kesimpulan:** Pembacaan asli, merespons aliran, tanpa noise. Pendekatan sama dengan Flow 1 (GPIO ber-pull-up langsung).
- **Status:** ✅ LULUS

## STEP 6 — Pengujian Pressure Sensor (WISNER WPT-83G) ✅ LULUS (fungsi)
- **Tanggal:** 1 Oktober 2026
- **Spesifikasi (dari label):** Supply 5–12VDC, Output 0.5–4.5V, Range 0–12 Bar
- **Warna kabel (dari label):** Supply=merah, GND=hitam, Output=kuning
- **Wiring:** Supply(merah)→rail+ , GND(hitam)→rail− , Output(kuning)→voltage divider 20K/10K→GPIO34 (ADC)
  - Divider DIPERLUKAN di sini (beda dari flow) karena pressure sensor mendorong tegangan 0.5–4.5V > 3.3V.
- **Hasil Serial Monitor (115200):**
  ```
  ADC berubah halus mengikuti tekanan: 2430, 2112, 1773 ... turun ke 0 ... naik lagi 139, 274 ... 2337
  ```
- **Kesimpulan:** ADC merespons perubahan tekanan dengan halus → sensor + wiring + divider bekerja.
- **ISU:** sesekali ADC=0 (drop) → diduga koneksi kabel SERABUT pressure sensor longgar (ujung belum ada pin/konektor). Perlu diperkuat (lilit ke jumper / solder pin header) sebelum pengukuran serius/kalibrasi.
- **Catatan:** perhitungan "~V sensor" (×3) hanya perkiraan kasar; kalibrasi presisi ADC→Bar menyusul (butuh referensi tekanan).
- **Status:** ✅ LULUS fungsi (deteksi perubahan tekanan; kalibrasi ke Bar + perkuat koneksi menyusul)

### Pelajaran penting dari proses debug STEP 4 (untuk BAB IV & pembahasan)
1. **GPIO34 tidak cocok untuk flow sensor** → input-only tanpa pull-up internal → pin floating → menghasilkan pulsa hantu/noise (~900-1900) meski tanpa aliran, bahkan saat kabel sinyal dicabut. Terbukti palsu.
2. **Voltage divider 20K/10K menghalangi sinyal** → YF-S201 open-collector butuh pull-up & menarik sinyal ke LOW; divider membuat sinyal tidak cukup kuat → Level pin stuck di 1, pulsa tidak terbaca.
3. **Solusi benar:** SIGNAL langsung ke GPIO ber-pull-up (GPIO25) + INPUT_PULLUP. Aman karena open-collector tidak mendorong >3.3V.
4. **Brownout** (`E BOD: Brownout detector triggered`) muncul karena power ESP32 drop → diatasi dengan ganti kabel USB yang lebih baik + port USB lain.
5. **Catatan proses:** tes-tes awal belum menghasilkan aliran karena TUAS/pemutar air pada pipa masih tertutup — tidak ada air/udara yang memutar turbin. Setelah tuas dibuka, pulsa asli baru terbaca.

---

## CATATAN PENTING (1 Okt 2026) — AKAR MASALAH: JUMPER RUSAK
Setelah rakit ulang bertahap dengan JUMPER BARU, semua sensor kembali terdeteksi:
- INA219 sempat tidak terdeteksi (nan, LED mati) di beberapa percobaan → ternyata JUMPER rusak. Ganti jumper → `0x40` terdeteksi, LED nyala.
- Flow2 "selalu 0" → kemungkinan besar juga jumper rusak.
- Ketidakstabilan parah (angka flow 100.000+ pulsa, V 8.2V, I inf/nan, harus ditekan) terjadi karena jumper aus + ESP32 kurang rekat di breadboard lama.
- SOLUSI: ganti ESP32 (ke unit yang lebih rekat) + ganti SEMUA jumper + rakit bertahap dengan verifikasi per komponen.
- PELAJARAN untuk BAB IV: kualitas jumper & kerapatan koneksi breadboard sangat kritis; sebelum pengambilan data serius sebaiknya koneksi dipermanenkan (solder/protoboard).

## STEP 7 — Pengujian Gabungan Semua Sensor ✅ LULUS
- **Tanggal:** 1 Oktober 2026 (setelah rakit ulang bertahap + jumper baru)
- **Tujuan:** membaca Flow1 (GPIO25), Flow2 (GPIO27), Pressure (GPIO34 ADC), INA219 (I2C) sekaligus; preview JSON.
- **Hasil Serial Monitor (115200):**
  ```
  Diam: Flow1=0, Flow2=0
  Dialiri: Flow1 = 213, 527, 249 pulsa | Flow2 = 85, 61 pulsa (keduanya merespons)
  Pressure: merespons (417 -> 1104 -> 1557)
  INA219: terdeteksi (tidak nan)
  JSON tercetak lengkap tiap detik
  ```
- **Kesimpulan:** keempat sensor terbaca BERSAMAAN, stabil, tanpa angka kacau, tanpa perlu ditekan. Format JSON siap dipakai MQTT & CoAP.
- **Temuan relevan:** Flow1 konsisten > Flow2 (mis. 213 vs 85) karena air berkurang via T-junction → dasar deteksi kebocoran (selisih Flow1-Flow2).
- **Catatan:**
  - Flow2 yang sebelumnya "selalu 0" ternyata SEHAT; penyebab lama = jumper rusak.
  - Konversi L/min (pulsa/7.5) masih PRELIMINARY, belum kalibrasi.
  - INA219 bus voltage sesekali loncat (mis. 8.10V) saat Vin+/Vin- KOSONG (input mengambang) — wajar; akan stabil setelah Vin dipasang seri untuk ukur energy.
  - Pressure sesekali ADC=0 (kabel serabut) — perlu diperkuat sebelum ambil data serius.
- **Status:** ✅ LULUS

---

## Progres keseluruhan
| Step | Komponen | Status |
|------|----------|--------|
| 1 | ESP32 upload + Serial | ✅ LULUS (terbukti saat tes INA219) |
| 2 | Wi-Fi | ✅ LULUS |
| 3 | INA219 | ✅ LULUS |
| 4 | Flow Sensor 1 | ✅ LULUS (GPIO25, tanpa divider, pembacaan asli) |
| 5 | Flow Sensor 2 | ✅ LULUS (GPIO27, tanpa divider, pembacaan asli) |
| 6 | Pressure Sensor | ✅ LULUS fungsi (GPIO34 via divider; ⚠️ koneksi serabut kadang drop ke ADC 0 — perlu diperkuat) |
| 7 | Gabungan | ✅ LULUS (keempat sensor terbaca bersamaan, stabil) |
| 8 | MQTT | ✅ BERHASIL (ESP32 publish, broker Mosquitto terima puluhan pesan) |
| 9 | CoAP | ✅ BERHASIL (ESP32 PUT, CoAP server terima data + response OK) |
| 10 | Dashboard | ⏭️ berikutnya |

## STEP 9 — Implementasi CoAP ✅ BERHASIL
- **Tanggal:** 16 Oktober 2026
- **Setup:** CoAP server Python (aiocoap 0.4.17) di laptop, resource `/sensor`, port 5683 UDP. Jaringan: Wi-Fi rumah (192.168.1.x — lebih stabil dari hotspot HP). Library ESP32: coap-simple-library (method `put`).
- **Hasil:** Server menerima data via PUT (`[CoAP] Diterima (PUT): {...}`); ESP32 dapat `Response dari server: OK`. Pressure_adc berubah = data sensor nyata terkirim.
- **Isu & solusi:**
  - `coap.post()` error (library tidak punya post) → ganti ke `coap.put()`.
  - "Method not allowed" → server hanya punya render_post; ESP32 kirim PUT → tambah `render_put` di server Python.
  - aiocoap tidak bisa bind `0.0.0.0` di Windows → bind ke IP laptop spesifik (auto-detect via socket).
  - Jaringan: pindah dari hotspot HP ke Wi-Fi rumah (192.168.1.x) agar lebih stabil & IP konsisten.
- **Catatan:** `current: nan` (INA219 koneksi longgar lagi) — perlu dirapikan sebelum ukur energy. Data flow 0 (tidak ada aliran air saat tes). Payload JSON IDENTIK dengan MQTT (penting untuk perbandingan adil).
- **Status:** ✅ BERHASIL (pipeline CoAP end-to-end terbukti)

### 🏆 MILESTONE: MQTT & CoAP KEDUANYA JALAN — siap tahap perbandingan kinerja (parameter: latency, throughput, packet loss, energy, reliability).

## STEP 8 — Implementasi MQTT ✅ BERHASIL
- **Tanggal:** 16 Oktober 2026
- **Setup:** Broker Mosquitto 2.1.2 di laptop Windows (config C:\mqtt\mosquitto.conf: `listener 1883 0.0.0.0` + `allow_anonymous true`). Jaringan: hotspot HP "zee". ESP32 publish JSON ke topik `smartwater/data`. Library: PubSubClient.
- **Hasil:** Broker menerima puluhan pesan PUBLISH (73-77 bytes) dari ESP32_SmartWater; PINGREQ/PINGRESP (keepalive) jalan.
- **Isu & solusi:**
  - IP hotspot HP dinamis → IP laptop berubah (jadi .3) → betulkan `mqtt_server` di kode. ESP32 dapat .2.
  - Putus-nyambung: "session taken over" (client ID sama saat reconnect) & "exceeded timeout" (k15, Wi-Fi goyang) → solusi: client ID unik (random), keepalive 60s, Wi-Fi auto-reconnect.
  - Perilaku koneksi ini relevan sebagai DATA untuk parameter reliability/packet loss (MQTT vs CoAP).
- **Catatan:** data sensor 0 saat tes (tidak ada aliran air); perlu tes ulang dengan air mengalir untuk lihat nilai nyata.
- **Verifikasi end-to-end:** subscriber `mosquitto_sub -h localhost -t smartwater/data -v` menerima data JSON mengalir dari ESP32 (pressure_adc berubah 0->461->368->469 = data sensor nyata terkirim). Pipeline lengkap: ESP32 publish -> broker -> subscriber -> data terlihat.
- **Pelajaran koneksi:** subscriber harus tuju IP broker (laptop), BUKAN IP ESP32. Paling aman pakai `localhost` karena subscriber & broker di mesin sama. IP ESP32=.2, laptop=.3 (hotspot dinamis).
- **Status:** ✅ TUNTAS (publish + subscribe + data nyata terverifikasi)

### 🎉 TAHAP HARDWARE SELESAI — semua sensor terbukti bekerja (individual + gabungan).
### Catatan sebelum tahap data serius: pertimbangkan permanenkan koneksi (solder/protoboard) agar stabil berjam-jam saat pengujian kinerja MQTT vs CoAP.

---

## STEP 11 — Pengujian Kinerja MQTT vs CoAP ✅ SELESAI (6 kombinasi)
- **Metode:** mengacu [R1] Kirilov & Chikurtev 2026 — cold transaction, 30 iterasi/kombinasi,
  jeda 4000 ms, payload 16/64/256 byte, MQTT QoS0 (+echo RTT), CoAP CON.
- **Jaringan:** hotspot HP "zee" (2.4 GHz); laptop 172.20.10.3; ESP32 172.20.10.2.
- **Hasil ringkas (MEDIAN latency, success rate) — CoAP 16B sudah di-run ulang (benar 16B):**
  | Protokol | 16 B | 64 B | 256 B |
  |----------|------|------|-------|
  | CoAP | 11.0 ms (100%) | 11.0 ms (100%) | 12.0 ms (96.67%) |
  | MQTT | 32.0 ms (96.67%) | 19.0 ms (96.67%) | 26.0 ms (96.67%) |
  (Median dipakai sbg nilai utama krn tahan outlier. Statistik lengkap: hasil-pengujian-kinerja.md)
- **Data mentah lengkap:** `bab 4/hasil-pengujian-kinerja.md`.
- **Isu & solusi (temuan teknis):**
  - Payload 256 B TIMEOUT di kedua protokol → buffer library kekecilan.
    CoAP: naikkan `BUF_MAX_SIZE` coap-simple → 512. MQTT: `client.setBufferSize(1024)`.
  - Hotspot HP kurang stabil → sesekali TIMEOUT/CONNECT_FAIL (gangguan jaringan,
    bukan karakteristik protokol); perlu reset bersih (cabut USB, restart hotspot,
    reconnect Wi-Fi, restart broker+echo).
  - MQTT latency diukur RTT via echo (laptop echo smartwater/data -> smartwater/ack)
    agar setara CoAP CON (ada balasan).
- **Catatan metodologi (perlu konfirmasi dosen):**
  - CoAP 16 B mean ketarik outlier cold-start → pertimbangkan ulang agar sebanding.
  - Perlakuan TIMEOUT/CONNECT_FAIL harus SAMA untuk kedua protokol (fair).
  - Gunakan MEDIAN selain mean (tahan outlier).
- **Status:** ✅ SELESAI (180 transaksi; tinggal hitung median/std & isi tabel 4.7).

### 🏆 MILESTONE: Pengujian kinerja MQTT vs CoAP (inti skripsi) TUNTAS.
### Berikutnya: hitung statistik deskriptif → isi BAB IV 4.7; lalu pengujian deteksi kebocoran (butuh air).
