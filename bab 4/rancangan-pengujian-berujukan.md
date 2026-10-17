# Rancangan Pengujian Berujukan — Smart Water Usage (MQTT vs CoAP)

> Tujuan dokumen: menyiapkan **metode pengujian yang punya rujukan jurnal** SEBELUM
> pengujian dilakukan. Jadi saat sudah dapat angka, tinggal diisi ke template ini
> tanpa mencari-cocokkan jurnal lagi.
>
> **Prinsip (sesuai arahan dosen):** Angka hasil = dari pengujian alat sendiri (asli).
> Tapi **cara/metode pengujiannya harus merujuk penelitian terdahulu** supaya ada
> dasar/justifikasi dan tidak dianggap "angka tanpa dasar" saat sidang. Alat tidak
> harus sama; yang penting KONSEP & METODE mengacu ke jurnal.
>
> Scope: MQTT + CoAP (LoRa dihapus), 1 node ESP32, gateway laptop, Wi-Fi.
> Status hasil: `[ISI SETELAH PENGUJIAN]` — jangan mengarang.

---

## 0. Daftar Referensi Metode (kandidat sitasi)

> Catatan kepatuhan: ringkasan di bawah diparafrasekan dari abstrak/snippet publik
> masing-masing sumber. Verifikasi & baca penuh tiap paper sebelum menyitasi final,
> dan sesuaikan gaya sitasi dengan panduan kampus (APA/IEEE).

| Kode | Referensi | Relevansi untuk metode kita |
|------|-----------|------------------------------|
| [R1] | Kirilov, A., Chikurtev, D. "Assessing MQTT, CoAP, and HTTP Performance in Real-Life Scenarios on ESP32-Based IoT Nodes." MDPI (Engineering Proceedings). URL: https://www.mdpi.com/2673-4591/150/1/126 | **Paling mirip** — pakai ESP32, banding MQTT/CoAP/HTTP di kondisi terkontrol, banyak sampel. Rujukan untuk: pengujian berbasis node ESP32 & pengulangan sampel. |
| [R2] | "A comparative analysis and implementation of CoAP and MQTT protocol for IoT communication." Springer (2025). URL: https://link.springer.com/article/10.1007/s41872-025-00324-7 | Rujukan untuk PARAMETER: message/second, throughput, packet success rate, packet loss rate, latency; banyak iterasi. CoAP throughput tinggi & latency rendah vs MQTT. |
| [R3] | "Impact of CoAP and MQTT on NB-IoT System Performance." MDPI Sensors 19(1):7 (2019). URL: https://www.mdpi.com/1424-8220/19/1/7 | Rujukan untuk: latency & kapasitas sistem; temuan CoAP/UDP lebih baik di latency, MQTT/TCP tetap jalan saat beban rendah. |
| [R4] | "A Performance Analysis of Internet of Things Networking Protocols (MQTT, CoAP, OPC UA)." MDPI Applied Sciences 11(11):4879 (2021). URL: https://www.mdpi.com/2076-3417/11/11/4879 | Rujukan untuk: metrik time-to-completion antar skenario; CoAP terendah. |
| [R5] | "Performance Evaluation of CoAP and MQTT-SN in an IoT Environment." MDPI Proceedings 31(1):49 (2019). URL: https://www.mdpi.com/2504-3900/31/1/49 | Rujukan untuk: metode ukur ENERGY consumption & network traffic pada konfigurasi jaringan berbeda. |
| [R6] | "Performance Evaluation of MQTT, CoAP and HTTP Protocols for Application-Specific IoT Scenarios." IJRASET. URL: https://www.ijraset.com/research-paper/performance-evaluation-of-mqtt-coap-and-http-protocols | Rujukan untuk: CoAP throughput tinggi + konsumsi sumber daya rendah; MQTT reliability unggul di monitoring persisten. |

> TODO sebelum menulis final:
> - [ ] Unduh & baca penuh [R1]–[R6] (beberapa open-access MDPI).
> - [ ] Catat angka pembanding mereka (sebagai konteks diskusi, bukan untuk ditiru).
> - [ ] Tambah 1–2 referensi dari jurnal nasional (SINTA) bila diminta kampus.
> - [ ] Format sitasi sesuai panduan (APA/IEEE).

---

## 1. Variabel Kontrol (agar perbandingan adil) — rujuk [R1], [R2]

Dikunci SAMA untuk MQTT & CoAP (prinsip "fair comparison" di [R1], [R2]):
- Payload JSON identik (isi & ukuran sama).
- Jumlah pesan uji sama (lihat §3).
- Jaringan Wi-Fi sama, posisi perangkat sama.
- Interval kirim sama per skenario.
- Durasi uji sama.
- Satu sumber data (1 ESP32) → data identik dikirim lewat dua protokol.

Justifikasi: [R1] menguji protokol pada node ESP32 dalam kondisi terkontrol dengan
banyak sampel; prinsip kondisi identik ini kita adopsi agar perbedaan yang terukur
murni berasal dari karakteristik protokol.

---

## 2. Definisi & Metode Ukur Tiap Parameter (DENGAN RUJUKAN)

### 2.1 End-to-End Latency
- **Definisi:** selisih waktu data dikirim ESP32 sampai diterima gateway (ms).
- **Metode:** [PILIH & KONFIRMASI DOSEN]
  - (a) Round-Trip Time (RTT): ESP32 catat T1 kirim → server balas → T2 terima;
    RTT=T2−T1; one-way ≈ RTT/2. Cocok CoAP (request-response). Untuk MQTT buat
    echo balasan via topik lain.
  - (b) NTP sync: samakan jam ESP32 & laptop ke NTP → latency = T_terima − T_kirim.
- **Rujukan:** metrik latency sebagai pembanding utama MQTT vs CoAP → [R2], [R3], [R4].
- **Pengukuran dicatat:** per protokol, per skenario; simpan ke CSV.
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.2 Throughput
- **Definisi:** jumlah pesan/byte berhasil diterima per satuan waktu (msg/s atau kbps).
- **Metode:** hitung jumlah pesan diterima di gateway dibagi durasi (script Node-RED/Python).
- **Rujukan:** throughput & message-per-second sebagai metrik → [R2], [R6].
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.3 Packet Loss (Packet Loss Rate / Packet Success Rate)
- **Definisi:** persentase paket gagal sampai = (dikirim − diterima)/dikirim × 100%.
- **Metode:** ESP32 hitung jumlah kirim; gateway hitung jumlah terima; selisih.
- **Rujukan:** packet loss rate & packet success rate → [R2]; keandalan TCP (MQTT)
  menekan loss vs UDP (CoAP) → [R6].
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.4 Energy Consumption
- **Definisi:** konsumsi daya ESP32 saat transmisi (mW) atau energi (mWh).
- **Metode:** INA219 dipasang SERI pada jalur daya ESP32 (Vin+/Vin−), ukur arus×tegangan
  saat transmisi MQTT vs CoAP; rata-ratakan selama durasi uji.
- **Rujukan:** studi energy consumption antar protokol IoT → [R5], [R6]. (Catatan:
  [R5]/[R3] memakai pendekatan/alat berbeda — di sini KONSEP pengukuran energi
  antar-protokol yang diadopsi, alat ukur boleh berbeda sesuai arahan dosen.)
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.5 Reliability
- **Definisi:** konsistensi/keberhasilan transmisi selama durasi uji (%).
- **Metode:** % pesan berhasil diterima terhadap total dikirim selama durasi;
  catat juga event putus/reconnect.
- **Rujukan:** keunggulan reliability MQTT (QoS/TCP) pada monitoring persisten → [R6];
  perilaku saat beban berbeda → [R3].
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

---

## 3. Skenario Pengujian — rujuk [R1], [R2], [R3]

> Catatan: skenario "variasi jarak" dari proposal lama DIHAPUS (relevan untuk LoRa).
> Konfirmasi persetujuan dosen. Diganti variasi frekuensi kirim & kondisi jaringan.

| Skenario | Variasi | MQTT | CoAP | Rujukan |
|----------|---------|------|------|---------|
| S1 Frekuensi kirim | interval 1 s / 5 s / 10 s | ✅ | ✅ | [R2] (banyak iterasi/msg-rate) |
| S2 Kondisi jaringan | normal vs terbebani/interferensi | ✅ | ✅ | [R1] (real-life/terkontrol), [R3] (beban) |
| S3 QoS/mode | MQTT QoS 0 vs 1; CoAP NON vs CON | ✅ | ✅ | [R6] (QoS→reliability) |
| S4 Durasi/stabilitas | kirim kontinu (mis. 30 menit) | ✅ | ✅ | [R1] (multiple samples) |

### Jumlah pengulangan & sampel (KONFIRMASI DOSEN)
- Tiap kombinasi diulang N kali (usul 3–5×) → ambil mean + standar deviasi.
- Jumlah pesan per uji: usul 1000 pesan/skenario (sesuaikan dgn [R2] yang pakai
  banyak iterasi). → `[TENTUKAN ANGKA SETELAH KONFIRMASI]`

---

## 4. Analisis Statistik — rujuk [R1]

- Statistik deskriptif: mean, min, max, standar deviasi tiap parameter per protokol.
- Sajikan tabel + grafik perbandingan (MQTT vs CoAP) per skenario.
- (Opsional, bila diminta) uji beda statistik.
- Rujukan: pelaporan berbasis banyak sampel & kondisi terkontrol → [R1].

---

## 5. Pemetaan ke BAB IV
- Metode ini mengisi **4.6.3 Pengujian Kinerja Protokol** & **4.7 Hasil & Analisis**.
- Tabel ringkas hasil → BAB IV Tabel 4.x (Latency/Throughput/Loss/Energy/Reliability).
- Tiap subbab metode WAJIB mencantumkan sitasi [R1]–[R6] yang relevan.

---

## 6. Checklist kesiapan sebelum mengetik hasil
- [ ] Metode latency final (RTT/NTP) + rujukan dikunci
- [ ] N pengulangan & jumlah pesan dikunci (disetujui dosen)
- [ ] INA219 terpasang seri (energy bisa diukur)
- [ ] Script logging CSV siap (timestamp, protokol, parameter)
- [ ] [R1]–[R6] sudah dibaca penuh & disitasi sesuai panduan kampus
- [ ] Skenario S1–S4 disetujui dosen
