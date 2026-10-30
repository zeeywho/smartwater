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

## CoAP — DATA MENTAH (BELUM — menyusul)

### CoAP 16 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | ___/100 | ___ | ___ | ___ | ___ |
| 2 | ___/100 | ___ | ___ | ___ | ___ |
| 3 | ___/100 | ___ | ___ | ___ | ___ |
| **Rata-rata** | | | ___ | ___ | ___ |

### CoAP 64 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | ___/100 | ___ | ___ | ___ | ___ |
| 2 | ___/100 | ___ | ___ | ___ | ___ |
| 3 | ___/100 | ___ | ___ | ___ | ___ |
| **Rata-rata** | | | ___ | ___ | ___ |

### CoAP 256 byte
| Ulang | Sukses | Waktu (s) | msg/s | bps | Success % |
|-------|--------|-----------|-------|-----|-----------|
| 1 | ___/100 | ___ | ___ | ___ | ___ |
| 2 | ___/100 | ___ | ___ | ___ | ___ |
| 3 | ___/100 | ___ | ___ | ___ | ___ |
| **Rata-rata** | | | ___ | ___ | ___ |

---

## TABEL RINGKAS THROUGHPUT (untuk BAB IV 4.6.3.2 & 4.7.2)

### Throughput rata-rata (bps)
| Payload | MQTT | CoAP | Lebih tinggi |
|---------|------|------|--------------|
| 16 B | 3812.94 | ___ | ___ |
| 64 B | 14797.70 | ___ | ___ |
| 256 B | 52932.25 | ___ | ___ |

### Throughput rata-rata (msg/s)
| Payload | MQTT | CoAP |
|---------|------|------|
| 16 B | 29.79 | ___ |
| 64 B | 28.90 | ___ |
| 256 B | 25.84 | ___ |

---

## OBSERVASI MQTT (bahan analisis)
- msg/s sedikit menurun saat payload membesar (29.79 → 28.90 → 25.84): pesan lebih besar
  butuh waktu kirim lebih lama → laju pesan turun.
- bps MEANINGKAT konsisten (3813 → 14798 → 52932) seiring payload membesar (16→64→256 B):
  lebih banyak byte terkirim per satuan waktu.
- Success rate 100% di semua payload MQTT (TCP andal).
