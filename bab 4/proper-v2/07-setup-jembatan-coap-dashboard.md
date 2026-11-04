# Setup Jembatan CoAP → Dashboard (Opsi 1: CoAP → MQTT re-publish) — v2

> Tujuan: data CoAP dari ESP32 TAMPIL di dashboard Node-RED, tanpa mengubah arsitektur.
> Melengkapi F10 (dashboard CoAP). Disiapkan SEBELUM uji agar pas uji CoAP tinggal pakai.
>
> Konsep:
> ```
> ESP32 --CoAP PUT--> coap_server_bridge.py --re-publish--> Mosquitto --MQTT--> Node-RED
>                      (server CoAP + bridge)   topik 'smartwater/data-coap'     (dashboard)
> ```
> Data MQTT (topik `smartwater/data`) & CoAP (topik `smartwater/data-coap`) bisa tampil
> BERSAMAAN → tidak perlu switch-switch saat uji.

---

## 1. Prasyarat (sekali saja)
- [ ] Mosquitto sudah jalan (`mosquitto -c C:\mqtt\mosquitto.conf -v`).
- [ ] Node-RED jalan + flow `dashboard/flow-dashboard.json` sudah di-import.
- [ ] Install library Python bridge: `pip install paho-mqtt aiocoap`

## 2. Pakai server CoAP versi bridge
Gantikan `coap_server.py` lama dengan **`coap/coap_server_bridge.py`** (dari repo ini).
Isinya = server CoAP-mu yang lama + tambahan ~25 baris untuk re-publish ke MQTT
(logika CoAP TIDAK diubah; hanya menambah `republish_to_mqtt()`).

Jalankan:
```
python coap_server_bridge.py
```
Output yang diharapkan:
```
[BRIDGE] Terhubung ke broker MQTT 127.0.0.1:1883 -> re-publish ke topik 'smartwater/data-coap'
=== CoAP Server (+ jembatan MQTT) berjalan ===
IP server (laptop): 172.20.10.3  |  Port: 5683
```
Saat ESP32 kirim CoAP:
```
[CoAP] Diterima (PUT): {"flow1":...,"status":"NORMAL"}
[BRIDGE] -> MQTT 'smartwater/data-coap': {"flow1":...,"status":"NORMAL"}
```

> Catatan: broker (Mosquitto) & server CoAP di laptop yang SAMA → MQTT_BROKER = 127.0.0.1.
> Topik CoAP sengaja dibedakan (`smartwater/data-coap`) agar tidak tercampur data MQTT.

## 3. Tambah node di Node-RED (agar data CoAP tampil)

Di editor Node-RED (http://localhost:1880):

1. Seret node **`mqtt in`** baru.
   - Topic: **`smartwater/data-coap`**
   - Broker: pakai broker config yang sama (localhost:1883), QoS 0.
2. Seret node **`json`** (action: "ke object") → hubungkan dari `mqtt in` tadi.
3. Hubungkan outputnya ke widget dashboard. Dua pilihan:
   - **(ringkas)** ke gauge/chart yang SAMA dengan MQTT, tapi beri **label berbeda** (mis.
     grup "CoAP"), supaya terlihat dua sumber; ATAU
   - **(disarankan saat uji)** buat **grup dashboard "CoAP"** sendiri: gauge Flow1/Flow2/
     Pressure + text Status, pakai `msg.payload.flow1`, `.flow2`, `.pressure_adc`, `.status`.

Alur akhir (tambahan, berdampingan dgn alur MQTT yang sudah ada):
```
mqtt in (smartwater/data-coap) -> json -> gauge/chart/text (grup "CoAP")
```

4. **Deploy**.

## 4. Verifikasi (sebelum uji)
- [ ] Jalankan: Mosquitto + Node-RED + `coap_server_bridge.py`.
- [ ] Upload firmware CoAP ke ESP32 (yang mengirim PUT ke `/sensor`).
- [ ] Alirkan air / biarkan data terkirim → cek:
      - Terminal bridge: muncul `[CoAP] Diterima` + `[BRIDGE] -> MQTT`.
      - Dashboard grup "CoAP": gauge bergerak + status tampil. ✅
- [ ] (opsional) `mosquitto_sub -h localhost -t smartwater/data-coap -v` → lihat data CoAP mengalir.

## 5. Saat pengujian nanti
- MQTT & CoAP bisa tampil BERSAMAAN (dua grup dashboard) → tidak perlu ganti-ganti.
- Untuk uji kinerja (latency/throughput), tetap pakai kode uji masing-masing; dashboard ini
  untuk VISUALISASI/bukti fungsional (F10 CoAP), bukan alat ukur kinerja.

## 6. Troubleshooting
| Isu | Solusi |
|-----|--------|
| `[BRIDGE] gagal konek MQTT` | pastikan Mosquitto jalan dulu sebelum server CoAP |
| Data CoAP tak muncul di dashboard | cek topik node `mqtt in` = `smartwater/data-coap` (persis) |
| `ModuleNotFoundError: paho` | `pip install paho-mqtt` |
| Server CoAP tak bind | aiocoap bind ke IP laptop spesifik (sudah auto di skrip); cek IP benar |

---

## Catatan untuk BAB IV
- Jembatan ini melengkapi **F10 (tampil dashboard — CoAP)** → BAB IV 4.2/4.4 (fungsional).
- Metode: CoAP (request/response, UDP) tidak punya broker pub/sub, sehingga dijembatani ke
  MQTT agar dapat divisualisasikan pada dashboard yang sama — pendekatan bridge konsisten
  dengan [M6] Malhotra (CoAP-MQTT bridge) & [M-arXiv] Krikorian (middleware 2 protokol).
