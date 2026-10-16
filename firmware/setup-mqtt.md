# Setup & Hasil Uji MQTT — Smart Water Usage (STEP 8)

> Status: ✅ BERHASIL (16 Oktober 2026). Pipeline end-to-end terbukti: ESP32 publish → broker → subscriber menerima data.
> Dokumen ini jadi acuan paralel saat menyiapkan CoAP (STEP 9) agar perbandingan adil.

---

## 1. Arsitektur MQTT

```
ESP32 (publisher)
   │  Wi-Fi (hotspot HP "zee")
   │  publish topik: smartwater/data
   ▼
Broker Mosquitto (laptop Windows, port 1883)
   │
   ▼
Subscriber (mosquitto_sub di laptop) → menampilkan data
```

- Model: **publish-subscribe** (TCP)
- Payload: JSON (sama persis untuk nanti dibandingkan dgn CoAP)

---

## 2. Komponen & Versi

| Komponen | Detail |
|----------|--------|
| Broker | Mosquitto 2.1.2 (Windows) |
| Library ESP32 | PubSubClient (Arduino) |
| Jaringan | Hotspot HP, SSID "zee" |
| Port | 1883 (TCP) |
| Topik | `smartwater/data` |
| IP laptop (broker) | 172.20.10.3 (dinamis, cek `ipconfig` tiap sesi) |
| IP ESP32 | 172.20.10.2 (dinamis) |

> ⚠️ IP hotspot HP BERUBAH-UBAH. Tiap mulai sesi: cek `ipconfig` di laptop, update `mqtt_server` di kode ESP32.

---

## 3. Setup Broker (Laptop Windows) — langkah

### a. Install Mosquitto
- Download dari https://mosquitto.org/download/ (installer Windows x64), install default.

### b. Buat file config `C:\mqtt\mosquitto.conf`
```
listener 1883 0.0.0.0
allow_anonymous true
```
> Taruh di `C:\mqtt\` (bukan Program Files) untuk hindari masalah permission save.

### c. Stop service bawaan (kalau port bentrok)
CMD as Administrator:
```
net stop mosquitto
```

### d. Izinkan firewall (port 1883 TCP)
CMD as Administrator:
```
netsh advfirewall firewall add rule name="Mosquitto MQTT" dir=in action=allow protocol=TCP localport=1883
```

### e. Jalankan broker
```
cd "C:\Program Files\mosquitto"
mosquitto -c C:\mqtt\mosquitto.conf -v
```
Berhasil jika muncul: `Opening ipv4 listen socket on port 1883` + `running` (TANPA "local only mode").
Biarkan jendela terbuka.

---

## 4. Subscriber (untuk lihat data masuk)

CMD biasa (TIDAK perlu admin), di mesin yang sama dengan broker → pakai `localhost`:
```
mosquitto_sub -h localhost -t smartwater/data -v
```
> PENTING: subscriber tuju IP BROKER (laptop), BUKAN IP ESP32. `localhost` paling aman.

---

## 5. Kode ESP32 (MQTT Publish — versi diperkuat)

File: `firmware/mqtt_publish.ino` (simpan terpisah, jangan dihapus).

Poin penting kode:
- Wi-Fi connect + auto-reconnect
- Client ID UNIK (random) → hindari "session taken over"
- `setKeepAlive(60)` + `setSocketTimeout(30)` → koneksi lebih tahan
- Publish payload JSON ke `smartwater/data` tiap 1 detik
- Pin sensor: Flow1=GPIO25, Flow2=GPIO27, Pressure=GPIO34, INA219=I2C(21/22)

Payload JSON (format identik untuk CoAP nanti):
```json
{"flow1":0.00,"flow2":0.00,"pressure_adc":461,"voltage":0.00,"current":0.0}
```

Yang harus diisi di kode: `ssid`, `password`, `mqtt_server` (IP laptop terkini).

---

## 6. Hasil Uji (BUKTI NYATA)

### Serial Monitor ESP32:
```
=== STEP 8: MQTT PUBLISH ===
WiFi OK. IP ESP32: 172.20.10.2
Connecting MQTT broker... terhubung!
Published: {"flow1":0.00,"flow2":0.00,"pressure_adc":461,"voltage":0.00,"current":0.0}
... (publish tiap detik)
```

### Log Broker (Mosquitto -v):
```
New client connected ... as ESP32_SmartWater
Received PUBLISH from ESP32_SmartWater (... 'smartwater/data', ... 73-77 bytes)  [puluhan pesan]
Received PINGREQ → Sending PINGRESP  (keepalive jalan)
```

### Subscriber (mosquitto_sub -h localhost):
```
smartwater/data {"flow1":0.00,"flow2":0.00,"pressure_adc":461,"voltage":0.00,"current":0.0}
smartwater/data {"flow1":0.00,"flow2":0.00,"pressure_adc":368,"voltage":0.00,"current":0.0}
```
→ pressure_adc berubah = data sensor NYATA terkirim end-to-end. ✅

---

## 7. Isu yang Ditemui & Solusi (untuk BAB IV / pembahasan)

| Isu | Penyebab | Solusi |
|-----|----------|--------|
| `rc=-2` gagal connect | IP laptop (broker) di kode salah | Update `mqtt_server` ke IP laptop terkini (`ipconfig`) |
| Subscriber error connect | subscriber tuju IP ESP32, bukan broker | Pakai `-h localhost` atau IP laptop |
| "session taken over" | client ID sama saat reconnect | Client ID unik (random) |
| "exceeded timeout" | keepalive pendek (k15) + Wi-Fi goyang | `setKeepAlive(60)`, Wi-Fi reconnect |
| Port 1883 bentrok | service Mosquitto sudah jalan | `net stop mosquitto` dulu |
| Config "local only mode" | listener belum di-set | tambah `listener 1883 0.0.0.0` di config |

---

## 8. Checklist mulai sesi MQTT (biar cepat)

1. [ ] Laptop + ESP32 connect ke hotspot "zee" yang sama
2. [ ] `ipconfig` → catat IPv4 laptop → update `mqtt_server` di kode ESP32
3. [ ] Jalankan broker: `mosquitto -c C:\mqtt\mosquitto.conf -v`
4. [ ] Upload kode ESP32 (trik BOOT)
5. [ ] Subscriber: `mosquitto_sub -h localhost -t smartwater/data -v`
6. [ ] Alirkan air → cek data sensor nyata muncul

---

## 9. Yang BELUM dilakukan (untuk tahap pengujian kinerja nanti)
- [ ] Tes dengan AIR MENGALIR (data sensor nyata, bukan 0)
- [ ] Ukur parameter: latency, throughput, packet loss, energy, reliability
- [ ] Bandingkan dengan CoAP (kondisi identik)
- [ ] Logging ke CSV untuk analisis statistik
