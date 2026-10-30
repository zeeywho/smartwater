# LEMBAR PENGUJIAN — Smart Water Usage (MQTT vs CoAP)

> Lembar siap-isi untuk mencatat hasil saat pengujian. Isi kolom bertanda `___`
> atau `[ ]`. Satu sesi = satu salinan lembar (atau salin tabel per skenario).
> Metode & rujukan: lihat `rancangan-pengujian-berujukan.md`. Rencana: `rencana-pengujian.md`.
>
> Cara pakai: (1) isi Lembar 0 tiap mulai sesi; (2) jalankan skenario; (3) catat di
> tabel terkait; (4) centang checklist black box; (5) salin angka ke tabel ringkas.

---

## LEMBAR 0 — Identitas & Kondisi Sesi Uji (isi tiap sesi)

| Item | Isian |
|------|-------|
| Tanggal / jam | ________________ |
| Penguji | ________________ |
| Protokol yang diuji sesi ini | [ ] MQTT  [ ] CoAP  [ ] Keduanya |
| SSID Wi-Fi | ________________ |
| IP laptop (gateway) | ________________ |
| IP ESP32 | ________________ |
| Broker MQTT (versi) | Mosquitto ________ |
| CoAP server (versi) | aiocoap ________ |
| Firmware yang di-upload | [ ] mqtt-code.ino  [ ] coap-code.ino |
| Ukuran payload uji | [ ] 16 B  [ ] 64 B  [ ] 256 B  (rujuk [R1]) |
| Jumlah iterasi (N) | ________ (rujuk [R1]: **30** per kombinasi) |
| Mode transaksi | [ ] Cold Transaction (buka→kirim→putus) (rujuk [R1]) |
| Jeda antar iterasi | ________ ms (rujuk [R1]: **4000 ms**) |
| QoS / mode | MQTT: [ ] QoS 0 ; CoAP: [ ] CON  (rujuk [R1]) |
| Jarak ESP32–router | ________ m (rujuk [R1]: **1.5 m LOS**) |
| Wi-Fi channel | ________ (rujuk [R1]: **Ch.6, 2.4 GHz**) |
| INA219 terpasang seri? | [ ] Ya  [ ] Tidak  (bila ukur energy) |
| Air mengalir? | [ ] Ya  [ ] Tidak |
| Catatan kondisi (sinyal, gangguan, dll) | ________________ |

---

## LEMBAR 1 — Pengujian Fungsional (Black Box)

> Centang hasil tiap kali diverifikasi. (F1–F7 sudah LULUS di STEP sebelumnya.)

| No | Fitur | Aksi/Input | Output diharapkan | Hasil | Catatan |
|----|-------|-----------|-------------------|-------|---------|
| F1 | ESP32 upload & serial | upload program | Serial Monitor jalan | [ ] PASS [ ] FAIL | |
| F2 | Wi-Fi connect | nyalakan | dapat IP | [ ] PASS [ ] FAIL | |
| F3 | Baca INA219 | nyalakan | V/I/P terbaca | [ ] PASS [ ] FAIL | |
| F4 | Baca Flow 1 | alirkan air | pulsa naik | [ ] PASS [ ] FAIL | |
| F5 | Baca Flow 2 | alirkan air | pulsa naik | [ ] PASS [ ] FAIL | |
| F6 | Baca Pressure | beri tekanan | ADC berubah | [ ] PASS [ ] FAIL | |
| F7 | Baca semua sensor | nyalakan | semua tampil + JSON | [ ] PASS [ ] FAIL | |
| F8 | Kirim via MQTT | publish | broker terima | [ ] PASS [ ] FAIL | |
| F9 | Kirim via CoAP | PUT | server terima | [ ] PASS [ ] FAIL | |
| F10 | Tampil di dashboard | kirim data | muncul di dashboard | [ ] PASS [ ] FAIL | |
| F11 | Deteksi kebocoran | simulasi bocor | status BOCOR | [ ] PASS [ ] FAIL | |

---

## LEMBAR 2 — Pengujian Deteksi Kebocoran

> Skenario: Normal (Flow1≈Flow2, tekanan stabil) vs Bocor (buka T-junction).
> Rujukan metode: [M9] YF-S201, [M10] flow inlet vs outlet, [M12] tekanan. Threshold = ___ (hasil kalibrasi normal).

| Uji ke- | Kondisi | Flow1 (L/min) | Flow2 (L/min) | Selisih | Pressure (ADC/Bar) | Status sistem | Benar? |
|---------|---------|---------------|---------------|---------|--------------------|---------------|--------|
| 1 | [ ] Normal [ ] Bocor | ______ | ______ | ______ | ______ | [ ] NORMAL [ ] BOCOR | [ ] Ya [ ] Tidak |
| 2 | [ ] Normal [ ] Bocor | ______ | ______ | ______ | ______ | [ ] NORMAL [ ] BOCOR | [ ] Ya [ ] Tidak |
| 3 | [ ] Normal [ ] Bocor | ______ | ______ | ______ | ______ | [ ] NORMAL [ ] BOCOR | [ ] Ya [ ] Tidak |
| 4 | [ ] Normal [ ] Bocor | ______ | ______ | ______ | ______ | [ ] NORMAL [ ] BOCOR | [ ] Ya [ ] Tidak |
| 5 | [ ] Normal [ ] Bocor | ______ | ______ | ______ | ______ | [ ] NORMAL [ ] BOCOR | [ ] Ya [ ] Tidak |

- Akurasi deteksi = (deteksi benar / total uji) × 100% = ______ %
- Catatan: ________________________________________________

---

## LEMBAR 3 — Pengujian Kinerja Protokol (INTI) — mengacu [R1]

> Skenario inti [R1]: variasi payload (16/64/256 B) × 2 protokol × **30 iterasi**.
> Konstan: jarak 1.5 m LOS, Wi-Fi Ch.6, cold transaction, jeda 4000 ms, MQTT QoS0 / CoAP CON.
> Satu baris = satu iterasi. Salin blok 3.A untuk tiap kombinasi payload × protokol.
> Parameter & rujukan: lihat `rancangan-pengujian-berujukan.md` §2.

### 3.A — Tabel data mentah per iterasi (satu kombinasi)

**Payload: [ ] 16 B [ ] 64 B [ ] 256 B  |  Protokol: [ ] MQTT (QoS0) [ ] CoAP (CON)  |  N = 30 (rujuk [R1])**

| Iterasi | Kirim berhasil? | Latency/TTC (ms) | Throughput (byte/s) | Catatan |
|---------|-----------------|------------------|---------------------|---------|
| 1 | [ ] Ya [ ] Tidak | ______ | ______ | |
| 2 | [ ] Ya [ ] Tidak | ______ | ______ | |
| 3 | [ ] Ya [ ] Tidak | ______ | ______ | |
| ... | ... | ... | ... | (lanjut sampai 30) |
| 30 | [ ] Ya [ ] Tidak | ______ | ______ | |

**Statistik ringkas kombinasi ini (hitung dari 30 iterasi — rujuk [R1]):**
| Statistik Latency (ms) | Nilai | | Metrik lain | Nilai |
|------------------------|-------|-|-------------|-------|
| Mean | ______ | | Packet Delivery Success Rate (%) | ______ |
| Median | ______ | | Throughput rata-rata (byte/s) | ______ |
| Std dev | ______ | | (opsional) Energy rata-rata (mW) | ______ |
| Min | ______ | | | |
| Max | ______ | | | |

> Rumus: Success Rate = (iterasi berhasil / 30) × 100%. Mean = Σx/30.
> Std dev (sampel) = √(Σ(x−mean)² / (30−1)).

### 3.B — Checklist kombinasi yang sudah diuji (6 kombinasi inti [R1])

| Payload | MQTT (QoS0) selesai? | CoAP (CON) selesai? |
|---------|----------------------|---------------------|
| 16 B | [ ] | [ ] |
| 64 B | [ ] | [ ] |
| 256 B | [ ] | [ ] |

### 3.C — (OPSIONAL) Skenario tambahan — hanya bila disetujui dosen
| Skenario | Variasi | MQTT | CoAP |
|----------|---------|------|------|
| Interval kirim | 1 / 5 / 10 s | [ ] | [ ] |
| Kondisi jaringan | normal / terbebani | [ ] | [ ] |
| Durasi kontinu | __ menit | [ ] | [ ] |

---

## LEMBAR 3B — Pengujian THROUGHPUT (uji terpisah, data baru)

> Metode: kirim 100 pesan BERUNTUN (tanpa jeda), hitung bps & msg/s. Rujuk [M2] Mishra
> (definisi bps) + [M-arXiv] Krikorian (pengiriman kontinu). N=100 = titik tengah 50–500
> (Mishra) yang realistis untuk hardware. Ulang 3× per kombinasi → rata-rata.
> TIDAK perlu air. CoAP: aiocoap server. MQTT: Mosquitto + echo_mqtt.py.

**Isi per kombinasi (3 pengulangan):**

| Kombinasi | Ulang | Sukses/100 | Waktu (s) | msg/s | bps | Success % |
|-----------|-------|-----------|-----------|-------|-----|-----------|
| CoAP 16B | 1 | ____ | ____ | ____ | ____ | ____ |
| CoAP 16B | 2 | ____ | ____ | ____ | ____ | ____ |
| CoAP 16B | 3 | ____ | ____ | ____ | ____ | ____ |
| CoAP 64B | 1–3 | ____ | ____ | ____ | ____ | ____ |
| CoAP 256B | 1–3 | ____ | ____ | ____ | ____ | ____ |
| MQTT 16B | 1–3 | ____ | ____ | ____ | ____ | ____ |
| MQTT 64B | 1–3 | ____ | ____ | ____ | ____ | ____ |
| MQTT 256B | 1–3 | ____ | ____ | ____ | ____ | ____ |

**Rata-rata throughput (dari 3 ulangan) → bahan BAB IV 4.6.3.2 & 4.7.2:**
| Payload | MQTT (bps) | CoAP (bps) | Lebih tinggi |
|---------|-----------|-----------|--------------|
| 16 B | ____ | ____ | ____ |
| 64 B | ____ | ____ | ____ |
| 256 B | ____ | ____ | ____ |

---

## LEMBAR 4 — Tabel Ringkas Hasil (untuk BAB IV 4.7)

> Salin nilai MEAN dari Lembar 3 ke sini (per skenario). Ini langsung jadi bahan tabel skripsi.

| Parameter | Satuan | MQTT | CoAP | Protokol lebih baik | Catatan |
|-----------|--------|------|------|---------------------|---------|
| Latency/TTC (mean) | ms | ______ | ______ | ________ | per payload: isi juga tabel bawah |
| Throughput | byte/s | ______ | ______ | ________ | |
| Packet Delivery Success Rate | % | ______ | ______ | ________ | |
| (opsional) Energy | mW / mWh | ______ | ______ | ________ | rujuk [R5] |

**Rincian Latency mean (ms) per ukuran payload — mengacu [R1]:**
| Payload | MQTT (QoS0) | CoAP (CON) | Lebih cepat |
|---------|-------------|------------|-------------|
| 16 B | ______ | ______ | ________ |
| 64 B | ______ | ______ | ________ |
| 256 B | ______ | ______ | ________ |

---

## LEMBAR 5 — Catatan Kendala & Observasi (untuk Pembahasan 4.8)

| No | Kendala/observasi saat uji | Dugaan penyebab | Tindakan/solusi |
|----|----------------------------|-----------------|-----------------|
| 1 | ________________________ | ________________ | ________________ |
| 2 | ________________________ | ________________ | ________________ |
| 3 | ________________________ | ________________ | ________________ |

---

## CHECKLIST PERSIAPAN SEBELUM UJI (contreng dulu biar nggak kelewat)
- [ ] Metode latency = time-to-completion (RTT-style, tanpa NTP) — rujuk [R1], konfirmasi dosen
- [ ] N = 30 iterasi/kombinasi + payload 16/64/256 B — rujuk [R1], disetujui dosen
- [ ] Mode Cold Transaction + jeda 4000 ms diset di firmware (rujuk [R1])
- [ ] MQTT QoS 0 & CoAP CON diset di kode (rujuk [R1])
- [ ] Jarak 1.5 m LOS & Wi-Fi 2.4 GHz Ch.6 diatur (rujuk [R1])
- [ ] 3 varian payload JSON (~16/64/256 B) sudah dibuat
- [ ] (bila energy masuk scope) INA219 terpasang SERI — rujuk [R5]
- [ ] Koneksi dipermanenkan/diperkuat (hindari drop saat uji lama)
- [ ] Script logging CSV siap (timestamp, protokol, payload, latency, status)
- [ ] Broker Mosquitto `listener 1883 0.0.0.0` + firewall OK
- [ ] CoAP server jalan + firewall UDP 5683 OK
- [ ] Payload MQTT & CoAP IDENTIK (ukuran & isi sama)
- [ ] Dashboard Node-RED jalan (opsional saat uji kinerja)

---

## LEMBAR 3C — Uji Kinerja dengan PAYLOAD JSON SENSOR ASLI (pelengkap)

> Tujuan: kondisi OPERASIONAL NYATA + validasi korelasi dengan tren payload sintetis.
> Payload = format JSON sensor sistem (~75 byte):
> `{"flow1":0.00,"flow2":0.00,"pressure_adc":461,"voltage":0.00,"current":0.0}`
> Metode SAMA dgn uji sintetis (agar sebanding). TIDAK perlu air (nilai statis utk uji jaringan).
> Kode: `*-uji-latency-json.ino` & `*-uji-throughput-json.ino`.

### Latency (30 iterasi) — ringkas statistik
| Protokol | Berhasil | Mean (ms) | Median (ms) | Std | Min | Max | Success % |
|----------|----------|-----------|-------------|-----|-----|-----|-----------|
| MQTT JSON | ___/30 | ___ | ___ | ___ | ___ | ___ | ___ |
| CoAP JSON | ___/30 | ___ | ___ | ___ | ___ | ___ | ___ |

### Throughput (100 pesan × 3 ulangan)
| Protokol | Ulang | Sukses | Waktu(s) | msg/s | bps | Success% |
|----------|-------|--------|----------|-------|-----|----------|
| MQTT JSON | 1–3 | ___ | ___ | ___ | ___ | ___ |
| CoAP JSON | 1–3 | ___ | ___ | ___ | ___ | ___ |

### Analisis korelasi (bahan pembahasan)
- Ukuran JSON asli (~75 B) berada di antara 64 B & 256 B → cek apakah hasilnya konsisten
  dengan tren payload sintetis (validasi silang). Catat temuan: ________________________
