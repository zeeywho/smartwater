# Lembar Pengujian Deteksi Kebocoran PER-TITIK (v2 final) — siap isi

> Pasangan `01b-metodologi-deteksi-PERTITIK-final.md`. Firmware: `deteksi-kebocoran-v2.ino`.
> Konsep: NORMAL=flow 0 ; BOCOR TITIK 1=Flow1 mengalir ; BOCOR TITIK 2=Flow2 mengalir.
> Angka = PERSIS dari Serial. AMBANG_FLOW = 0.5 L/min. N = ___ per kondisi (konfirmasi dosen).

## LEMBAR 0 — Sesi
- Tanggal/jam: ______  | Galon penuh: [ ] | IP ESP32: ______ | IP laptop: ______
- Catatan: _______________________________________________

## LEMBAR N — UJI NORMAL (kran utama buka; cabang-1 & cabang-2 TUTUP)
| Uji | flow1 | flow2 | pressure_adc | status | titik | Benar? (harusnya NORMAL) |
|-----|-------|-------|--------------|--------|-------|--------------------------|
| 1 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | ___ | [ ]✓ [ ]✗ |
| 2 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | ___ | [ ]✓ [ ]✗ |
| 3 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | ___ | [ ]✓ [ ]✗ |
| 4 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | ___ | [ ]✓ [ ]✗ |
| 5 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | ___ | [ ]✓ [ ]✗ |
| ...(s/d N) | | | | | | |

## LEMBAR B1 — UJI BOCOR TITIK 1 (buka cabang-1)
| Uji | flow1 | flow2 | pressure_adc | status | titik | Benar? (harusnya BOCOR TITIK 1) |
|-----|-------|-------|--------------|--------|-------|--------------------------------|
| 1 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| 2 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| 3 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| 4 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| 5 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| ...(s/d N) | | | | | | |

## LEMBAR B2 — UJI BOCOR TITIK 2 (buka cabang-2)
| Uji | flow1 | flow2 | pressure_adc | status | titik | Benar? (harusnya BOCOR TITIK 2) |
|-----|-------|-------|--------------|--------|-------|--------------------------------|
| 1 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| 2 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| 3 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| 4 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| 5 | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]T1 [ ]T2 | [ ]✓ [ ]✗ |
| ...(s/d N) | | | | | | |

## LEMBAR H — HITUNG (setelah uji)
Akurasi deteksi (NORMAL vs BOCOR) = (deteksi benar / total) × 100% = ___ %
Akurasi lokalisasi (titik 1 vs 2, dari kasus BOCOR) = (lokasi benar / total bocor) × 100% = ___ %
Confusion matrix 3 kelas (NORMAL / BOCOR T1 / BOCOR T2) — isi jumlah benar/salah tiap kelas.
Catatan pressure (pendukung): tekanan saat NORMAL = ___ (tinggi?), saat BOCOR = ___ (drop?).

## LEMBAR S — SCREENSHOT DASHBOARD
- [ ] NORMAL (hijau) → file: ______
- [ ] BOCOR TITIK 1 → file: ______
- [ ] BOCOR TITIK 2 → file: ______
