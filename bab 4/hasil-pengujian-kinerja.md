# Hasil Pengujian Kinerja MQTT vs CoAP (STEP 11) — DATA MENTAH

> Tanggal uji: (isi). Metode [R1]: cold transaction, 30 iterasi/kombinasi, jeda 4000 ms,
> payload 16/64/256 byte, MQTT QoS0 + echo RTT, CoAP CON.
> Jaringan: hotspot HP "zee" (2.4 GHz), gateway laptop (172.20.10.3), ESP32 (172.20.10.2).
> Latency = RTT (ms). status OK/TIMEOUT/CONNECT_FAIL.
>
> CATATAN: hotspot HP kurang stabil → beberapa TIMEOUT/CONNECT_FAIL akibat gangguan
> jaringan (bukan karakteristik protokol). Lihat bagian "Catatan & keputusan".
> Buffer library dinaikkan untuk payload 256 B (coap-simple BUF_MAX_SIZE=512;
> PubSubClient setBufferSize(1024)) — temuan teknis, catat di pembahasan.

---

## Ringkasan (mean dari alat, sebelum hitung median/std)

| # | Protokol | Payload | Success Rate | Mean Latency | Catatan |
|---|----------|---------|--------------|--------------|---------|
| 1 | CoAP | 16 B | 100% (30/30) | 156.23 ms | ada outlier cold-start awal → pertimbangkan ulang |
| 2 | CoAP | 64 B | 100% (30/30) | 16.60 ms | bersih, stabil |
| 3 | CoAP | 256 B | 96.67% (29/30) | 50.45 ms | 1 TIMEOUT; buffer dinaikkan |
| 4 | MQTT | 16 B | 96.67% (29/30) | 59.21 ms | 1 TIMEOUT |
| 5 | MQTT | 64 B | 96.67% (29/30) | 76.72 ms | 1 CONNECT_FAIL; outlier 1636 ms |
| 6 | MQTT | 256 B | 96.67% (29/30) | 30.21 ms | 1 CONNECT_FAIL; buffer dinaikkan |

---

## DATA MENTAH per kombinasi (iterasi,latency_ms,status)

### 1) CoAP — 16 byte  (Success 100%, Mean 156.23 ms)
> Data lengkap akan dilengkapi; tercatat ada outlier awal (mis. 1=280). Sebagian terekam:
> 22,14 / 23,127 / 24,42 / 25,18 / 26,14 / 27,21 / 28,12 / 29,21 / 30,11 (semua OK).
> (TODO: salin 30 baris penuh dari hasil_coap_16.txt)

### 2) CoAP — 64 byte  (Success 100%, Mean 16.60 ms)
```
1,18   2,27   3,41   4,10   5,9    6,11   7,39   8,12   9,29   10,29
11,40  12,28  13,11  14,10  15,11  16,10  17,10  18,9   19,10  20,8
21,11  22,10  23,11  24,10  25,12  26,18  27,19  28,11  29,12  30,12
```
semua OK (30/30).

### 3) CoAP — 256 byte  (Success 96.67%, Mean 50.45 ms)
```
1,109  2,13   3,9    4,38   5,11   6,49   7,31   8,12   9,227  10,13
11,190 12,15  13,127 14,10  15,12  16,119 17,11  18,TIMEOUT  19,341  20,11
21,10  22,10  23,12  24,10  25,14  26,10  27,11  28,12  29,9   30,17
```
TIMEOUT di iterasi 18 (29/30 OK).

### 4) MQTT — 16 byte  (Success 96.67%, Mean 59.21 ms)
```
1,44   2,78   3,49   4,19   5,58   6,23   7,102  8,24   9,23   10,32
11,28  12,34  13,313 14,294 15,23  16,18  17,19  18,TIMEOUT  19,43  20,26
21,15  22,96  23,61  24,22  25,27  26,18  27,58  28,102 29,47  30,21
```
TIMEOUT di iterasi 18 (29/30 OK).

### 5) MQTT — 64 byte  (Success 96.67%, Mean 76.72 ms)
```
1,20   2,25   3,1636 4,20   5,19   6,19   7,CONNECT_FAIL  8,21   9,19   10,18
11,18  12,20  13,18  14,16  15,18  16,30  17,54  18,22  19,19  20,20
21,19  22,19  23,19  24,22  25,20  26,18  27,22  28,18  29,18  30,18
```
CONNECT_FAIL di iterasi 7 (29/30 OK). Outlier besar: iterasi 3 = 1636 ms.

### 6) MQTT — 256 byte  (Success 96.67%, Mean 30.21 ms)
```
1,26   2,31   3,CONNECT_FAIL  4,22   5,22   6,24   7,25   8,23   9,27   10,27
11,22  12,70  13,27  14,41  15,27  16,22  17,24  18,24  19,22  20,25
21,63  22,23  23,27  24,24  25,30  26,29  27,22  28,60  29,28  30,39
```
CONNECT_FAIL di iterasi 3 (29/30 OK).

---

## Catatan & keputusan (perlu konfirmasi dosen)

1. **CoAP 16 B** mean 156 ms tidak sebanding (ketarik outlier cold-start di awal).
   → OPSI: ulang run CoAP 16 B di kondisi stabil agar sebanding.
2. **TIMEOUT / CONNECT_FAIL** sesekali = akibat ketidakstabilan hotspot HP, BUKAN
   karakteristik protokol. Perlakuan harus SAMA untuk MQTT & CoAP (fair).
   → OPSI A: hitung sebagai kegagalan (success rate < 100%) + catat di pembahasan.
   → OPSI B: ulang run yang ada fail agar bersih.
3. **Outlier besar** (MQTT 64 iterasi3=1636; CoAP 16 awal) → gunakan MEDIAN selain mean.
4. **Buffer library** dinaikkan untuk payload 256 B (coap-simple 512; PubSubClient 1024).
   → temuan: payload besar butuh penyesuaian buffer di perangkat constrained.
5. **Konsistensi jaringan:** semua 6 kombinasi di hotspot "zee" (konsisten). Bila nanti
   pindah Wi-Fi rumah, ulang SEMUA agar fair.

## Statistik yang akan dihitung (langkah berikutnya)
- Per kombinasi: mean, median, std dev, min, max, success rate.
- Median lebih representatif (tahan outlier).
- Tabel ringkas MQTT vs CoAP per payload → BAB IV 4.7.
