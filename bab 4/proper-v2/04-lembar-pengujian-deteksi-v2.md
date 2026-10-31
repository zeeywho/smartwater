# Lembar Pengujian Deteksi Kebocoran (v2 — confusion-matrix-ready)

> Lembar siap-cetak untuk diisi SAAT uji di lab. Fokus deteksi kebocoran (galon penuh).
> Berpasangan dengan `03-template-hasil-deteksi-kebocoran.md` (penyajian) &
> `01-metodologi-deteksi-kebocoran-berujukan.md` (metode + N). Angka = PERSIS dari Serial/CSV.

---

## LEMBAR 0 — Identitas Sesi
- Tanggal/jam: __________   Penguji: __________
- Galon penuh? [ ] Ya   | Threshold terpasang di firmware: ______ L/min
- SSID: ______  | IP laptop: ______  | IP ESP32: ______
- File CSV logging: __________ (dari Node-RED/Python)
- Catatan kondisi: __________________________________

## LEMBAR K — KALIBRASI (wajib dulu)
Alirkan NORMAL (kedua cabang TUTUP), tunggu stabil, ambil 5 baris:
| # | flow1 | flow2 | selisih |
|---|-------|-------|---------|
| 1 | ___ | ___ | ___ |
| 2 | ___ | ___ | ___ |
| 3 | ___ | ___ | ___ |
| 4 | ___ | ___ | ___ |
| 5 | ___ | ___ | ___ |
- Selisih normal maksimum = ______ → **THRESHOLD = ______** (sedikit di atasnya) → upload ulang firmware.

## LEMBAR N — UJI NORMAL (kedua cabang TUTUP)  [N = ___ , konfirmasi dosen]
| Uji | flow1 | flow2 | selisih | pressure | status sistem | Benar? (harusnya NORMAL) |
|-----|-------|-------|---------|----------|---------------|--------------------------|
| 1 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 2 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 3 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 4 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 5 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| (lanjut bila N>5) | | | | | | |

## LEMBAR B — UJI BOCOR (buka cabang-1)  [N = ___]
| Uji | bukaan (kecil/sedang/besar) | flow1 | flow2 | selisih | pressure | status sistem | Benar? (harusnya BOCOR) |
|-----|------------------------------|-------|-------|---------|----------|---------------|--------------------------|
| 1 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 2 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 3 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 4 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 5 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| (lanjut bila N>5) | | | | | | | |

## LEMBAR H — HITUNG (setelah uji)
Isi dari dua lembar di atas:
- TP (BOCOR benar) = ___ | TN (NORMAL benar) = ___ | FP (NORMAL→BOCOR) = ___ | FN (BOCOR→NORMAL) = ___
- Accuracy = (TP+TN)/total = ___ %
- Precision = TP/(TP+FP) = ___ % | Recall = TP/(TP+FN) = ___ % | F1 = ___ %

## LEMBAR S — SCREENSHOT
- [ ] Dashboard NORMAL (hijau) disimpan: ______
- [ ] Dashboard BOCOR (merah) disimpan: ______

## CHECKLIST sebelum mulai
- [ ] Galon penuh | [ ] Mosquitto + Node-RED jalan | [ ] Logging CSV aktif
- [ ] Threshold sudah dikalibrasi & di-upload | [ ] Kedua cabang bisa buka/tutup
