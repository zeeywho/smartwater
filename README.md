# Skripsi — Analisis Kinerja Protokol Komunikasi IoT (MQTT & CoAP) pada Sistem Smart Water Usage untuk Deteksi Kebocoran Pipa

> **Catatan scope (per revisi supervisi):** Penelitian ini membandingkan **MQTT dan CoAP** saja. **LoRa telah dihapus** dari cakupan. Gateway/server menggunakan **laptop**, bukan Raspberry Pi.

## Struktur Folder

```
/skripsi
  /bab4          -> Draf BAB IV (Markdown per bagian)
  /diagrams      -> File diagram (.drawio untuk draw.io, .mmd untuk Mermaid)
  /firmware      -> Kode Arduino + setup/log (MQTT STEP 8, CoAP STEP 9, Dashboard STEP 10)
  /dashboard     -> Flow Node-RED siap import (flow-dashboard.json)
  README.md
```

## Status Pengerjaan

| Bagian | Status |
|--------|--------|
| Kerangka BAB IV | ✅ Dibuat (kerangka + placeholder) |
| 4.1 Implementasi Sistem | 🚧 Dikerjakan bertahap |
| Diagram arsitektur (.drawio) | ✅ Draf awal (rancangan, belum terverifikasi hardware) |
| Tes hardware STEP 1–7 |✅ |
| MQTT / CoAP / Dashboard | MQTT ✅, CoAP ✅, Dashboard 🚧 (JSON masuk, visual + jembatan CoAP On Progress — lihat `firmware/setup-dashboard.md`)|

## Penanda Penting di Dokumen

- `[ISI SETELAH PENGUJIAN]` — bagian yang menunggu data eksperimen nyata. **Tidak boleh diisi hasil karangan.**
- `[RANCANGAN — BELUM DIVERIFIKASI]` — bagian rancangan yang harus dikonfirmasi ulang setelah tes hardware.
