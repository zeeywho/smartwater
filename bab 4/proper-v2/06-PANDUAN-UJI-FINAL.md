# 🏁 PANDUAN UJI FINAL — Smart Water Usage (sekali jalan → fokus menulis)

> Panduan TUNGGAL untuk menuntaskan sisa pengujian dalam satu sesi terencana, lalu fokus
> menulis BAB IV. Disusun agar: (1) semua komponen DIPASTIKAN tersambung & jalan DULU,
> (2) baru pengujian, (3) hasil langsung dipetakan ke outline resmi kampus.
>
> PRINSIP (arahan dosen): angka = PERSIS dari Serial Monitor / CSV, TIDAK dikarang.
> Metode & jumlah sampel = berujukan (lihat `01-metodologi-deteksi-kebocoran-berujukan.md`).
>
> Urutan disepakati: TAHAP 0 (setup & verifikasi) → A (deteksi kebocoran) → C (payload JSON)
> → B (energy) DIKERJAKAN NANTI (satu per satu, lihat §Tahap B).

---

## 🔧 TAHAP 0 — SETUP & VERIFIKASI SEMUA KOMPONEN (WAJIB sebelum uji)

> Tujuan: PASTIKAN sensor → ESP32 → MQTT/CoAP → dashboard semua OK. Jangan mulai uji
> sebelum SEMUA checklist di bawah tercentang. Ini mencegah uji ulang gara-gara 1 kabel lepas.

### 0.1 Jaringan
- [ ] Hotspot **"zee"** (2.4 GHz) nyala; laptop & ESP32 di jaringan yang SAMA.
- [ ] Cek IP laptop: `ipconfig` (Windows) → catat. Contoh: **172.20.10.3**.
- [ ] Pastikan `mqtt_server` di firmware = IP laptop terbaru (hotspot bisa berubah IP).
- [ ] Firewall laptop IZINKAN port **1883 (MQTT/TCP)** & **5683 (CoAP/UDP)**.

### 0.2 Wiring hardware (rujuk `firmware/wiring-saat-ini.md`)
- [ ] Adaptor 5V → rail breadboard (+/−). ESP32 via USB laptop.
- [ ] **COMMON GROUND** terpasang (rail − ↔ GND ESP32). ← kritis, tanpa ini sensor salah baca.
- [ ] Flow1 SIGNAL(kuning) → **GPIO25** langsung; VCC→5V, GND→rail−.
- [ ] Flow2 SIGNAL(kuning) → **GPIO27** langsung; VCC→5V, GND→rail−.
- [ ] Pressure output → **voltage divider 20K/10K** → **GPIO34**; supply→5V, GND→rail−.
- [ ] INA219: VCC→3V3, GND→GND, SDA→GPIO21, SCL→GPIO22. (Vin+/Vin− KOSONG dulu; seri saat energy.)
- [ ] Jumper DIPASTIKAN rekat/tidak aus (riwayat masalah: jumper longgar → sensor baca 0).
- [ ] Kabel pressure serabut diperkuat (jangan sampai ADC drop ke 0 saat uji).

### 0.3 Upload firmware & verifikasi SENSOR (Serial Monitor)
- [ ] Upload `firmware/deteksi-kebocoran.ino` (isi ssid/password/IP dulu). Baud **115200**.
- [ ] Serial muncul header: `waktu(s),flow1,flow2,selisih,pressure_adc,status`.
- [ ] **Pressure**: alirkan/tekan → `pressure_adc` berubah (bukan diam 0 terus). ✅
- [ ] **Flow1 & Flow2**: isi galon PENUH, alirkan NORMAL → **keduanya NAIK dari 0**. ✅
      (kalau 0 terus walau air deras → cek galon penuh, jumper, arah panah sensor.)
- [ ] INA219 tidak error (tidak `nan` terus).

> ⚠️ Pelajaran sesi lalu: aliran galon harus CUKUP DERAS (galon penuh) agar Flow2 (hilir)
> ikut berputar. Kalau Flow2 tetap 0 saat NORMAL → tekanan kurang; tinggikan/ isi penuh galon.

### 0.4 MQTT jalan (rujuk `firmware/setup-mqtt.md`)
- [ ] Jalankan broker: `mosquitto -c C:\mqtt\mosquitto.conf -v` (listener 1883, allow_anonymous).
- [ ] Serial ESP32: `Connecting MQTT broker... terhubung!` ✅
- [ ] Verifikasi terima data: `mosquitto_sub -h localhost -t smartwater/data -v`
      → JSON mengalir (pressure_adc berubah = data nyata). ✅

### 0.5 CoAP jalan (rujuk `firmware/setup-coap.md`) — untuk uji kinerja/JSON
- [ ] Jalankan server: `python coap_server.py` (aiocoap, resource `/sensor`, UDP 5683).
- [ ] Saat pakai firmware CoAP: server cetak `[CoAP] Diterima (PUT): {...}` + ESP32 dapat `OK`. ✅
      (Untuk TAHAP A deteksi, cukup MQTT. CoAP dipakai di TAHAP C & uji kinerja.)

### 0.6 Dashboard Node-RED jalan (rujuk `firmware/setup-dashboard.md`)
- [ ] `node-red` → buka http://localhost:1880 (editor) & http://localhost:1880/dashboard.
- [ ] Flow sudah ada; kalau hilang → Import `dashboard/flow-dashboard.json`.
- [ ] Gauge **Flow1/Flow2/Pressure** bergerak saat air mengalir. ✅
- [ ] Node text **Status** menampilkan `msg.payload.status` (NORMAL/BOCOR). ✅
- [ ] (opsional) warna hijau=NORMAL, merah=BOCOR.

### 0.7 Logging CSV otomatis AKTIF (rujuk `02-setup-logging-csv-otomatis.md`)
- [ ] Node-RED: cabang `json → csv → file` ke `C:\smartwater\log-deteksi.csv` (append).
- [ ] Tes: alirkan air sebentar → file CSV BERTAMBAH baris. ✅ (biar tak perlu copas Serial manual.)

### ✅ GATE: lanjut ke pengujian HANYA jika SEMUA checklist 0.1–0.7 tercentang.
> Kalau ada 1 yang belum OK, selesaikan dulu. Ini kunci "sekali uji langsung beres".

---

## 🅰️ TAHAP A — PENGUJIAN DETEKSI KEBOCORAN (fungsi utama) — prioritas 1

> Metode & N: lihat `01-metodologi-deteksi-kebocoran-berujukan.md`.
> **N = 10 per kondisi** (10× NORMAL + 10× BOCOR). Evaluasi = confusion matrix +
> Accuracy/Precision/Recall/F1 (mengacu [M-TL] Cheng 2025). Logika flow in-out ([M9],[M10]).
> Lembar isi: `04-lembar-pengujian-deteksi-v2.md`. Penyajian: `03-template-hasil-deteksi-kebocoran.md`.

Susunan: `Galon → keran utama → Flow1 → cabang-1(bocor) → Flow2 → cabang-2(bocor) → Pressure → keluar`.

### A.1 Kalibrasi threshold (WAJIB dulu)
1. Galon PENUH. Alirkan NORMAL: **keran utama BUKA, cabang-1 TUTUP, cabang-2 TUTUP**.
2. Tunggu ~10–15 detik (buang udara, aliran stabil). Flow1 & Flow2 harus sama-sama > 0.
3. Catat 5 baris `selisih` (Lembar K) → ambil **selisih normal maksimum**.
4. Set `THRESHOLD_SELISIH` = sedikit DI ATAS selisih normal maks (mis. maks 0.4 → threshold 0.6).
5. Upload ulang firmware dengan threshold final.

### A.1b SOLUSI bila Flow2 LEMAH / 0 saat kalibrasi (TETAP pakai galon, TANPA ganti ke keran/pompa)

> Jika galon sudah penuh tapi Flow2 (sensor hilir) tetap tidak terbaca / baca sangat kecil,
> masalahnya = TEKANAN DI HILIR KURANG. Tidak perlu ganti sumber air. Coba urut dari termudah;
> BERHENTI di langkah pertama yang berhasil.

**Opsi fisik (tetap galon):**
1. **Tinggikan galon maksimal** — makin tinggi permukaan air di atas pipa, makin deras
   (head gravitasi). Target beda tinggi galon→pipa **≥ 1–1.5 m** (rak/lemari/kursi ditumpuk).
2. **Perpendek & perlurus selang Flow1→Flow2** — makin pendek + tanpa belokan tajam/terlipat,
   makin sedikit tekanan hilang sebelum sampai Flow2.
3. **Outlet (ujung keluaran) dibuat LEBIH RENDAH** dari permukaan air galon — perbesar beda
   tinggi; jangan outlet naik/sejajar galon.
4. **Buang udara terjebak** — biarkan air mengalir beberapa detik dulu sebelum mencatat.

**Opsi kalibrasi (software, TANPA ubah hardware) — paling ampuh:**
5. **Flow2 tidak harus sebesar Flow1.** Yang penting: **selisih saat BOCOR konsisten LEBIH BESAR
   daripada selisih saat NORMAL**, lalu threshold dipasang DI ANTARA keduanya.
   Contoh (data sesi galon-penuh lalu): NORMAL → Flow1≈1.7, Flow2≈0.1–0.3, selisih≈1.5;
   BOCOR → Flow1≈2.2, Flow2≈0, selisih≈2.2. Maka set **THRESHOLD ≈ 1.8** →
   normal (1.5 < 1.8) = NORMAL ✅ ; bocor (2.2 > 1.8) = BOCOR ✅.
   - **Syarat:** selisih NORMAL & selisih BOCOR harus TERPISAH (ada jarak). Cek saat kalibrasi:
     ambil 5 baris NORMAL + 3 baris BOCOR → pastikan rentangnya tidak tumpang-tindih.
   - **Wajib JUJUR di pembahasan (4.8/keterbatasan):** Flow2 under-read akibat keterbatasan
     debit sumber gravitasi (galon); deteksi tetap valid karena berbasis SELISIH relatif, bukan
     nilai absolut. Ini temuan nyata (bukan kelemahan metode) — didukung [M9] Ayamga: aliran
     lemah membuat turbin YF-S201 berputar lambat/di bawah ambang.

> Kesimpulan: walau Flow2 kecil/0, pengujian TETAP bisa jalan & valid selama threshold
> dikalibrasi dari data NORMAL vs BOCOR yang terpisah. Tidak perlu ganti ke keran/pompa.

### A.2 Uji NORMAL — 10×  (kedua cabang TUTUP)
- Tiap percobaan: alirkan air normal, catat 1 baris (flow1, flow2, selisih, pressure, status).
- Harapan: status **NORMAL** (selisih kecil). Tandai benar/salah di Lembar N.

### A.3 Uji BOCOR — 10×  (buka cabang-1; variasikan bukaan kecil/sedang/besar)
- Tiap percobaan: buka cabang-1, catat 1 baris. Harapan: status **BOCOR** (Flow1>Flow2).
- Variasikan bukaan agar mewakili bocor kecil–besar (mengikuti variasi bukaan [M-TL]).

### A.4 Screenshot dashboard (lampiran skripsi)
- [ ] Dashboard kondisi NORMAL (indikator hijau) → simpan file.
- [ ] Dashboard kondisi BOCOR (indikator merah) → simpan file.

### A.5 Hitung hasil (Lembar H / template `03`)
- Susun confusion matrix: TP/TN/FP/FN → Accuracy, Precision, Recall, F1.
- Isi ringkasan ke `03-template-hasil-deteksi-kebocoran.md`.

→ **Mengisi outline:** 4.4 (hasil deteksi), 4.6 (evaluasi: sistem memenuhi kebutuhan deteksi?).

---

## 🅲 TAHAP C — UJI PAYLOAD JSON ASLI (STEP 13) — prioritas 2 (cepat, TANPA air)

> Kode SUDAH SIAP (tinggal upload & run). Tujuan: kondisi operasional nyata (payload = JSON
> sensor ~75 B) + validasi silang tren dengan payload sintetis. Lembar: `lembar-pengujian.md` (Lembar 3C).

### C.1 Latency JSON (30 iterasi) — MQTT & CoAP
- [ ] Jalankan `mqtt/mqtt-uji-latency-json.ino` → catat 30 latency → mean/median/std/min/max.
- [ ] Jalankan `coap/coap-uji-latency-json.ino` → idem.
- (MQTT butuh `echo_mqtt.py` jalan; CoAP butuh `coap_server.py` jalan.)

### C.2 Throughput JSON (100 pesan × 3 ulangan) — MQTT & CoAP
- [ ] Jalankan `mqtt/mqtt-uji-throughput-json.ino` → catat bps & msg/s (3×).
- [ ] Jalankan `coap/coap-uji-throughput-json.ino` → idem.

### C.3 Analisis korelasi
- Ukuran JSON (~75 B) ada di antara 64 & 256 B → cek apakah hasil KONSISTEN dengan tren sintetis.

→ **Mengisi outline:** 4.3 (skenario kondisi nyata), 4.4 (hasil), penguat 4.5.

---

## 🅱️ TAHAP B — ENERGY (INA219 seri) — DIKERJAKAN NANTI (satu per satu)

> Ditunda sesuai kesepakatan (fokus A & C dulu). Saat siap, akan disiapkan:
> - Kode baru `firmware/uji-energy.ino` (ukur arus×tegangan saat idle / kirim MQTT / kirim CoAP).
> - Panduan wiring INA219 SERI (Vin+/Vin− di jalur daya ESP32).
> - Ukur daya (mW) per protokol × payload (16/64/256 B). Rujukan: [M8] Jara Ochoa, [R5].
> → Nanti mengisi 4.4 (hasil energy) & 4.5 (MQTT vs CoAP sisi daya).

**Status: BELUM dikerjakan. Minta ke asisten saat TAHAP A & C sudah selesai.**

---

## 🗺️ PEMETAAN KE OUTLINE RESMI KAMPUS (BAB 4)

> Tafsir disepakati: proyek ini TIDAK punya "sistem sebelumnya", jadi 4.5 = perbandingan
> PERFORMA dua protokol komunikasi (MQTT vs CoAP). (Konfirmasi dosen bila perlu.)

| Outline kampus (BAB 4) | Isi untuk skripsi ini | Sumber data/file | Status |
|------------------------|------------------------|------------------|--------|
| **4.1 Spesifikasi devais** | ESP32 DOIT V1, YF-S201 ×2, INA219, WPT-83G, adaptor 5V | `firmware/log-pengujian.md`, `wiring-saat-ini.md` | ✅ ada |
| **4.2 Konfigurasi devais (script)** | kode .ino (MQTT/CoAP/deteksi), Mosquitto conf, aiocoap, Node-RED flow | folder `firmware/`,`mqtt/`,`coap/`,`dashboard/` | ✅ ada |
| **4.3 Simulasi / skenario (kondisi nyata)** | payload 16/64/256 B, payload JSON asli, skenario NORMAL/BOCOR | `rancangan-pengujian-berujukan.md`, TAHAP C, TAHAP A | 🚧 (JSON & deteksi nyusul) |
| **4.4 Hasil implementasi & pengukuran** | latency, packet loss, throughput, reliability (✅) + deteksi (A) + energy (B) | `hasil-pengujian-*.md`, template `03`, uji baru | 🚧 |
| **4.5 Analisis perbandingan (MQTT vs CoAP)** | 5 parameter kinerja protokol | `4.6-4.7-draft-kinerja.md` | 🚧 (energy nyusul) |
| **4.6 Analisis evaluasi (kebutuhan user)** | apakah deteksi bocor berfungsi? protokol mana cocok utk Smart Water? | sintesis A + 4.5 | ⏳ (setelah A) |

> Catatan: outline BAB 1–3 (topologi, distribusi IP, fact finding) diisi dari dokumen
> perancangan yang sudah ada — di luar lingkup pengujian ini.

---

## ✅ URUTAN EKSEKUSI RINGKAS (checklist besar)
1. [ ] TAHAP 0 — setup & verifikasi SEMUA komponen (0.1–0.7) → GATE.
2. [ ] TAHAP A — deteksi kebocoran (kalibrasi → NORMAL 10× → BOCOR 10× → screenshot → hitung).
3. [ ] TAHAP C — payload JSON asli (latency + throughput, MQTT & CoAP).
4. [ ] (NANTI) TAHAP B — energy (INA219 seri) — minta kode `uji-energy.ino` saat siap.
5. [ ] Tuang semua hasil ke BAB IV sesuai pemetaan outline di atas → fokus menulis.

> Setelah TAHAP A & C: data deteksi + JSON lengkap. Hanya energy (B) tersisa, dikerjakan
> terpisah. Dengan ini pengujian inti CUKUP SEKALI sesi besar + 1 sesi energy kecil.
