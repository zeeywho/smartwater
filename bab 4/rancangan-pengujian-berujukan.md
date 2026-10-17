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

> Catatan: referensi [R1]–[R6] berasal dari pencarian web (Kiro). DISIMPAN sebagai
> kandidat untuk di-review ulang & dicocokkan. Di bawahnya ada [M1]–[Mxx] dari
> library Mendeley milik penulis (sudah dipakai BAB 1–3) yang relevan. Saat finalisasi,
> pilih/cocokkan mana yang paling akurat & kredibel.

### 0.b Referensi dari library Mendeley penulis (relevan — untuk di-review & cocokkan)

> Status: KANDIDAT. Sebagian sudah sangat cocok untuk metode. Perlu baca penuh &
> konfirmasi sebelum sitasi final. Metadata diringkas dari entri Mendeley penulis.

**Kelompok A — Metode pengujian kinerja protokol (paling relevan):**
| Kode | Referensi | Relevansi untuk metode kita |
|------|-----------|------------------------------|
| [M1] | Thangavel, D. et al. (2014). "Performance evaluation of MQTT and CoAP via a common middleware." IEEE ISSNIP 2014. doi:10.1109/ISSNIP.2014.6827678 | ⭐ PALING KUAT. Ukur **end-to-end delay & bandwidth** MQTT vs CoAP pada berbagai **packet loss rate**. Temuan: MQTT delay lebih rendah saat loss kecil; CoAP lebih baik saat loss tinggi. → rujukan metode LATENCY & PACKET LOSS. |
| [M2] | Mishra, M. & Guru, S. (2026). "Performance Evaluation of MQTT, CoAP and HTTP Protocols for Application-Specific IoT Scenarios." IJRASET 14(2). doi:10.22214/IJRASET.2026.77541 | ⭐ Benchmark **9 metrik** (latency, jitter, CPU energy, throughput) pada beban iterasi berbeda (50 & 500). → rujukan JUMLAH ITERASI, ENERGY, THROUGHPUT. (Catatan: ini kemungkinan sama dgn [R6].) |
| [M3] | Martí, M., Garcia-Rubio, C., Campo, C. (2019). "Performance Evaluation of CoAP and MQTT_SN in an IoT Environment." MDPI Proceedings 31(1):49. doi:10.3390/proceedings2019031049 | Studi **energy consumption & network traffic** pada konfigurasi jaringan berbeda. → rujukan ENERGY. (Catatan: sama dgn [R5].) |
| [M4] | Seoane, V. et al. (2021). "Performance evaluation of CoAP and MQTT with security support for IoT environments." Computer Networks 197:108338 (Elsevier). doi:10.1016/J.COMNET.2021.108338 | Fokus **bandwidth & CPU use** (≈ power consumption) di skenario jaringan realistis. → rujukan ENERGY/efisiensi. |
| [M5] | Silva, D. et al. (2021). "A performance analysis of IoT networking protocols: MQTT, CoAP, OPC UA." Applied Sciences 11(11):4879 (MDPI). doi:10.3390/APP11114879 | Metode **time-to-completion** di local testbed + FIT-IoT testbed. CoAP tercepat. → rujukan METODOLOGI TESTBED & LATENCY. (Catatan: sama dgn [R4].) |
| [M6] | Malhotra, J. & Muddassir, M. (2025). "Design and Simulation of a Basic IoT Network Using MQTT and CoAP Protocols." IEEE IC2SDT 2025. doi:10.1109/IC2SDT68218.2025.11383693 | ⭐ Pakai **CoAP-MQTT bridge** + ukur latency/packet loss/jitter/throughput + simulasi variabilitas jaringan. **Mirip arsitektur jembatan kita.** → rujukan JEMBATAN & multi-parameter. |
| [M7] | Naik, N. (2017). "Choice of effective messaging protocols for IoT systems: MQTT, CoAP, AMQP and HTTP." IEEE ISSE 2017. doi:10.1109/SysEng.2017.8088251 | Perbandingan karakteristik protokol (QoS, dll). → rujukan DASAR TEORI pemilihan parameter. |
| [M8] | Jara Ochoa, H. J. et al. (2023). "Comparative Analysis of Power Consumption between MQTT and HTTP..." Sensors 23(10):4896 (MDPI). doi:10.3390/S23104896 | Metode ukur **power consumption** MQTT (QoS 0/1) via NodeMCU, uji jangka panjang. → rujukan METODE ENERGY & QoS. |

**Kelompok B — Metode deteksi kebocoran (flow & pressure):**
| Kode | Referensi | Relevansi |
|------|-----------|-----------|
| [M9] | Ayamga, M. A. & Nakpih, C. I. (2024). "An IoT-Based Water Leakage Detection and Localization System." Asian J. Research in Computer Science 17(3). doi:10.9734/AJRCOS/2024/V17I3421 | ⭐ Pakai **YF-S201** (sensor flow SAMA dgn kita) + static leak detection. → rujukan METODE DETEKSI via flow sensor. |
| [M10] | Sikkandhar, R. A. et al. (2026). "An IoT-Enabled System for Water Conservation, Quality Detection, and Leakage Control." IJEETR 8(2). doi:10.15662/IJEETR.2026.0802203 | ⭐ Deteksi bocor via **perbandingan flow inlet vs outlet** — persis logika Flow1 vs Flow2 kita. → rujukan LOGIKA DETEKSI. |
| [M11] | Fulawade, S. S. et al. (2025). "Smart Water Flow and Pipeline Leakage Detection Using IoT and Arduino UNO." IJRAET 14(1s). | Sensor flow di source & destination, deteksi anomali. → rujukan konsep deteksi. |
| [M12] | Ali, S., Hawwa, M. A., Baroudi, U. (2022). "Effect of Leak Geometry on Water Characteristics Inside Pipes." Sustainability 14(9):5224 (MDPI). doi:10.3390/SU14095224 | Dasar teori: tekanan & kecepatan berfluktuasi di sekitar bocor. → justifikasi pakai PRESSURE sensor. |

**Kelompok C — Dasar teori & spesifikasi (sudah dipakai BAB 1–3):**
| Kode | Referensi | Kegunaan |
|------|-----------|----------|
| [M13] | Al-Fuqaha, A. et al. (2015). "Internet of Things: A Survey on Enabling Technologies, Protocols, and Applications." IEEE Comm. Surveys & Tutorials 17(4). | Teori IoT & protokol (arsitektur 3-lapisan, dll). |
| [M14] | Mishra, B. & Kertesz, A. (2020). "The use of MQTT in M2M and IoT systems: A survey." IEEE Access 8. | Teori & karakteristik MQTT. |
| [M15] | ESP32 Series Datasheet / Technical Reference Manual (Espressif). | Spesifikasi ESP32 (Tabel Alat BAB IV). |
| [M16] | INA219 Datasheet (Texas Instruments, 2008). | Spesifikasi INA219 (pengukuran energy). |
| [M17] | Water Flow Sensor — Seeed Studio Wiki (YF-S201). | Spesifikasi & cara baca flow sensor. |
| [M18] | SEN0257 / Pressure Sensor Reference (DFRobot) & WPT-83G. | Spesifikasi pressure sensor. |

> TODO sebelum menulis final:
> - [ ] Review & cocokkan [R1]–[R6] (web) dengan [M1]–[M18] (Mendeley) — hilangkan duplikat
>       (dugaan duplikat: [R6]≈[M2], [R5]≈[M3], [R4]≈[M5]).
> - [ ] Unduh & baca penuh referensi metode utama ([M1], [M2], [M6], [M8]).
> - [ ] Catat angka pembanding mereka (konteks diskusi, bukan untuk ditiru).
> - [ ] Pastikan tiap metode parameter punya minimal 1 sitasi yang sudah diverifikasi.
> - [ ] Tambah 1–2 referensi jurnal nasional (SINTA) bila diminta kampus.
> - [ ] Format sitasi sesuai panduan (APA/IEEE).

---

## 1. Variabel Kontrol (agar perbandingan adil) — rujuk [R1], [R2]; cek [M1], [M5]

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
- **METODE MENGACU REFERENSI (arahan dosen):** gunakan metode **end-to-end delay**
  seperti pada **[M1] Thangavel et al. 2014** — mengukur delay pengiriman pesan dari
  pengirim ke penerima melalui perantara/middleware (dalam kasus kita: ESP32 → gateway
  laptop), diamati pada beberapa kondisi (termasuk variasi packet loss). Adaptasi
  teknis (RTT vs NTP) menyesuaikan metode [M1] + konfirmasi dosen:
  - (a) Round-Trip Time (RTT): ESP32 catat T1 kirim → server balas → T2 terima;
    RTT=T2−T1; one-way ≈ RTT/2. Cocok CoAP (request-response). Untuk MQTT buat
    echo balasan via topik lain.
  - (b) NTP sync: samakan jam ESP32 & laptop ke NTP → latency = T_terima − T_kirim.
- **Rujukan utama:** [M1] Thangavel 2014 (end-to-end delay MQTT vs CoAP). Pendukung:
  [M5] (time-to-completion/testbed), [M6] (latency + jembatan); web: [R2], [R3], [R4].
- **⚠️ VERIFIKASI DARI PDF [M1]** (wajib sebelum tulis final — PDF publik:
  web.cs.wpi.edu/~rek/IoT/Papers/MQTT_CoAP_Performance.pdf):
  - [ ] cara pasti mereka ukur end-to-end delay (one-way / RTT)
  - [ ] berapa jumlah pesan per uji & ukuran payload
  - [ ] level packet loss yang diuji
- **Pengukuran dicatat:** per protokol, per skenario; simpan ke CSV.
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.2 Throughput
- **Definisi:** jumlah pesan/byte berhasil diterima per satuan waktu (msg/s atau kbps).
- **Metode:** hitung jumlah pesan diterima di gateway dibagi durasi (script Node-RED/Python).
- **Rujukan (web):** [R2], [R6]. **Rujukan (Mendeley):** [M2] (throughput multi-metrik),
  [M6] (throughput + jembatan). → cocokkan saat final.
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.3 Packet Loss (Packet Loss Rate / Packet Success Rate)
- **Definisi:** persentase paket gagal sampai = (dikirim − diterima)/dikirim × 100%.
- **Metode:** ESP32 hitung jumlah kirim; gateway hitung jumlah terima; selisih.
- **Rujukan (web):** [R2], [R6]. **Rujukan (Mendeley):** [M1] (packet loss rate vs delay —
  rujukan terkuat), [M6] (packet loss + jembatan). → cocokkan saat final.
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.4 Energy Consumption
- **Definisi:** konsumsi daya ESP32 saat transmisi (mW) atau energi (mWh).
- **Metode:** INA219 dipasang SERI pada jalur daya ESP32 (Vin+/Vin−), ukur arus×tegangan
  saat transmisi MQTT vs CoAP; rata-ratakan selama durasi uji.
- **Rujukan (web):** [R5], [R6]. **Rujukan (Mendeley):** [M3] (energy & network traffic),
  [M4] (CPU/bandwidth ≈ power), [M8] (metode power consumption QoS 0/1, uji jangka panjang),
  [M2] (CPU energy multi-metrik). (Catatan: KONSEP pengukuran energi antar-protokol yang
  diadopsi; alat ukur boleh berbeda sesuai arahan dosen.) → cocokkan saat final.
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.5 Reliability
- **Definisi:** konsistensi/keberhasilan transmisi selama durasi uji (%).
- **Metode:** % pesan berhasil diterima terhadap total dikirim selama durasi;
  catat juga event putus/reconnect.
- **Rujukan (web):** [R6], [R3]. **Rujukan (Mendeley):** [M2] (reliability MQTT di monitoring
  persisten), [M7] (QoS→reliability), [M1] (keandalan vs loss rate). → cocokkan saat final.
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

### Jumlah pengulangan & sampel N (ANGKA BERSUMBER DARI REFERENSI — arahan dosen)
> Dosen: jumlah sampel N harus bersumber dari referensi, bukan angka pilihan sendiri.

- **MENGACU [M2] Mishra & Guru 2026:** mereka memakai beban **iterasi 50 (nominal)**
  dan **500 (high-stress)** untuk mensimulasikan kondisi normal vs beban tinggi.
  → Usulan: adopsi **N = 50 dan 500 pengiriman** per skenario (atau sesuai angka
  final [M2] setelah diverifikasi dari PDF). Kalimat skripsi: "jumlah pengulangan
  pengiriman mengacu pada [Mishra & Guru, 2026] (50 dan 500 iterasi)."
- **Alternatif/pendukung:** jumlah pesan uji besar dgn banyak iterasi juga dipakai
  [M1] Thangavel 2014 — ambil angka pastinya dari PDF untuk justifikasi tambahan.
- Tiap kondisi → ambil **mean + standar deviasi**.
- **⚠️ VERIFIKASI DARI PDF [M2]/[M1]:**
  - [ ] pastikan angka iterasi [M2] (50 & 500) dari isi paper, bukan hanya abstrak
  - [ ] catat jumlah pesan/iterasi [M1] untuk pembanding
  - [ ] konfirmasi ke dosen: pakai N dari [M2], [M1], atau kombinasi

---

## 4. Analisis Statistik — rujuk [R1]

- Statistik deskriptif: mean, min, max, standar deviasi tiap parameter per protokol.
- Sajikan tabel + grafik perbandingan (MQTT vs CoAP) per skenario.
- (Opsional, bila diminta) uji beda statistik.
- Rujukan: pelaporan berbasis banyak sampel & kondisi terkontrol → [R1].

---

## 4.b Metode Pengujian Deteksi Kebocoran — rujuk [M9], [M10], [M11], [M12]

- **Prinsip:** indikasi bocor = selisih Flow1 − Flow2 signifikan (air "hilang" di antara
  dua sensor) DAN/ATAU penurunan tekanan. Threshold dari kalibrasi data kondisi normal.
- **Rujukan (Mendeley):**
  - [M9] Ayamga 2024 — deteksi kebocoran berbasis sensor **YF-S201** (sama dgn kita).
  - [M10] Sikkandhar 2026 — deteksi via **perbandingan flow inlet vs outlet** (= Flow1 vs Flow2).
  - [M11] Fulawade 2025 — sensor flow di source & destination untuk deteksi anomali.
  - [M12] Ali 2022 — dasar teori fluktuasi tekanan di sekitar bocor (justifikasi pressure).
- **Skenario:** Normal (Flow1≈Flow2, tekanan stabil) vs Bocor (buka T-junction → Flow1>Flow2,
  tekanan turun). Lihat `rencana-pengujian.md` bagian B.
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

---

## 5. Pemetaan ke BAB IV
- Metode kinerja protokol → mengisi **4.6.3 Pengujian Kinerja Protokol** & **4.7 Hasil & Analisis**.
- Metode deteksi kebocoran → mengisi **4.6.2 Pengujian Deteksi Kebocoran**.
- Tabel ringkas hasil → BAB IV Tabel 4.x (Latency/Throughput/Loss/Energy/Reliability).
- Tiap subbab metode WAJIB mencantumkan sitasi yang relevan ([R*] web dan/atau [M*] Mendeley,
  setelah dicocokkan & diverifikasi).

---

## 6. Checklist kesiapan sebelum mengetik hasil
- [ ] Metode latency final (RTT/NTP) + rujukan dikunci
- [ ] N pengulangan & jumlah pesan dikunci (disetujui dosen)
- [ ] INA219 terpasang seri (energy bisa diukur)
- [ ] Script logging CSV siap (timestamp, protokol, parameter)
- [ ] Referensi dibaca penuh & disitasi sesuai panduan kampus (APA/IEEE)
- [ ] **Review & cocokkan [R1]–[R6] (web) dengan [M1]–[M18] (Mendeley)** — buang duplikat,
      pilih yang paling akurat & kredibel per metode
- [ ] Skenario S1–S4 disetujui dosen
