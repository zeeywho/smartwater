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

## PARAMETER UJI TERKUNCI (mengacu [R1] Kirilov & Chikurtev 2026) ⭐

> [R1] = rujukan UTAMA (gold standard) karena setup paling identik: ESP32, laptop
> gateway (Mosquitto + aiocoap), Wi-Fi 2.4 GHz. Nilai di bawah diadopsi dari [R1]
> kecuali ditandai. **Konfirmasi dosen sebelum dieksekusi.**

| Aspek | Nilai (mengacu [R1]) | Rujukan |
|-------|----------------------|---------|
| Topologi | 1 node ESP32 → Router Wi-Fi 2.4 GHz (Ch.6) → Laptop gateway (Mosquitto + aiocoap) | [R1] |
| Jarak | **1.5 m, Line-of-Sight (LOS)**, tanpa halangan | [R1] |
| Mode transaksi | **Cold Transaction** (buka koneksi → kirim → putus) | [R1] |
| Jeda antar iterasi | **4000 ms (4 detik)** | [R1] |
| Ukuran payload | **16 B, 64 B, 256 B** (3 variasi) | [R1], [R4] |
| Jumlah sampel (N) | **30 iterasi per kombinasi** protokol × payload | [R1], [R2] |
| QoS/mode | **MQTT QoS 0** ; **CoAP CON (Confirmable, ACK)** | [R1], [R3] |
| Metrik utama | Latency (mean/median/std/min/max), Packet Delivery Success Rate (%), Throughput (byte/s) | [R1], [R2], [R4] |

> CATATAN penyesuaian penting:
> - Angka N final = **30** (bukan 50/500). 50/500 berasal dari [M2]/Mishra (simulasi NS-3,
>   konteks beda). Untuk setup hardware kita, [R1] (30 iterasi, ESP32 nyata) lebih tepat.
> - Payload kita JSON. Siapkan 3 varian JSON berukuran ~16/64/256 byte agar selaras [R1].
> - Variabel utama di [R1] = ukuran payload (bukan interval). Interval kirim & kondisi
>   jaringan (S1/S2 lama) boleh jadi skenario TAMBAHAN bila disetujui dosen, tapi
>   skenario INTI mengikuti [R1]: 3 payload × 30 iterasi × 2 protokol.

---

## 0. Daftar Referensi Metode (kandidat sitasi)

> Catatan kepatuhan: ringkasan di bawah diparafrasekan dari abstrak/snippet publik
> masing-masing sumber. Verifikasi & baca penuh tiap paper sebelum menyitasi final,
> dan sesuaikan gaya sitasi dengan panduan kampus (APA/IEEE).

| Kode | Referensi | Relevansi untuk metode kita |
|------|-----------|------------------------------|
| [R1] | Kirilov, A., Chikurtev, D. "Assessing MQTT, CoAP, and HTTP Performance in Real-Life Scenarios on ESP32-Based IoT Nodes." MDPI (Engineering Proceedings, 2026). URL: https://www.mdpi.com/2673-4591/150/1/126 | ⭐ **RUJUKAN UTAMA (gold standard)** — setup paling identik (ESP32, laptop gateway Mosquitto+aiocoap, Wi-Fi 2.4GHz Ch.6, 1.5m LOS). Metode: Cold Transaction, jeda 4000ms, payload 16/64/256B, **30 iterasi/kombinasi**, MQTT QoS0 & CoAP CON. Metrik: latency (mean/median/std/min/max) + delivery success rate. → sumber parameter uji terkunci. |
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
| [M-arXiv] | Krikorian, S., Skarnes, B., Vadlamudi, S. V. (2023). "Implementing A Middleware API for Facilitating Heterogeneous IoT Device Communication Protocols and Data Retrieval." arXiv:2312.10294. | ⭐ Setup paling mirip kita: **Mosquitto+paho (MQTT) & aiocoap (CoAP)** + **middleware jembatan 2 protokol**. Ukur travel time (latency) & throughput (distribusi pesan diterima/periode), interval 1 detik, MQTT QoS0. → rujukan THROUGHPUT (pengiriman kontinu) & JEMBATAN CoAP-MQTT dashboard. (Catatan: preprint arXiv — dampingi referensi peer-review; konfirmasi dosen.) |

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

### 2.1 End-to-End Latency / Time-to-Completion
- **Definisi:** waktu dari data dikirim ESP32 sampai diterima/terkonfirmasi di gateway (ms).
- **METODE MENGACU [R1] + [R4] (arahan dosen):** ukur **time-to-completion** tiap
  transaksi dalam **Cold Transaction mode** (buka koneksi → kirim → putus) [R1].
  Karena [R1] memakai CoAP **CON** (ada ACK) dan MQTT **QoS 0**, waktu transaksi
  diukur pada sisi ESP32 (dari mulai kirim sampai selesai/ACK) → setara **RTT**,
  sehingga TIDAK perlu NTP sync. Catat per iterasi.
  - Statistik dari 30 iterasi: **mean, median, std dev, min, max** [R1].
- **Rujukan:** [R1] Kirilov 2026 (metode latency ESP32, cold transaction),
  [R4] Silva 2021 (time-to-completion), [R3] Larmo 2019 (justifikasi TCP vs UDP).
- **⚠️ KONFIRMASI DOSEN:** metode time-to-completion (RTT-style, tanpa NTP) sesuai [R1].
- **Pengukuran dicatat:** per protokol × payload (16/64/256 B); simpan ke CSV.
- **Hasil:** `[ISI SETELAH PENGUJIAN]`

### 2.2 Throughput
- **Definisi:** laju data sukses per satuan waktu (bps dan/atau msg/s).
- **METODE MENGACU [M2] Mishra 2026 + [M-arXiv] Krikorian 2023:** kirim N=100 pesan
  **beruntun (tanpa jeda)**, catat jumlah sukses & total waktu, lalu:
  `bps = (sukses × payload_byte × 8)/waktu` ; `msg/s = sukses/waktu`. Ulang 3× → rata-rata.
  Uji TERPISAH dari latency (data baru). Lihat "Jumlah sampel untuk UJI THROUGHPUT" di §3.
- **Rujukan:** [M2] Mishra (definisi bps + angka pembanding 50/500 iterasi, NS-3),
  [M-arXiv] Krikorian (pendekatan pengiriman kontinu, setup aiocoap+Mosquitto mirip kita),
  [R2] (message/second — figure/abstrak). → CoAP throughput cenderung lebih tinggi (teori).
- **⚠️ CATATAN:** Mishra pakai simulasi (payload 1024 B, 50/500 iter); kita hardware
  (payload 16/64/256 B, 100 pesan). Definisi bps diadopsi; angka disesuaikan ke hardware.
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
> Konfirmasi persetujuan dosen.

### Skenario INTI (mengacu [R1] — variabel utama = ukuran payload)
| Skenario inti | Variasi | MQTT | CoAP | Iterasi | Rujukan |
|---------------|---------|------|------|---------|---------|
| SP — Variasi payload | 16 B | QoS 0 | CON | 30 | [R1], [R4] |
| SP — Variasi payload | 64 B | QoS 0 | CON | 30 | [R1], [R4] |
| SP — Variasi payload | 256 B | QoS 0 | CON | 30 | [R1], [R4] |

→ Total: 3 payload × 2 protokol × 30 iterasi = **180 transaksi** (pola [R1]).
Kondisi konstan: jarak 1.5 m LOS, Wi-Fi 2.4 GHz Ch.6, cold transaction, jeda 4000 ms.

### Skenario TAMBAHAN (opsional — hanya bila disetujui dosen)
| Skenario | Variasi | Rujukan | Catatan |
|----------|---------|---------|---------|
| S-freq | interval kirim 1/5/10 s | [R4] (sending rate → saturasi) | tambahan, bukan inti [R1] |
| S-net | jaringan normal vs terbebani | [R3] (beban) | tambahan |
| S-qos | MQTT QoS 0 vs 1; CoAP NON vs CON | [R6] (QoS→reliability) | tambahan |
| S-durasi | kirim kontinu (mis. 30 menit) | [R1] | untuk reliability jangka panjang |

### Jumlah pengulangan & sampel N (ANGKA BERSUMBER DARI REFERENSI — arahan dosen)
> Dosen: jumlah sampel N harus bersumber dari referensi, bukan angka pilihan sendiri.

- **FINAL — MENGACU [R1] Kirilov & Chikurtev 2026: N = 30 iterasi per kombinasi**
  (protokol × ukuran payload). Setup [R1] paling identik dgn kita (ESP32 + laptop
  gateway), jadi angka 30 ini paling tepat & kredibel. 30 = batas minimal standar
  statistik untuk mean/median/standar deviasi. → didukung [R2] (iterasi berulang).
  Kalimat skripsi: "setiap kombinasi diuji 30 iterasi, mengacu pada [Kirilov & Chikurtev, 2026]."
- **Total transaksi per protokol:** 3 payload (16/64/256 B) × 30 = **90 transaksi/protokol**
  (ikut pola [R1]; [R1] memakai 180 karena 6 kombinasi — sesuaikan jumlah variasi kita).
- Catatan: angka 50/500 dari [M2]/Mishra (NS-3, simulasi) TIDAK dipakai sebagai N utama
  karena konteks berbeda; boleh disebut di pembahasan sebagai pembanding literatur.
- Tiap kombinasi → hitung **mean, median, std dev, min, max** ([R1]).
- **⚠️ KONFIRMASI DOSEN:** setujui N=30 + 3 variasi payload (16/64/256 B) dari [R1].

### Jumlah sampel untuk UJI THROUGHPUT (BERBEDA dari latency — berdasar referensi)
> Dosen: angka sampel harus berdasar referensi, bukan asal. Berikut asal-usulnya.

**Metode throughput ≠ metode latency.** Latency pakai cold transaction + jeda 4000 ms
(1 transaksi/±4 detik). Throughput = **laju data sukses per detik (bps)**, jadi harus
**kirim pesan beruntun (tanpa jeda)** lalu hitung bps. Karena itu butuh uji TERPISAH
(data baru), bukan diturunkan dari data latency.

**Definisi throughput (rumus) — mengacu [M2] Mishra & Guru 2026 (IJRASET):**
> Mishra mendefinisikan *System Throughput (bps)* = "the effective data rate successfully
> transmitted over the channel", dengan rumus berbasis jumlah pesan sukses, ukuran payload,
> dan waktu. Hasil Mishra (NS-3, @500 iterasi): CoAP 625.017 bps > MQTT 139.577 bps.

Rumus yang dipakai:
```
Throughput (bps)   = (pesan_sukses × payload_byte × 8) / total_waktu_detik
Throughput (msg/s) = pesan_sukses / total_waktu_detik
```

**JUMLAH PESAN per kombinasi: N = 100 pesan** — ASAL-USUL ANGKA (3 dasar):
1. **Mishra & Guru 2026 [M2]** memakai **50 (nominal) & 500 (stress) iterasi** pada
   simulasi NS-3. Karena penelitian kita memakai **hardware nyata (ESP32)** — BUKAN
   simulasi — angka 500 kurang praktis (durasi lama + hotspot HP tidak stabil untuk
   burst besar). Dipilih **100** sebagai titik tengah 50–500 yang realistis di hardware.
2. **arXiv Krikorian dkk. 2023** (setup aiocoap+Mosquitto, mirip kita) mengukur throughput
   dari **aliran pesan kontinu** (interval tetap, hitung pesan diterima per periode) —
   mendukung pendekatan "kirim beruntun lalu hitung laju".
3. **Prinsip pengukuran laju**: throughput perlu cukup banyak pesan beruntun agar nilai
   bps **stabil/representatif** (tidak terdistorsi 1–2 pesan). 100 pesan memberi durasi
   uji yang cukup (beberapa detik) tanpa membebani hotspot berlebihan.

→ Kalimat skripsi (contoh): "Pengukuran throughput menggunakan 100 pesan beruntun per
kombinasi. Jumlah ini dipilih sebagai titik tengah yang realistis untuk lingkungan
hardware, mengacu pada rentang beban iterasi (50–500) yang digunakan Mishra & Guru (2026)
pada simulasi NS-3, serta pendekatan pengiriman kontinu pada Krikorian dkk. (2023)."

- **Pengulangan:** tiap kombinasi diulang **3×** → ambil rata-rata (konsistensi).
- **Payload:** tetap **16/64/256 B** (konsisten dengan uji latency & [R1]).
- **⚠️ KONFIRMASI DOSEN:** setujui N=100 pesan (atau sesuaikan: 50/500 ala Mishra bila
  diminta — tapi 500 di hardware+hotspot berisiko lama/tidak stabil).

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
- [ ] Metode latency = time-to-completion (RTT-style, tanpa NTP) sesuai [R1] — konfirmasi dosen
- [ ] N = 30 iterasi per kombinasi + payload 16/64/256 B (mengacu [R1]) — disetujui dosen
- [ ] Konstanta uji disiapkan: jarak 1.5 m LOS, Wi-Fi Ch.6, cold transaction, jeda 4000 ms
- [ ] 3 varian payload JSON (~16/64/256 B) sudah dibuat di firmware
- [ ] MQTT QoS 0 & CoAP CON diset di kode
- [ ] INA219 terpasang seri (bila energy dimasukkan — rujuk [R5])
- [ ] Script logging CSV siap (timestamp, protokol, payload, parameter)
- [ ] Referensi dibaca penuh & disitasi sesuai panduan kampus (APA/IEEE)
- [ ] Skenario INTI (payload×30) + tambahan (opsional) disetujui dosen

---

## 7. Draft Teks Metodologi (siap pakai di BAB IV — sesuaikan & verifikasi)

> Sumber: dirangkum dari [R1] Kirilov & Chikurtev (2026) dan [R4] Silva et al. (2021).
> Verifikasi angka dari paper asli & sesuaikan dgn hasil konfirmasi dosen sebelum final.

> **4.x Metode dan Prosedur Pengujian Performa Protokol**
>
> Prosedur pengujian komparasi performa protokol MQTT dan CoAP pada sistem Smart
> Water Usage ini dirancang mengacu pada metodologi pengujian oleh Kirilov &
> Chikurtev [R1] dan Silva et al. [R4]. Pengujian dilakukan dalam kondisi
> laboratorium terkontrol menggunakan satu node transmisi ESP32, gateway laptop
> (menjalankan broker Mosquitto untuk MQTT dan server aiocoap untuk CoAP), serta
> router Wi-Fi 2.4 GHz pada jarak konstan 1,5 meter Line-of-Sight (LOS).
>
> Pengujian menerapkan metode Cold Transaction, di mana setiap iterasi pengiriman
> diawali inisiasi koneksi baru dan diakhiri pemutusan koneksi [R1]. Untuk mencegah
> residual buffer pada stack jaringan TCP/UDP ESP32, diterapkan jeda inter-trial
> sebesar 4.000 ms antar iterasi [R1]. Pengujian dilakukan terhadap tiga variasi
> ukuran payload telemetry, yaitu 16 byte, 64 byte, dan 256 byte [R1, R4]. Setiap
> variasi direplikasi sebanyak 30 iterasi untuk memenuhi kecukupan sampel statistik
> deskriptif (mean, median, standar deviasi) [R1, R2]. MQTT dikonfigurasi pada QoS 0
> dan CoAP pada mode Confirmable (CON) [R1, R3].

---

## 8. STATUS KESESUAIAN DENGAN REFERENSI (transparansi)

| Hal | Status | Keterangan |
|-----|--------|------------|
| N = 30 iterasi | ✅ dari ringkasan [R1] | perlu verifikasi di PDF [R1] & setujui dosen |
| Payload 16/64/256 B | ✅ dari ringkasan [R1] | sesuaikan ke JSON kita |
| Jeda 4000 ms, cold transaction | ✅ dari ringkasan [R1] | implementasi di firmware |
| QoS 0 / CoAP CON | ✅ dari ringkasan [R1] | set di kode |
| Jarak 1.5 m LOS, Wi-Fi Ch.6 | ✅ dari ringkasan [R1] | atur saat setup uji |
| Metode latency (time-to-completion) | ✅ dari [R1]/[R4] | konfirmasi dosen |
| Energy consumption | ⚠️ opsional | [R1] fokus latency; energy rujuk [R5]. Putuskan apakah energy masuk scope |
| Angka pembanding hasil [R1] | ⏳ belum diekstrak | baca PDF untuk konteks diskusi (bukan ditiru) |
