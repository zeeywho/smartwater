# Setup & Hasil Uji CoAP — Smart Water Usage (STEP 9)

> Status: ✅ BERHASIL (16 Oktober 2026). Pipeline end-to-end terbukti: ESP32 PUT → CoAP server menerima data + response OK.
> Dokumen paralel dengan `setup-mqtt.md` agar perbandingan MQTT vs CoAP mudah ditrack.

---

## 1. Arsitektur CoAP

```
ESP32 (CoAP client)
   │  Wi-Fi rumah (192.168.1.x)
   │  PUT ke resource: /sensor  (port 5683 UDP)
   ▼
CoAP Server (laptop, Python aiocoap)
   │  balas "OK" (response)
   ▼
(data tampil di console server)
```

- Model: **request-response** (UDP) — beda dari MQTT (publish-subscribe/TCP)
- TIDAK ada broker perantara (ESP32 langsung ke server)
- Payload: JSON **IDENTIK** dengan MQTT (untuk perbandingan adil)

---

## 2. Komponen & Versi

| Komponen | Detail |
|----------|--------|
| Server | Python aiocoap 0.4.17 (Windows) |
| Library ESP32 | coap-simple-library (method `put`) |
| Jaringan | Wi-Fi rumah (192.168.1.x) — lebih stabil dari hotspot HP |
| Port | 5683 (UDP) |
| Resource | `/sensor` |
| IP laptop (server) | 192.168.1.10 |

---

## 3. Setup CoAP Server (Laptop Windows)

### a. Install aiocoap
```
pip install aiocoap
```

### b. File `C:\mqtt\coap_server.py`
```python
import asyncio
import socket
from aiocoap import resource, Message
import aiocoap

def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    except Exception:
        ip = '127.0.0.1'
    finally:
        s.close()
    return ip

class SensorResource(resource.Resource):
    async def render_put(self, request):
        payload = request.payload.decode('utf-8')
        print(f"[CoAP] Diterima (PUT): {payload}")
        return Message(code=aiocoap.CHANGED, payload=b"OK")
    async def render_post(self, request):
        payload = request.payload.decode('utf-8')
        print(f"[CoAP] Diterima (POST): {payload}")
        return Message(code=aiocoap.CHANGED, payload=b"OK")

async def main():
    local_ip = get_local_ip()
    root = resource.Site()
    root.add_resource(['sensor'], SensorResource())
    print("=== CoAP Server berjalan ===")
    print(f"IP server (laptop): {local_ip}  |  Port: 5683")
    await aiocoap.Context.create_server_context(root, bind=(local_ip, 5683))
    await asyncio.get_running_loop().create_future()

if __name__ == "__main__":
    asyncio.run(main())
```

### c. Jalankan server
```
cd C:\mqtt
python coap_server.py
```
Catat IP yang muncul → masukkan ke kode ESP32.

### d. Firewall (port 5683 UDP) — CMD as Administrator
```
netsh advfirewall firewall add rule name="CoAP" dir=in action=allow protocol=UDP localport=5683
```

---

## 4. Kode ESP32 (CoAP client)

File: `firmware/coap_send.ino` (simpan terpisah).
Poin penting:
- Library `coap-simple.h`, kirim via `coap.put(coap_server, 5683, "sensor", payload)`
- Payload JSON IDENTIK dengan MQTT
- Isi: `ssid`, `password` (Wi-Fi rumah 2.4GHz), `coap_server` IP (192.168.1.10)
- Pin sensor sama: Flow1=GPIO25, Flow2=GPIO27, Pressure=GPIO34, INA219=I2C(21/22)

---

## 5. Hasil Uji (BUKTI NYATA)

### Console CoAP server:
```
=== CoAP Server berjalan ===
IP server (laptop): 192.168.1.10  |  Port: 5683
[CoAP] Diterima (PUT): {"flow1":0.00,"flow2":0.00,"pressure_adc":2896,"voltage":0.00,"current":nan}
[CoAP] Diterima (PUT): {"flow1":0.00,"flow2":0.00,"pressure_adc":0,...}
```

### Serial Monitor ESP32:
```
CoAP sent: {"flow1":0.00,...,"pressure_adc":1710,...}
Response dari server: OK
```
→ pressure_adc berubah = data sensor nyata; response OK = komunikasi dua arah. ✅

---

## 6. Isu yang Ditemui & Solusi (untuk BAB IV / pembahasan)

| Isu | Penyebab | Solusi |
|-----|----------|--------|
| `coap.post` compile error | library tidak punya method post | pakai `coap.put()` |
| "Method not allowed" | server hanya render_post, ESP32 kirim PUT | tambah `render_put` di server |
| aiocoap tidak bind `0.0.0.0` | keterbatasan transport Windows | bind ke IP laptop spesifik (auto-detect) |
| ESP32 beda jaringan dgn laptop | laptop pindah ke Wi-Fi rumah | samakan: ESP32 juga ke Wi-Fi rumah |
| `current: nan` | INA219 koneksi longgar | rapikan koneksi/jumper (belum kritis utk tes kirim) |
| `wifi: Association refused too many times, max allowed 1` | ESP32 nyangkut di state koneksi lama / router menolak sementara | **RESET BERSIH:** unplug USB ESP32 (tunggu 5 dtk), disconnect+reconnect Wi-Fi di laptop, colok USB lagi, tekan EN. Opsional tambah `WiFi.disconnect(true); delay(1000);` sebelum `WiFi.begin()`. Terbukti berhasil → ESP32 dapat IP 192.168.1.28. |

> CATATAN WI-FI (sering kejadian): kalau muncul "Association refused" padahal SSID/password benar & 2.4GHz, lakukan RESET BERSIH di atas. Biasanya langsung connect.

---

## 7. Perbandingan MQTT vs CoAP (ringkasan setup — untuk BAB IV)

| Aspek | MQTT | CoAP |
|-------|------|------|
| Model | publish-subscribe | request-response |
| Transport | TCP | UDP |
| Perantara | broker (Mosquitto) | tidak ada (langsung ke server) |
| Port | 1883 | 5683 |
| Server/broker di laptop | Mosquitto | Python aiocoap |
| Library ESP32 | PubSubClient | coap-simple-library |
| Target di kode ESP32 | topik `smartwater/data` | resource `/sensor` (PUT) |
| Payload | JSON identik | JSON identik |

---

## 8. Checklist mulai sesi CoAP

1. [ ] Laptop + ESP32 di Wi-Fi rumah yang sama (2.4GHz)
2. [ ] Jalankan `python coap_server.py` → catat IP yang muncul
3. [ ] Update `coap_server` IP + SSID/password di kode ESP32 (bila berubah)
4. [ ] Upload kode ESP32 (trik BOOT)
5. [ ] Cek console server: "[CoAP] Diterima (PUT)" + ESP32: "Response: OK"
6. [ ] Alirkan air → cek data sensor nyata

---

## 9. Yang BELUM dilakukan (tahap pengujian kinerja)
- [ ] Tes dengan AIR MENGALIR (data sensor nyata)
- [ ] Rapikan INA219 (current nan) untuk ukur energy
- [ ] Ukur parameter: latency, throughput, packet loss, energy, reliability
- [ ] Jalankan MQTT & CoAP di kondisi identik → bandingkan
- [ ] Logging ke CSV untuk analisis statistik
