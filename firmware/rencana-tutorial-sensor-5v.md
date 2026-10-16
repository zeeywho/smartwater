# Rencana Tutorial Tahap Berikutnya — Sensor 5V (Flow ×2 + Pressure + Gabungan)

> Status: MENUNGGU adaptor 5V 2A fixed datang. Semua langkah di sini dieksekusi SETELAH adaptor ada.
> Prinsip: tes satu per satu dulu, jangan langsung gabungan. Foto wiring sebelum nyalakan.

---

## 0. Persiapan Power — Adaptor 5V (WAJIB sebelum sensor 5V)

**Rangkaian power yang dituju:**
```
Adaptor 5V (colok listrik)
  → konektor DC female + terminal screw hijau (SUDAH PUNYA)
  → kabel + ke rail MERAH (+) breadboard
  → kabel - ke rail BIRU (-) breadboard
```

**Langkah:**
1. Colok steker DC adaptor 5V ke konektor female hijau (pastikan steker 5.5x2.1mm cocok).
2. Dari terminal screw: kabel kutub **+** → rail **+** (merah) breadboard; kutub **−** → rail **−** (biru) breadboard.
3. ⚠️ **CEK POLARITAS** sebelum nyalakan: mayoritas adaptor = center positive (pin tengah = +).
   - [PERLU KONFIRMASI SAAT EKSEKUSI] Pastikan mana kabel + dan − dari konektor hijau.
   - Tanpa multimeter: ikuti label +/− di konektor hijau + spesifikasi "center positive" di body adaptor.
4. ⚠️ **GND HARUS COMMON:** rail − (dari adaptor) harus tersambung ke GND ESP32 juga.
   Caranya: 1 kabel jumper dari rail − breadboard → pin GND ESP32.
   (Kalau GND tidak common, sinyal sensor tidak akan terbaca benar.)
5. ESP32 tetap di-power dari USB laptop (untuk upload + Serial Monitor).

**Checklist aman sebelum colok listrik adaptor:**
- [ ] + adaptor → rail + ; − adaptor → rail −
- [ ] rail − tersambung ke GND ESP32 (common ground)
- [ ] tidak ada 5V nyasar ke pin ESP32 selain lewat jalur yang disengaja
- [ ] foto wiring sudah dikirim & diverifikasi

---

## STEP 4 & 5 — Flow Sensor YF-S201 (×2)

### Spesifikasi & isu yang HARUS dicek
- YF-S201: VCC 5–18V, GND, SIGNAL (pulsa). Prinsip Hall effect.
- ⚠️ [PERLU KONFIRMASI] Level tegangan pin SIGNAL. YF-S201 di-supply 5V → sinyal pulsa bisa mendekati 5V.
  - GPIO ESP32 = 3.3V. Idealnya sinyal 5V TIDAK langsung ke GPIO.
  - OPSI AMAN: pasang **voltage divider** pada jalur SIGNAL (sama seperti pressure), ATAU gunakan level yang sudah aman bila sensor di-pull-up ke 3.3V.
  - Keputusan final diambil saat eksekusi setelah cek sinyal.
- ⚠️ GPIO34 & GPIO35 = INPUT-ONLY, TIDAK punya internal pull-up. Flow sensor sering butuh pull-up.
  - Flow 1 rencana di GPIO34 → mungkin butuh resistor pull-up eksternal (mis. 10kΩ ke 3V3).
  - [PERLU KONFIRMASI] uji dulu; bila pulsa tidak terbaca, tambah pull-up eksternal.

### Wiring Flow Sensor 1 (rencana)
| Kabel sensor | Ke |
|--------------|-----|
| VCC (merah) | rail + (5V) |
| GND (hitam) | rail − (GND common) |
| SIGNAL (kuning) | GPIO34 (via voltage divider / pull-up sesuai hasil cek) |

### Wiring Flow Sensor 2 (rencana)
| Kabel sensor | Ke |
|--------------|-----|
| VCC | rail + (5V) |
| GND | rail − (GND) |
| SIGNAL | GPIO27 (GPIO biasa, punya pull-up internal) |

### Cara tes
- Kode: hitung pulsa pakai interrupt. Target STEP 4/5 = KONFIRMASI PULSA TERDETEKSI (belum kalibrasi).
- Uji dengan meniup/memutar turbin sensor atau mengalirkan air → cek nilai pulsa naik di Serial Monitor.
- (kode akan disiapkan saat eksekusi)

---

## STEP 6 — Pressure Sensor WISNER WPT-83G (0–12 Bar)

### Spesifikasi & isu
- ⚠️ [PERLU KONFIRMASI] Output sensor: tegangan (0.5–4.5V, 3 kabel) ATAU arus (4–20mA, 2 kabel)?
  - Foto label/kabel sensor saat eksekusi untuk pastikan.
  - Jika 3 kabel (V output) → pakai voltage divider.
  - Jika 4–20mA → butuh resistor beban + perhitungan beda (bukan voltage divider biasa).
- Jika output 0.5–4.5V: pakai **voltage divider 15kΩ + 10kΩ**:
  - Vout = Vin × 10/(15+10). Pada 4.5V → 1.8V (aman untuk ADC 3.3V). ✅

### Wiring (rencana, asumsi output tegangan 3 kabel)
```
Sinyal sensor —[15kΩ]—•—[10kΩ]— GND
                       |
                     GPIO35 (ADC)
VCC sensor → rail + (5V)
GND sensor → rail − (GND)
```

### Cara tes
- Target STEP 6 = KONFIRMASI pembacaan ADC + cek nilai tegangan setelah divider (kalibrasi ke bar menyusul).
- (kode akan disiapkan saat eksekusi)

---

## STEP 7 — Gabungan Semua Sensor

Setelah STEP 4, 5, 6 masing-masing LULUS:
- Gabungkan Flow1 + Flow2 + Pressure + INA219 dalam satu sketch.
- Output Serial Monitor target:
  - Flow 1 (pulsa / L/min)
  - Flow 2 (pulsa / L/min)
  - Pressure (ADC / tegangan)
  - Voltage / Current / Power (INA219)
- Format data mengarah ke JSON (lihat 4.2.5) untuk dipakai MQTT & CoAP.
- (kode gabungan disiapkan saat eksekusi)

---

## Hal yang WAJIB dikonfirmasi saat eksekusi (jangan ditebak)
1. Polaritas konektor DC hijau (+/−) + center-positive adaptor.
2. Level tegangan sinyal YF-S201 (perlu divider/pull-up atau tidak).
3. Tipe output pressure sensor WPT-83G (tegangan vs 4–20mA).
4. Apakah GPIO34 butuh pull-up eksternal untuk flow sensor.

## Prinsip keamanan (tetap berlaku)
- Tes satu komponen per waktu.
- Foto wiring sebelum nyalakan; verifikasi dulu.
- GND harus selalu common.
- Jangan pakai MP1584 (belum terukur). Pakai adaptor 5V fixed.
- Jangan solder sampai wiring terbukti benar di breadboard.
