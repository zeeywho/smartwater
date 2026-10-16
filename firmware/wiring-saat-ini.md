[wiring-saat-ini.md](https://github.com/user-attachments/files/33565258/wiring-saat-ini.md)
# Wiring Saat Ini — Smart Water Usage (MQTT + CoAP)

> Update terakhir: 1 Oktober 2026
> Status: STEP 3–6 LULUS (INA219, Flow1, Flow2, Pressure). STEP 7 (gabungan) kode disiapkan, belum diuji.
> Prinsip wiring: Flow sensor LANGSUNG ke GPIO ber-pull-up (tanpa divider). Pressure PAKAI divider (output tegangan 0.5–4.5V).

---

## RINGKASAN PETA PIN ESP32

| Pin ESP32 | Tersambung ke | Jenis |
|-----------|---------------|-------|
| GPIO25 | SIGNAL Flow Sensor 1 (langsung) | digital pulsa (INPUT_PULLUP) |
| GPIO27 | SIGNAL Flow Sensor 2 (langsung) | digital pulsa (INPUT_PULLUP) |
| GPIO34 | Output Pressure Sensor (via voltage divider) | analog ADC |
| GPIO21 | SDA INA219 | I2C |
| GPIO22 | SCL INA219 | I2C |
| 3V3 | VCC INA219 | power |
| GND | GND INA219 + common ground (rail −) | ground |
| USB | power ESP32 (dari laptop) | power |

GPIO35: belum dipakai (cadangan ADC).

---

## 1. POWER (adaptor 5V → breadboard)
| Dari | Ke |
|------|-----|
| Adaptor 5V kabel + (merah) | rail + (merah) breadboard |
| Adaptor 5V kabel − (hitam) | rail − (biru) breadboard |

ESP32 dipower terpisah via USB laptop (bukan dari adaptor).

## 2. COMMON GROUND (WAJIB)
| Dari | Ke |
|------|-----|
| rail − (biru) | pin GND ESP32 |

> Tanpa common ground, sinyal sensor (yang dipower adaptor) tidak terbaca benar oleh ESP32 (yang dipower USB).

## 3. INA219 (I2C) — STEP 3 ✅ LULUS
| Pin INA219 | Ke ESP32 |
|------------|----------|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |
| Vin+ / Vin− | KOSONG (dipasang nanti untuk ukur energy consumption) |

## 4. FLOW SENSOR 1 (YF-S201) — STEP 4 ✅ LULUS — tanpa divider
| Kabel | Warna | Ke |
|-------|-------|-----|
| VCC | merah | rail + (5V) |
| GND | hitam | rail − (GND) |
| SIGNAL | kuning | GPIO25 (LANGSUNG, tanpa resistor) |

Kode: pinMode INPUT_PULLUP, interrupt FALLING (gabungan) / CHANGE (diagnostik).

## 5. FLOW SENSOR 2 (YF-S201) — STEP 5 ✅ LULUS — tanpa divider
| Kabel | Warna | Ke |
|-------|-------|-----|
| VCC | merah | rail + (5V) |
| GND | hitam | rail − (GND) |
| SIGNAL | kuning | GPIO27 (LANGSUNG, tanpa resistor) |

## 6. PRESSURE SENSOR (WISNER WPT-83G, 0–12 Bar) — STEP 6 ✅ LULUS (fungsi) — PAKAI voltage divider
Spesifikasi (label): Supply 5–12VDC, Output 0.5–4.5V. Warna (label): Supply=merah, GND=hitam, Output=kuning.

### Koneksi power sensor
| Kabel | Warna | Ke |
|-------|-------|-----|
| Supply | merah | rail + (5V) |
| GND | hitam | rail − (GND) |

### Koneksi output via VOLTAGE DIVIDER → GPIO34
Divider menurunkan output 0.5–4.5V menjadi aman untuk ADC 3.3V.
Rumus: Vout = Vin × 10K/(20K+10K) = Vin × 0.333 → pada 4.5V = ~1.5V (aman).

LETAK RESISTOR DI BREADBOARD (sesuai pemasangan aktual, pola divider):
```
Output kuning (sensor) ─┐
                        ├─ baris INPUT ──[ R 20KΩ ]── baris TENGAH ─┬─► kabel ke GPIO34
                                                                     │
                                                   [ R 10KΩ ] antara │ baris TENGAH ── rail −
```
| Komponen | Kaki 1 | Kaki 2 | Keterangan |
|----------|--------|--------|------------|
| Kabel Output kuning | baris INPUT | — | masuk ke titik input divider |
| Resistor 20KΩ | baris INPUT | baris TENGAH | resistor "atas" (seri) |
| Resistor 10KΩ | baris TENGAH | rail − (GND) | resistor "bawah" (ke ground) |
| Kabel ke GPIO34 | baris TENGAH | pin GPIO34 ESP32 | output divider = titik tengah |

Aturan penting divider:
- Output kuning + kaki atas 20K → SATU baris (titik INPUT).
- Kaki bawah 20K + kaki atas 10K + kabel GPIO34 → SATU baris (titik TENGAH = output).
- Kaki bawah 10K → rail − (GND).
- Tiap resistor MENYEBERANG 2 baris berbeda (tidak boleh kedua kaki di baris sama).

> Catatan: baris INPUT & TENGAH = nomor baris breadboard aktual yang dipakai (area kosong). Yang penting POLA-nya benar, bukan nomor spesifik.

---

## CATATAN TEKNIS PENTING (bahan BAB IV & pembahasan)

### Flow sensor (YF-S201) — kenapa TANPA divider
- YF-S201 output open-collector → butuh pull-up, sinyal menarik ke LOW saat pulsa.
- Divider 20K/10K pada flow JUSTRU menghalangi sinyal (terbukti saat debug: Level pin stuck).
- Solusi: SIGNAL langsung ke GPIO ber-pull-up (GPIO25/27) + INPUT_PULLUP.
- Aman tanpa divider karena open-collector tidak mendorong tegangan >3.3V (hanya menarik ke LOW).
- GPIO34/35 TIDAK dipakai untuk flow → input-only tanpa pull-up → floating → noise/pulsa hantu.

### Pressure sensor — kenapa PAKAI divider
- WPT-83G benar-benar mendorong tegangan 0.5–4.5V (bukan open-collector).
- 4.5V > 3.3V batas GPIO → WAJIB diturunkan dengan voltage divider.
- GPIO34 (input-only ADC) cocok untuk sinyal analog ini (floating tidak masalah untuk ADC).

### Isu terbuka
- Kabel pressure sensor berujung SERABUT (belum ada pin) → koneksi kadang drop (ADC=0 sesaat).
  Perlu diperkuat (lilit ke pin jumper / solder pin header) sebelum pengambilan data serius.

### Power
- Sumber 5V sensor = ADAPTOR 5V 2A (fixed), BUKAN pin ESP32.
- ESP32 dari USB. Common ground menyatukan keduanya.
- Brownout pernah terjadi karena kabel USB kurang baik → diatasi dengan kabel USB lebih baik + port lain.
- MP1584 & 7805 TIDAK dipakai (adaptor 5V fixed lebih aman tanpa multimeter).
- LoRa module sudah dilepas (di luar scope).
