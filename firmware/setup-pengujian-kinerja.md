# Tutorial Setup Pengujian Kinerja MQTT vs CoAP (STEP 11)

> Metode mengacu [R1] Kirilov & Chikurtev 2026. Wiring diasumsikan SUDAH READY.
> Dokumen ini fokus: (A) penyesuaian kode untuk UKUR latency, (B) setup gateway,
> (C) prosedur eksekusi, (D) logging & hitung statistik.
> Pakai bareng: `bab 4/rancangan-pengujian-berujukan.md` + `bab 4/lembar-pengujian.md`.

---

## PENTING DIBACA DULU — kenapa kode lama belum cukup

Kode `mqtt-code.ino` & `coap-code.ino` versi STEP 8/9:
- kirim tiap 1 detik terus-menerus (BUKAN cold transaction),
- TIDAK mengukur latency (tidak ada timestamp/RTT),
- payload tetap (belum 16/64/256 byte).

Untuk pengujian [R1] kita perlu versi uji yang:
1. **Cold transaction:** connect → kirim 1 paket → tunggu konfirmasi → ukur waktu → disconnect.
2. **Ukur latency (RTT):** catat waktu sebelum kirim (t0) & saat konfirmasi diterima (t1); latency = t1−t0.
3. **30 iterasi** per kombinasi, **jeda 4000 ms** antar iterasi.
4. **3 ukuran payload:** 16 / 64 / 256 byte.
5. **MQTT QoS 0 ; CoAP CON.**
6. Cetak hasil tiap iterasi ke Serial (nanti disalin ke CSV/lembar).

> File kode uji siap pakai sudah disiapkan:
> - `mqtt/mqtt-uji-kinerja.ino`
> - `coap/coap-uji-kinerja.ino`
> (lihat penjelasan di bagian A.)

---

## A. Penyesuaian Kode (sudah disiapkan — tinggal isi & upload)

### A.1 Yang WAJIB kamu isi di kedua file uji
- `ssid`, `password` (Wi-Fi 2.4 GHz)
- `server IP` = IP laptop terkini (cek `ipconfig`)
- `PAYLOAD_SIZE` = pilih 16 / 64 / 256 untuk tiap sesi (atau ikuti instruksi di kode)

### A.2 Catatan metode latency (RTT) — konfirmasi dosen
- **CoAP (CON):** server balas ACK → RTT natural. t0 = sebelum `coap.put`, t1 = saat
  callback response. latency = t1−t0. (paling akurat)
- **MQTT (QoS 0):** QoS 0 TIDAK ada ACK aplikasi. Agar ada "konfirmasi", pakai salah satu:
  - (disarankan) ukur waktu `client.publish()` kembali + 1 round-trip dgn subscribe echo:
    ESP32 subscribe topik `smartwater/ack`; laptop (Node-RED/script) echo balik tiap
    pesan diterima; ESP32 catat t1 saat echo tiba. latency = t1−t0.
  - (lebih sederhana, bila dosen setuju) catat waktu proses publish lokal sebagai
    proxy — kurang akurat, kurang disarankan.
- Kode uji MQTT disiapkan dengan OPSI echo (lebih adil vs CoAP). Bila dosen memilih
  metode lain, sesuaikan.

---

## B. Setup Gateway (Laptop) — ulang tiap sesi

### B.1 Cek IP laptop
```
ipconfig
```
Catat IPv4 Wi-Fi → masukkan ke kode ESP32 (server IP).

### B.2 MQTT — jalankan broker Mosquitto
Pastikan `C:\mqtt\mosquitto.conf`:
```
listener 1883 0.0.0.0
allow_anonymous true
```
Jalankan:
```
mosquitto -c C:\mqtt\mosquitto.conf -v
```
(Untuk metode echo MQTT: siapkan Node-RED flow sederhana `mqtt in smartwater/data`
→ `mqtt out smartwater/ack`, ATAU script Python yang subscribe lalu publish balik.)

### B.3 CoAP — jalankan server aiocoap
```
cd C:\mqtt
python coap_server.py
```
(server sudah punya render_put yang balas CHANGED/OK → ini jadi ACK untuk RTT.)

### B.4 Firewall (sekali saja, CMD admin)
```
netsh advfirewall firewall add rule name="Mosquitto MQTT" dir=in action=allow protocol=TCP localport=1883
netsh advfirewall firewall add rule name="CoAP" dir=in action=allow protocol=UDP localport=5683
```

---

## C. Prosedur Eksekusi Pengujian (ikuti urut)

> Target: 3 payload × 2 protokol × 30 iterasi = 180 transaksi.
> Jaga KONSTAN: jarak 1.5 m LOS, Wi-Fi Ch.6, jeda 4000 ms, cold transaction.

### C.1 Persiapan tiap sesi
1. [ ] Isi **Lembar 0** (`lembar-pengujian.md`): tanggal, IP, SSID, payload, protokol.
2. [ ] Posisikan ESP32 ~1.5 m LOS dari router (konsisten tiap sesi).
3. [ ] Pastikan tidak ada beban jaringan lain (skenario normal) — kecuali uji S-net.
4. [ ] Jalankan gateway sesuai protokol (B.2 untuk MQTT / B.3 untuk CoAP).

### C.2 Urutan pengujian (6 kombinasi inti)
Jalankan satu per satu, catat hasil ke **Lembar 3.A** per kombinasi:

| Urut | Protokol | Payload | File di-upload | PAYLOAD_SIZE |
|------|----------|---------|----------------|--------------|
| 1 | MQTT | 16 B | mqtt-uji-kinerja.ino | 16 |
| 2 | MQTT | 64 B | mqtt-uji-kinerja.ino | 64 |
| 3 | MQTT | 256 B | mqtt-uji-kinerja.ino | 256 |
| 4 | CoAP | 16 B | coap-uji-kinerja.ino | 16 |
| 5 | CoAP | 64 B | coap-uji-kinerja.ino | 64 |
| 6 | CoAP | 256 B | coap-uji-kinerja.ino | 256 |

### C.3 Langkah tiap kombinasi
1. Set `PAYLOAD_SIZE` di kode → upload (trik BOOT).
2. Buka Serial Monitor (115200). Kode otomatis jalan 30 iterasi, tiap iterasi:
   `iterasi,latency_ms,status` tercetak.
3. Tunggu sampai "=== SELESAI 30 ITERASI ===" muncul.
4. Salin 30 baris hasil ke Lembar 3.A (atau simpan log Serial ke file).
5. Centang di Lembar 3.B. Ulang untuk kombinasi berikutnya.

> Tips: di Arduino IDE, Serial Monitor bisa di-copy semua → tempel ke Notepad →
> simpan sebagai `hasil_<protokol>_<payload>.txt`.

---

## D. Logging & Hitung Statistik

### D.1 Format baris hasil (dari Serial)
```
iterasi,latency_ms,status
1,42,OK
2,39,OK
...
30,45,OK
```

### D.2 Rekap per kombinasi (isi Lembar 3.A statistik)
- Mean latency = Σlatency / 30
- Median = nilai tengah (urutkan dulu)
- Std dev (sampel) = √(Σ(x−mean)² / 29)
- Min, Max
- Packet Delivery Success Rate = (jumlah OK / 30) × 100%
- Throughput (byte/s) ≈ (PAYLOAD_SIZE × jumlah OK) / total durasi kirim

> Bisa hitung manual, atau salin CSV ke Excel/Google Sheets:
> AVERAGE, MEDIAN, STDEV.S, MIN, MAX, COUNTIF(status,"OK").

### D.3 Isi tabel ringkas (Lembar 4) → langsung bahan BAB IV 4.7

---

## E. Checklist cepat sebelum "GO"
- [ ] Wiring ready (✓ sudah)
- [ ] IP laptop diupdate di kode
- [ ] Broker/server jalan + firewall OK
- [ ] PAYLOAD_SIZE diset sesuai kombinasi
- [ ] Jarak 1.5 m LOS, Wi-Fi Ch.6
- [ ] Lembar 0 terisi
- [ ] Serial Monitor 115200 terbuka
- [ ] Metode latency MQTT (echo) sudah disepakati dosen

---

## F. Troubleshooting (dari pengalaman STEP 8/9)
| Isu | Solusi |
|-----|--------|
| `rc=-4` MQTT connect gagal | broker listen `0.0.0.0` + firewall 1883; IP laptop benar |
| `Association refused`/`CCMP replay` | reset bersih: cabut USB 5 dtk, reconnect Wi-Fi laptop, colok, tekan EN |
| CoAP "Method not allowed" | pastikan server punya `render_put` |
| latency 0 / aneh | cek urutan t0/t1; pastikan callback ACK jalan |
| nilai loncat / nan | INA219 seri longgar (tak kritis utk uji kirim; sudah ada guard nan/inf) |
