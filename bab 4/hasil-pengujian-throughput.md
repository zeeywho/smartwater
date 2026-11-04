# Hasil Pengujian THROUGHPUT MQTT vs CoAP (STEP 12) — DATA MENTAH

> Metode: kirim 100 pesan BERUNTUN (tanpa jeda), hitung bps & msg/s. Rujuk [M2] Mishra
> (definisi bps) + [M-arXiv] Krikorian (pengiriman kontinu). N=100 (titik tengah 50–500
> Mishra, realistis utk hardware). Ulang 3× per kombinasi → rata-rata.
> Jaringan: hotspot HP "zee" (2.4 GHz), laptop 172.20.10.3, ESP32 172.20.10.2.
> MQTT: Mosquitto + echo_mqtt.py (sukses = echo diterima). CoAP: aiocoap (sukses = ACK CON).
>
> ANGKA DICATAT PERSIS dari Serial Monitor (tidak dibulatkan). Tanpa air.

---

## MQTT — DATA MENTAH (SELESAI ✅)

### MQTT 16 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | 100/100 | 3.352 | 29.83 | 3818.62 | 100.00 |
| 2 | 100/100 | 3.356 | 29.80 | 3814.06 | 100.00 |
| 3 | 100/100 | 3.363 | 29.74 | 3806.13 | 100.00 |
| **Rata-rata** | | | **29.79** | **3812.94** | **100.00** |

### MQTT 64 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | 100/100 | 3.463 | 28.88 | 14784.87 | 100.00 |
| 2 | 100/100 | 3.459 | 28.91 | 14801.97 | 100.00 |
| 3 | 100/100 | 3.458 | 28.92 | 14806.25 | 100.00 |
| **Rata-rata** | | | **28.90** | **14797.70** | **100.00** |

### MQTT 256 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | 100/100 | 3.895 | 25.67 | 52580.23 | 100.00 |
| 2 | 100/100 | 3.886 | 25.73 | 52702.01 | 100.00 |
| 3 | 100/100 | 3.827 | 26.13 | 53514.50 | 100.00 |
| **Rata-rata** | | | **25.84** | **52932.25** | **100.00** |

> Catatan: saat MQTT 256B, terjadi 1 run gagal connect broker (GAGAL connect / Association
> refused) akibat hotspot HP sesaat → run itu TIDAK dihitung; diulang hingga 3 run valid.
> (Catat juga di Lembar 5 kendala.)

---

## CoAP — DATA MENTAH (SELESAI ✅)

> Catatan: CoAP butuh buffer coap-simple `BUF_MAX_SIZE=512` untuk payload 256 B
> (default 128 → 256 B TIMEOUT semua). Run awal yang terdampak hotspot HP (variatif /
> success <100%) DIBUANG & diulang hingga 3 run valid (kondisi bersih), demi konsistensi.

### CoAP 16 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | 100/100 | 0.983 | 101.73 | 13021.36 | 100.00 |
| 2 | 100/100 | 1.062 | 94.16 | 12052.73 | 100.00 |
| 3 | 100/100 | 1.087 | 92.00 | 11775.53 | 100.00 |
| **Rata-rata** | | | **95.96** | **12283.21** | **100.00** |

### CoAP 64 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | 100/100 | 0.871 | 114.81 | 58783.01 | 100.00 |
| 2 | 100/100 | 0.814 | 122.85 | 62899.26 | 100.00 |
| 3 | 100/100 | 0.848 | 117.92 | 60377.36 | 100.00 |
| **Rata-rata** | | | **118.53** | **60686.54** | **100.00** |

### CoAP 256 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | 100/100 | 0.835 | 119.76 | 245269.47 | 100.00 |
| 2 | 100/100 | 1.009 | 99.11 | 202973.25 | 100.00 |
| 3 | 100/100 | 1.054 | 94.88 | 194307.39 | 100.00 |
| **Rata-rata** | | | **104.58** | **214183.37** | **100.00** |

> (Run CoAP 256B yang dibuang: 33.37 msg/s / 99% — hotspot goyang + 1 timeout; tidak dipakai.)

---

## TABEL RINGKAS THROUGHPUT (untuk BAB IV 4.6.3.2 & 4.7.2)

### Throughput rata-rata (bps)
| Payload | MQTT | CoAP | Lebih tinggi |
|---------|------|------|--------------|
| 16 B | 3812.94 | 12283.21 | **CoAP** (~3,2×) |
| 64 B | 14797.70 | 60686.54 | **CoAP** (~4,1×) |
| 256 B | 52932.25 | 214183.37 | **CoAP** (~4,0×) |

### Throughput rata-rata (msg/s)
| Payload | MQTT | CoAP | Lebih tinggi |
|---------|------|------|--------------|
| 16 B | 29.79 | 95.96 | **CoAP** |
| 64 B | 28.90 | 118.53 | **CoAP** |
| 256 B | 25.84 | 104.58 | **CoAP** |

### Success rate (%)
| Payload | MQTT | CoAP |
|---------|------|------|
| 16 B | 100.00 | 100.00 |
| 64 B | 100.00 | 100.00 |
| 256 B | 100.00 | 100.00 |
(semua 100% pada run valid; catatan kegagalan sesaat akibat hotspot ada di Lembar 5)

## OBSERVASI CoAP & PERBANDINGAN (bahan analisis 4.7.2)
- **CoAP throughput jauh lebih tinggi dari MQTT** di semua payload (sekitar 3–4× dalam bps).
  Sesuai teori: CoAP/UDP ringan, tanpa handshake/koneksi persisten → laju lebih tinggi.
- CoAP jauh lebih cepat per transaksi (uji 0,8–1,1 detik) vs MQTT (3,3–3,9 detik) karena
  MQTT menunggu echo round-trip pada koneksi TCP.
- bps kedua protokol NAIK seiring payload membesar (lebih banyak byte per satuan waktu).
- Catatan metodologi: CoAP lebih sensitif terhadap ketidakstabilan hotspot (timeout CON
  memperlambat saat ada loss); run bersih diambil setelah reset jaringan agar adil.

---

## OBSERVASI MQTT (bahan analisis)
- msg/s sedikit menurun saat payload membesar (29.79 → 28.90 → 25.84): pesan lebih besar
  butuh waktu kirim lebih lama → laju pesan turun.
- bps MEANINGKAT konsisten (3813 → 14798 → 52932) seiring payload membesar (16→64→256 B):
  lebih banyak byte terkirim per satuan waktu.
- Success rate 100% di semua payload MQTT (TCP andal).
