# Hasil Pengujian Kinerja MQTT vs CoAP (STEP 11) — DATA MENTAH & STATISTIK

> Metode [R1]: cold transaction, 30 iterasi/kombinasi, jeda 4000 ms,
> payload 16/64/256 byte, MQTT QoS0 (+echo RTT), CoAP CON. Latency = RTT (ms).
> Jaringan: hotspot HP "zee" (2.4 GHz), gateway laptop (172.20.10.3), ESP32 (172.20.10.2).
>
> CATATAN: hotspot HP kurang stabil → beberapa TIMEOUT/CONNECT_FAIL = gangguan jaringan
> (bukan karakteristik protokol). Buffer library dinaikkan untuk payload 256 B
> (coap-simple BUF_MAX_SIZE=512; PubSubClient setBufferSize(1024)) — temuan teknis.
>
> Semua data ASLI dari pengujian alat. Tidak ada angka karangan.

---

## TABEL STATISTIK DESKRIPTIF (per kombinasi)

| Protokol | Payload | N OK | Success % | Mean (ms) | Median (ms) | Std (ms) | Min | Max |
|----------|---------|------|-----------|-----------|-------------|----------|-----|-----|
| CoAP | 16 B | 30/30 | 100.00 | 15.37 | 11.0 | 10.70 | 9 | 63 |
| CoAP | 64 B | 30/30 | 100.00 | 16.60 | 11.0 | 10.12 | 8 | 41 |
| CoAP | 256 B | 29/30 | 96.67 | 50.45 | 12.0 | 80.15 | 9 | 341 |
| MQTT | 16 B | 29/30 | 96.67 | 59.21 | 32.0 | 72.40 | 15 | 313 |
| MQTT | 64 B | 29/30 | 96.67 | 76.72 | 19.0 | 299.97 | 16 | 1636 |
| MQTT | 256 B | 29/30 | 96.67 | 30.21 | 26.0 | 12.72 | 22 | 70 |

> Catatan: Mean beberapa kombinasi ketarik OUTLIER (mis. MQTT 64B ada 1636 ms; CoAP 256B
> ada 341 ms). MEDIAN lebih representatif untuk latency steady-state.

---

## TABEL PERBANDINGAN MQTT vs CoAP (untuk BAB IV 4.7)

### Latency MEDIAN (ms) — nilai utama (tahan outlier)
| Payload | MQTT | CoAP | Lebih cepat |
|---------|------|------|-------------|
| 16 B | 32.0 | 11.0 | **CoAP** |
| 64 B | 19.0 | 11.0 | **CoAP** |
| 256 B | 26.0 | 12.0 | **CoAP** |

### Latency MEAN (ms) — pelengkap
| Payload | MQTT | CoAP |
|---------|------|------|
| 16 B | 59.21 | 15.37 |
| 64 B | 76.72 | 16.60 |
| 256 B | 30.21 | 50.45 |

### Packet Delivery Success Rate (%)
| Payload | MQTT | CoAP |
|---------|------|------|
| 16 B | 96.67 | 100.00 |
| 64 B | 96.67 | 100.00 |
| 256 B | 96.67 | 96.67 |

---

## DATA MENTAH per kombinasi (latency ms; TIMEOUT/CONNECT_FAIL = gagal)

### 1) CoAP — 16 byte (100%, mean 15.37 / median 11.0)
```
21,9,11,11,11,11,22,10,15,10,14,11,22,16,10,9,10,13,35,10,10,12,18,11,63,23,13,10,10,10
```
semua OK (30/30).

### 2) CoAP — 64 byte (100%, mean 16.60 / median 11.0)
```
18,27,41,10,9,11,39,12,29,29,40,28,11,10,11,10,10,9,10,8,11,10,11,10,12,18,19,11,12,12
```
semua OK (30/30).

### 3) CoAP — 256 byte (96.67%, mean 50.45 / median 12.0)
```
109,13,9,38,11,49,31,12,227,13,190,15,127,10,12,119,11,[TIMEOUT iter18],341,11,10,10,12,10,14,10,11,12,9,17
```
TIMEOUT di iterasi 18 (29/30 OK). Buffer coap-simple dinaikkan ke 512.

### 4) MQTT — 16 byte (96.67%, mean 59.21 / median 32.0)
```
44,78,49,19,58,23,102,24,23,32,28,34,313,294,23,18,19,[TIMEOUT iter18],43,26,15,96,61,22,27,18,58,102,47,21
```
TIMEOUT di iterasi 18 (29/30 OK).

### 5) MQTT — 64 byte (96.67%, mean 76.72 / median 19.0)
```
20,25,1636,20,19,19,[CONNECT_FAIL iter7],21,19,18,18,20,18,16,18,30,54,22,19,20,19,19,19,22,20,18,22,18,18,18
```
CONNECT_FAIL iter7 (29/30 OK). Outlier besar iter3 = 1636 ms.

### 6) MQTT — 256 byte (96.67%, mean 30.21 / median 26.0)
```
26,31,[CONNECT_FAIL iter3],22,22,24,25,23,27,27,22,70,27,41,27,22,24,24,22,25,63,23,27,24,30,29,22,60,28,39
```
CONNECT_FAIL iter3 (29/30 OK). Buffer PubSubClient dinaikkan ke 1024.

---

## CATATAN & KEPUTUSAN (konfirmasi dosen)

1. **Nilai utama = MEDIAN** (tahan outlier). Mean dilaporkan sebagai pelengkap. Sesuai [R1]
   yang melaporkan mean + median + std + min/max.
2. **TIMEOUT/CONNECT_FAIL** = gangguan hotspot HP (bukan karakteristik protokol). Perlakuan
   SAMA untuk MQTT & CoAP (fair). Dihitung sebagai kegagalan → success rate < 100%.
3. **Outlier** (MQTT 64B 1636 ms; CoAP 256B 341 ms) dari gangguan jaringan sesaat →
   karena itu median dipakai sebagai nilai utama.
4. **Buffer library** dinaikkan untuk payload 256 B (coap-simple 512; PubSubClient 1024).
   Temuan: payload besar butuh penyesuaian buffer di perangkat constrained.
5. **Konsistensi jaringan:** semua 6 kombinasi di hotspot "zee" (konsisten/fair).

## RINGKASAN TEMUAN (bahan analisis BAB IV)
- Berdasarkan MEDIAN, **CoAP lebih rendah latency** di semua ukuran payload (11-12 ms)
  dibanding MQTT (19-32 ms). Sesuai teori: CoAP/UDP tanpa handshake, overhead lebih kecil.
- **CoAP success rate = 100%** di payload 16 & 64 B; turun ke 96.67% di 256 B.
- **MQTT success rate konsisten 96.67%** (dipengaruhi kestabilan hotspot).
- Variabilitas (std) MQTT 64B & CoAP 256B tinggi karena outlier gangguan jaringan.
