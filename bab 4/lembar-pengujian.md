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
| Payload JSON (ukuran byte) | ________ bytes |
| Interval kirim | ________ s |
| Jumlah pesan/iterasi (N) | ________ (rujuk [M2]: 50 / 500) |
| INA219 terpasang seri? | [ ] Ya  [ ] Tidak |
| Air mengalir? | [ ] Ya  [ ] Tidak |
| Catatan kondisi (cuaca sinyal, gangguan, dll) | ________________ |

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

## LEMBAR 3 — Pengujian Kinerja Protokol (INTI)

> Isi tabel di bawah untuk SETIAP kombinasi skenario × protokol. Satu baris = satu
> kali uji (satu pengulangan). Pastikan kondisi MQTT & CoAP IDENTIK.
> Parameter & rujukan: lihat `rancangan-pengujian-berujukan.md` §2.

### 3.A — Tabel data mentah per pengiriman/iterasi

**Skenario: S____  |  Variasi: __________  |  Protokol: [ ] MQTT  [ ] CoAP  |  N = ____ (rujuk [M2])**

| Ulangan ke- | Pesan dikirim | Pesan diterima | Latency (ms) | Throughput (msg/s) | Packet loss (%) | Daya (mW) | Reliability (%) |
|-------------|---------------|----------------|--------------|--------------------|-----------------|-----------|-----------------|
| 1 | ______ | ______ | ______ | ______ | ______ | ______ | ______ |
| 2 | ______ | ______ | ______ | ______ | ______ | ______ | ______ |
| 3 | ______ | ______ | ______ | ______ | ______ | ______ | ______ |
| 4 | ______ | ______ | ______ | ______ | ______ | ______ | ______ |
| 5 | ______ | ______ | ______ | ______ | ______ | ______ | ______ |
| **Mean** | | | ______ | ______ | ______ | ______ | ______ |
| **Std dev** | | | ______ | ______ | ______ | ______ | ______ |

> Salin blok tabel 3.A ini untuk tiap kombinasi:
> S1 (1s/5s/10s) × 2 protokol; S2 (normal/terbebani) × 2; S3 (QoS0/QoS1 ; NON/CON); S4 (durasi).

### 3.B — Checklist kombinasi yang sudah diuji

| Skenario | Variasi | MQTT selesai? | CoAP selesai? |
|----------|---------|---------------|---------------|
| S1 | interval 1 s | [ ] | [ ] |
| S1 | interval 5 s | [ ] | [ ] |
| S1 | interval 10 s | [ ] | [ ] |
| S2 | jaringan normal | [ ] | [ ] |
| S2 | jaringan terbebani | [ ] | [ ] |
| S3 | QoS0 / NON | [ ] | [ ] |
| S3 | QoS1 / CON | [ ] | [ ] |
| S4 | durasi kontinu (__ menit) | [ ] | [ ] |

---

## LEMBAR 4 — Tabel Ringkas Hasil (untuk BAB IV 4.7)

> Salin nilai MEAN dari Lembar 3 ke sini (per skenario). Ini langsung jadi bahan tabel skripsi.

| Parameter | Satuan | MQTT | CoAP | Protokol lebih baik | Catatan |
|-----------|--------|------|------|---------------------|---------|
| End-to-end Latency | ms | ______ | ______ | ________ | |
| Throughput | msg/s | ______ | ______ | ________ | |
| Packet Loss | % | ______ | ______ | ________ | |
| Energy Consumption | mW / mWh | ______ | ______ | ________ | |
| Reliability | % | ______ | ______ | ________ | |

---

## LEMBAR 5 — Catatan Kendala & Observasi (untuk Pembahasan 4.8)

| No | Kendala/observasi saat uji | Dugaan penyebab | Tindakan/solusi |
|----|----------------------------|-----------------|-----------------|
| 1 | ________________________ | ________________ | ________________ |
| 2 | ________________________ | ________________ | ________________ |
| 3 | ________________________ | ________________ | ________________ |

---

## CHECKLIST PERSIAPAN SEBELUM UJI (contreng dulu biar nggak kelewat)
- [ ] Metode latency final (RTT/NTP) sudah dikunci (rujuk [M1], konfirmasi dosen)
- [ ] N pengulangan dikunci (rujuk [M2]: 50 & 500) & disetujui dosen
- [ ] INA219 terpasang SERI (biar energy terukur, bukan 0)
- [ ] Koneksi dipermanenkan/diperkuat (hindari drop saat uji lama)
- [ ] Script logging CSV siap (timestamp, protokol, parameter)
- [ ] Broker Mosquitto `listener 1883 0.0.0.0` + firewall OK
- [ ] CoAP server jalan + firewall UDP 5683 OK
- [ ] Payload MQTT & CoAP IDENTIK (ukuran & isi sama)
- [ ] Dashboard Node-RED jalan (opsional saat uji kinerja)
