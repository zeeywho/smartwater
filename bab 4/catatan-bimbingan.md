# Catatan Bimbingan — SmartWater (MQTT vs CoAP)

> Bahan untuk bimbingan online. Dibuat agar penyampaian progress simpel, dan
> daftar pertanyaan ke dosen tidak terlewat.
> Scope: MQTT + CoAP (LoRa dihapus), gateway laptop, 1 node ESP32.

---

## 1. Cara menyampaikan progress (versi simpel, ~1 menit)

> "Update: **tahap hardware & komunikasi sudah selesai semua**. Keempat sensor
> (2 flow, pressure, INA219) terbaca bersamaan dan menghasilkan data JSON.
> Data itu sudah berhasil dikirim lewat **MQTT maupun CoAP** ke laptop. Terbaru,
> saya sudah bikin **dashboard Node-RED** yang menampilkan data sensor real-time —
> data sudah masuk dan gauge bergerak. Jadi pipeline lengkap dari sensor sampai
> dashboard. **Langkah berikutnya tinggal pengujian & pengambilan data untuk
> membandingkan kinerja kedua protokol.**"

### Peta progress

| Tahap | Status |
|-------|--------|
| STEP 1–7: Hardware & sensor (individual + gabungan) | ✅ Selesai |
| STEP 8: Komunikasi MQTT | ✅ Berhasil |
| STEP 9: Komunikasi CoAP | ✅ Berhasil |
| STEP 10: Dashboard Node-RED (MQTT) | ✅ Baru selesai |
| STEP 11: Jembatan CoAP → dashboard (2 protokol) | 🔜 Berikutnya |
| STEP 12: Ambil data + ukur 5 parameter kinerja | ⏳ Belum |
| STEP 13: Analisis & isi BAB IV 4.7 | ⏳ Belum |

**Posisi satu kalimat:** "Membangun sistem hampir selesai; sekarang masuk ke
ambil data & analisis."

---

## 2. Kemungkinan pertanyaan dosen (+ arah jawaban)

### A. Metodologi perbandingan
- "Bagaimana memastikan perbandingan adil?" → 1 sumber data ESP32, **payload JSON
  identik**, kondisi/jaringan/interval sama. (Sudah diterapkan ✅)
- "Parameter apa yang diukur?" → **latency, throughput, packet loss, energy,
  reliability** (5 parameter).
- "Cara ukur latency?" → ⚠️ belum final — ESP32 kirim timestamp, laptop catat
  waktu terima. (Perlu tanya: NTP sync vs round-trip — lihat bagian 3.)

### B. Deteksi kebocoran
- "Logika deteksi?" → selisih Flow1 − Flow2 + penurunan tekanan. Terbukti di
  STEP 7: Flow1 > Flow2 karena air keluar via T-junction.
- "Threshold berapa?" → ⚠️ belum dikalibrasi; rencana ambil dari data kondisi normal.

### C. Scope
- "Kenapa LoRa dihapus?" → sesuai revisi sebelumnya; fokus MQTT vs CoAP via Wi-Fi.
- "Kenapa gateway laptop?" → menyederhanakan arsitektur, pusatkan logging & analisis.

### D. Keterbatasan (jujur = nilai plus)
- Jumper rusak bikin sensor tidak stabil (sudah diatasi), INA219 kadang `nan`/`inf`
  (sudah di-fix di kode), koneksi breadboard perlu dipermanenkan sebelum ambil
  data berjam-jam.

### E. Pengujian
- "Berapa node?" → 1 node ESP32 (skala rumah tangga).
- "Berapa pengulangan / pesan per uji?" → ⚠️ belum ditentukan (perlu tanya).

---

## 3. YANG PERLU DITANYAKAN KE DOSEN (jangan terlewat)

1. **Metode ukur latency** — NTP sync atau round-trip? Mana yang disarankan &
   bisa dijustifikasi?
2. **Jumlah pengulangan & sampel** — berapa kali per skenario (3×/5×) & berapa
   pesan per uji (mis. 1000) agar statistik valid?
3. **Skenario jarak dihapus** — skenario variasi jarak (proposal lama, untuk LoRa)
   dihapus; disetujui? Diganti variasi frekuensi kirim & kondisi jaringan?
4. **Platform dashboard** — Node-RED sudah jalan sebagai pusat (jembatan 2
   protokol). Cukup, atau tambah Grafana + InfluxDB untuk grafik analisis?
5. **Kalibrasi** — perlu kalibrasi presisi flow→L/min & pressure→Bar, atau cukup
   nilai relatif/ADC untuk deteksi & perbandingan protokol?
6. **Kedalaman analisis** — cukup statistik deskriptif (mean, std, grafik), atau
   perlu uji statistik beda?
7. **Permanenkan koneksi** — perlu solder/protoboard sebelum ambil data
   (stabilitas berjam-jam), atau breadboard cukup?

---

## 4. Bukti yang bisa ditunjukkan saat bimbingan
- `firmware/log-pengujian.md` — log STEP 1–9 (hasil nyata tiap sensor + MQTT + CoAP)
- `firmware/setup-mqtt.md`, `setup-coap.md`, `setup-dashboard.md` — setup & hasil uji
- Screenshot dashboard Node-RED (gauge Pressure bergerak dgn data nyata)
- `bab 4/rencana-pengujian.md` — rencana 5 parameter + skenario

## 5. Tips penyampaian
- Mulai dari yang **sudah selesai** → rencana berikutnya → pertanyaan.
- Tunjukkan log + screenshot sebagai bukti.
- Jujur soal yang belum (threshold, metode latency) — itu justru bahan diskusi.

---

# LAMPIRAN A — Penjelasan 5 Parameter Kinerja (bahan jawab dosen)

Analogi: kirim paket 📦. ESP32 = pengirim, laptop = penerima, MQTT/CoAP = kurir yang dibandingkan.

| Parameter | Pertanyaan inti | Satuan | Bagus kalau | Alat ukur |
|-----------|-----------------|--------|-------------|-----------|
| Latency | Seberapa cepat 1 data sampai? | ms | kecil ⬇️ | timestamp ESP32 + laptop |
| Throughput | Berapa banyak data/detik? | msg/s, kbps | besar ⬆️ | script hitung di laptop |
| Packet Loss | Berapa % data hilang? | % | kecil ⬇️ | hitung (kirim−terima)/kirim |
| Energy | Seberapa boros daya? | mW, mWh | kecil ⬇️ | INA219 (fisik) |
| Reliability | Seberapa konsisten berhasil? | % | besar ⬆️ | % keberhasilan di laptop |

1. **Latency** — waktu 1 data dari dikirim ESP32 → diterima laptop. Penting untuk
   respons cepat deteksi kebocoran. Teori: CoAP (UDP) cenderung lebih cepat.
2. **Throughput** — jumlah data berhasil per detik (kapasitas). Beda dgn latency
   (latency = cepat 1 data; throughput = banyak data/detik).
3. **Packet Loss** — % paket gagal sampai. Teori: MQTT (TCP) loss rendah (retransmit);
   CoAP (UDP/NON) bisa lebih tinggi. Trade-off penting dibahas.
4. **Energy** — konsumsi daya ESP32 saat transmisi (ukur fisik via INA219 seri).
   Penting untuk IoT bertenaga baterai. Teori: CoAP cenderung lebih hemat.
5. **Reliability** — konsistensi keberhasilan selama durasi uji (lebih luas dari
   packet loss: termasuk putus-nyambung/reconnect). Teori: MQTT lebih tinggi.

### Prediksi hasil (HIPOTESIS — wajib dibuktikan dgn data, jangan dikarang)
| Parameter | Prediksi | Alasan |
|-----------|----------|--------|
| Latency | CoAP lebih cepat | UDP tanpa handshake |
| Throughput | sering CoAP | pesan lebih ringkas |
| Packet Loss | MQTT lebih kecil | TCP retransmit |
| Energy | CoAP lebih hemat | UDP ringan |
| Reliability | MQTT lebih tinggi | TCP + QoS |

> Kesimpulan umum: CoAP unggul kecepatan & hemat daya; MQTT unggul keandalan.
> Mana "terbaik" tergantung kebutuhan deteksi kebocoran rumah tangga.

---

# LAMPIRAN B — Metode Ukur Latency: NTP Sync vs Round-Trip (RTT)

Masalah: `latency = waktu_terima(laptop) − waktu_kirim(ESP32)`, tapi jam ESP32 &
jam laptop TIDAK sinkron → hasil bisa ngaco. Dua solusi:

### NTP Sync (samakan jam dulu)
- Samakan jam ESP32 & laptop ke server waktu internet (NTP), lalu ukur one-way.
- ✅ one-way latency lebih "benar" secara konsep.
- ❌ butuh internet; sinkron tak pernah sempurna (bias beberapa ms); setup lebih ribet.

### Round-Trip Time / RTT (pakai 1 jam saja)
- Ukur waktu pulang-pergi pakai jam ESP32 saja: T1 kirim → server balas → T2 terima.
  RTT = T2 − T1; latency 1 arah ≈ RTT/2. Beda jam tak jadi masalah.
- ✅ tak perlu NTP/internet; simpel; COCOK untuk CoAP (request-response, ada balasan).
- ❌ hanya RTT (bukan one-way murni); untuk MQTT perlu bikin mekanisme balasan sendiri
  (pub-sub tak otomatis balas ke ESP32).

> Rekomendasi praktis: RTT paling realistis (apalagi CoAP memang balas). Untuk MQTT
> buat echo/balasan via topik lain. KONFIRMASI ke dosen metode mana yang dipakai.

---

# LAMPIRAN C — Alat Ukur Tiap Pengujian

### Sensor sebagai alat ukur variabel sistem
| Alat | Mengukur | Cara | Status |
|------|----------|------|--------|
| YF-S201 ×2 | debit air (L/min) Flow1 & Flow2 | turbin + Hall → pulsa; baca via interrupt GPIO25/27; L/min≈pulsa/7.5 | ✅ (kalibrasi L/min belum presisi) |
| WPT-83G | tekanan (0–12 Bar) | tegangan 0.5–4.5V → voltage divider 20K/10K → ADC GPIO34 | ✅ fungsi (masih ADC mentah, belum ke Bar) |
| INA219 | V/I/daya ESP32 | dipasang SERI jalur daya (Vin+/Vin−); baca via I2C 21/22 | ✅ terdeteksi; ⚠️ belum dipasang seri utk ukur beban |
| ESP32 | (bukan alat ukur) pusat akuisisi + sumber timestamp | baca semua sensor → JSON → kirim MQTT/CoAP | ✅ |

### Alat ukur 5 parameter kinerja
- Energy = INA219 (fisik). Latency/Throughput/Packet loss/Reliability = software di
  laptop (Node-RED/Python) catat timestamp + hitung pesan kirim vs terima → CSV.
- Titik ukur "diterima": broker Mosquitto (MQTT) & CoAP server aiocoap (CoAP).

---

# LAMPIRAN D — HASIL BIMBINGAN: metode pengujian WAJIB ada rujukan jurnal ⭐

Poin penting dari dosen pembimbing:

- **Angka hasil** boleh (harus) dari pengujian alat sendiri — itu data asli.
- **TAPI metode/cara pengujiannya** (cara ukur latency, throughput, jumlah paket,
  interval kirim, skenario, parameter yang dipilih) **harus merujuk jurnal/paper**.
- Alat tak harus sama; yang penting **konsep & metode pengujian berbasis penelitian**
  terdahulu, sehingga ada dasar/justifikasi.
- Alasan: di sidang, penguji bisa menilai angka "palsu/tidak berdasar" bila metode
  pengujian tidak punya rujukan. Dengan rujukan: "metode mengacu pada [Penulis, Tahun]".

### To-do rujukan
- [ ] Cari 2–4 jurnal perbandingan kinerja MQTT vs CoAP (IoT).
- [ ] Catat: bagaimana mereka ukur latency, throughput, packet loss, energy, reliability.
- [ ] Catat: berapa jumlah paket/pengulangan & interval yang mereka pakai.
- [ ] Tulis di BAB III/IV: metode pengujian mengacu pada rujukan tsb (sitasi).
- [ ] Simpan daftar paper + metodenya (mis. di `bab 4/referensi-metode.md`).

---

# LAMPIRAN E — Rencana penulisan BAB IV (saat tidak pegang alat)

Bisa ditulis SEKARANG (data sudah ada di log):
- 4.1 Implementasi Sistem (arsitektur — draf ada)
- 4.2 Implementasi Hardware (tabel alat, prosedur, wiring)
- 4.3 Pengujian Sensor (INA219, Flow1, Flow2, Pressure, Gabungan — semua LULUS)
- 4.4 Implementasi MQTT & CoAP (sudah berhasil, ada bukti)
- 4.5 Dashboard — bagian MQTT (sudah jadi); CoAP menyusul

DITUNDA (butuh alat + pengujian + rujukan jurnal):
- 4.6 Pengujian sistem (deteksi bocor + kinerja) → `[ISI SETELAH PENGUJIAN]`
- 4.7 Hasil perbandingan 5 parameter → `[ISI SETELAH PENGUJIAN]`
- 4.8–4.9 Pembahasan & ringkasan → `[ISI SETELAH PENGUJIAN]`
