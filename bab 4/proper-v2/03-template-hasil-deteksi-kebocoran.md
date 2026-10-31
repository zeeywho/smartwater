# Template Penyajian Hasil Deteksi Kebocoran — v2

> Kerangka penyajian hasil yang PROFESIONAL & berujukan (meniru [M9], [M-TL]).
> SEMUA angka masih KOSONG (`___`) — DIISI dari data REAL uji galon-penuh. Jangan dikarang.
> Sumber data: file CSV dari logging otomatis (lihat `02-setup-logging-csv-otomatis.md`).

---

## 1. Kondisi & Setup Uji (isi saat sesi)

| Item | Isian |
|------|-------|
| Tanggal / jam | ______ |
| Galon terisi penuh? | [ ] Ya |
| Threshold hasil kalibrasi | ______ L/min |
| Selisih normal (rata-rata) saat kalibrasi | ______ L/min |
| Jumlah percobaan NORMAL (N) | ______ |
| Jumlah percobaan BOCOR (N) | ______ |
| Variasi bukaan keran bocor | [ ] kecil [ ] sedang [ ] besar |
| File CSV sesi ini | ______ |

---

## 2. Kalibrasi Threshold (dari kondisi NORMAL) — data REAL

> Ambil beberapa baris Serial/CSV saat NORMAL stabil (galon penuh, kedua cabang tutup).

| Sampel | flow1 | flow2 | selisih |
|--------|-------|-------|---------|
| 1 | ___ | ___ | ___ |
| 2 | ___ | ___ | ___ |
| 3 | ___ | ___ | ___ |
| 4 | ___ | ___ | ___ |
| 5 | ___ | ___ | ___ |
| **Selisih normal maks** | | | **___** |

→ **THRESHOLD ditetapkan = ___ L/min** (sedikit di atas selisih normal maks). Alasan: __________

---

## 3. Data Mentah per Percobaan (excerpt — ala [M9] Table 1)

### 3a. Kondisi NORMAL (kedua cabang TUTUP)
| Uji ke- | flow1 | flow2 | selisih | pressure_adc | status sistem | benar? |
|---------|-------|-------|---------|--------------|---------------|--------|
| 1 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 2 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 3 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 4 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 5 | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |

### 3b. Kondisi BOCOR (cabang-1 BUKA)
| Uji ke- | bukaan | flow1 | flow2 | selisih | pressure_adc | status sistem | benar? |
|---------|--------|-------|-------|---------|--------------|---------------|--------|
| 1 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 2 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 3 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 4 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |
| 5 | ___ | ___ | ___ | ___ | ___ | [ ]NORMAL [ ]BOCOR | [ ]✓ [ ]✗ |

---

## 4. Confusion Matrix (ala [M-TL] Cheng 2025)

> TP=BOCOR terdeteksi benar · TN=NORMAL terdeteksi benar · FP=NORMAL disangka BOCOR (alarm palsu)
> · FN=BOCOR lolos (disangka NORMAL).

| | Prediksi NORMAL | Prediksi BOCOR |
|---|---|---|
| **Aktual NORMAL** | TN = ___ | FP = ___ |
| **Aktual BOCOR** | FN = ___ | TP = ___ |

---

## 5. Metrik Evaluasi (hitung dari confusion matrix)

| Metrik | Rumus | Hasil |
|--------|-------|-------|
| Accuracy | (TP+TN)/(TP+TN+FP+FN) | ___ % |
| Precision | TP/(TP+FP) | ___ % |
| Recall | TP/(TP+FN) | ___ % |
| F1-Score | 2·(P·R)/(P+R) | ___ % |

---

## 6. Grafik (untuk skripsi — buat dari CSV di Excel/Sheets)

- **Grafik 1:** flow1 & flow2 vs waktu, kondisi NORMAL (dua garis berdekatan). [file: ___]
- **Grafik 2:** flow1 & flow2 vs waktu, kondisi BOCOR (flow1 ≫ flow2). [file: ___]
- **Grafik 3:** bar chart metrik (Accuracy/Precision/Recall/F1). [file: ___]

## 7. Screenshot Dashboard (lampiran)
- [ ] Dashboard kondisi NORMAL (indikator hijau) — file: ___
- [ ] Dashboard kondisi BOCOR (indikator merah) — file: ___

---

## 8. Ringkasan untuk BAB IV 4.6.2 (isi setelah data lengkap)
- Threshold terkalibrasi: ___ L/min.
- Akurasi deteksi: ___ %.
- Temuan: ______________________
- Keterbatasan (debit gravitasi, Flow2 under-read): ______________________
