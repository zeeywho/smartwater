# Hasil Pengujian RELIABILITY MQTT vs CoAP — RANGKUMAN

> Reliability diukur sebagai **Packet Delivery Ratio (PDR) / success rate** = persentase
> transmisi yang berhasil (data sampai + terkonfirmasi) terhadap total transmisi.
> Dirangkum dari data yang SUDAH ada (uji latency STEP 11 + uji throughput STEP 12) —
> tidak perlu uji baru, sesuai definisi reliability pada referensi.
>
> Rujukan metode: [M2] Mishra 2026 (Reliability = PDR %), [R4] Silva 2021 (packet loss),
> [M7] Naik (QoS → reliability). Semua angka PERSIS dari hasil pengujian.
>
> Catatan penting: kegagalan sesaat (TIMEOUT/CONNECT_FAIL) berasal dari ketidakstabilan
> hotspot HP, bukan karakteristik protokol; untuk run valid keduanya mendekati 100%.

---

## 1. PDR dari UJI LATENCY (STEP 11 — 30 iterasi/kombinasi)

| Protokol | Payload | Berhasil | PDR (%) |
|----------|---------|----------|---------|
| CoAP | 16 B | 30/30 | 100.00 |
| CoAP | 64 B | 30/30 | 100.00 |
| CoAP | 256 B | 29/30 | 96.67 |
| MQTT | 16 B | 29/30 | 96.67 |
| MQTT | 64 B | 29/30 | 96.67 |
| MQTT | 256 B | 29/30 | 96.67 |

PDR rata-rata (latency): CoAP = (100+100+96.67)/3 = **98.89%** ; MQTT = **96.67%**.

## 2. PDR dari UJI THROUGHPUT (STEP 12 — 100 pesan × 3 ulangan, run valid)

| Protokol | Payload | PDR (%) run valid |
|----------|---------|-------------------|
| CoAP | 16 B | 100.00 |
| CoAP | 64 B | 100.00 |
| CoAP | 256 B | 100.00 |
| MQTT | 16 B | 100.00 |
| MQTT | 64 B | 100.00 |
| MQTT | 256 B | 100.00 |

PDR rata-rata (throughput, run valid): **100%** untuk kedua protokol.

---

## 3. RINGKASAN RELIABILITY (untuk BAB IV 4.6.3.5 & 4.7.5)

| Protokol | PDR Latency (avg) | PDR Throughput (avg) | Reliability keseluruhan |
|----------|-------------------|----------------------|-------------------------|
| **MQTT** | 96.67% | 100% | Tinggi & stabil (TCP menjamin pengiriman) |
| **CoAP** | 98.89% | 100% | Tinggi; sedikit turun di payload besar (UDP) |

---

## 4. OBSERVASI (bahan analisis 4.7.5)
- Kedua protokol menunjukkan **reliability tinggi** (PDR ≥ 96.67% di uji latency; 100% di
  uji throughput pada run valid).
- **MQTT (TCP)** secara teori menjamin pengiriman (retransmisi), konsisten dengan PDR
  stabil 96.67–100%.
- **CoAP (UDP)** andal pada payload kecil–menengah (100%), sedikit menurun pada payload
  256 B (96.67% di uji latency) — sesuai sifat UDP yang tidak menjamin pengiriman.
- Kegagalan sesaat (TIMEOUT/CONNECT_FAIL) yang teramati lebih dipengaruhi ketidakstabilan
  hotspot HP daripada perbedaan protokol; diperlakukan sama untuk keduanya (fair).

## 5. CATATAN METODOLOGI & SARAN
- Definisi reliability = PDR mengikuti [M2] Mishra & [R4] Silva (mayoritas literatur).
- **OPSIONAL (bila diminta dosen):** uji durasi panjang (kirim kontinu ~30 menit) untuk
  reliability jangka panjang — belum dilakukan; rencana ada di `rencana-pengujian.md` (S4).
- **⚠️ KONFIRMASI DOSEN:** apakah reliability cukup dari PDR (sudah ada), atau perlu uji
  durasi panjang tambahan.
