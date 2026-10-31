# Metodologi Pengujian Deteksi Kebocoran (Berujukan) — v2

> Tujuan: menetapkan METODE pengujian deteksi kebocoran yang punya dasar jurnal,
> termasuk JUMLAH PERCOBAAN (N) dan cara EVALUASI, agar hasil tidak dianggap "angka asal".
> Prinsip dosen: angka = hasil alat sendiri (PERSIS dari Serial); metode = merujuk literatur.

---

## 1. Prinsip deteksi (berujukan)

Sistem mendeteksi kebocoran dengan membandingkan **debit air masuk (Flow1/inlet) vs
debit air keluar (Flow2/outlet)**. Jika selisih (Flow1 − Flow2) melebihi ambang (threshold),
berarti ada air yang "hilang" di antara kedua sensor → **BOCOR**.

| Dasar | Rujukan |
|-------|---------|
| Deteksi berbasis sensor flow YF-S201 (sensor sama dgn penelitian ini) | [M9] Ayamga & Nakpih (2024) |
| Logika "bandingkan pembacaan sensor berurutan; beda pembacaan = bocor pada pipa sebelum sensor bernilai lebih rendah" | [M9] Ayamga (Algoritma 1) |
| Deteksi via perbandingan flow inlet vs outlet | [M10] Sikkandhar (2026) |
| Tekanan berfluktuasi/menurun di sekitar titik bocor (indikator pendukung) | [M12] Ali (2022) |

> Catatan [M9]: pada YF-S201, aliran lemah membuat turbin berputar lambat (pembacaan rendah),
> dan sistem perlu dikalibrasi dengan mengalirkan air beberapa kali sampai pembacaan stabil
> sebelum pengambilan data. Ini menjadi dasar prosedur kalibrasi kita (galon penuh).

---

## 2. Variabel pengujian

- **Variabel bebas:** kondisi keran cabang (titik bocor) — TUTUP (normal) vs BUKA (bocor).
- **Variasi tingkat bocor (opsional, memperkuat):** besar bukaan keran cabang
  (kecil / sedang / besar) — meniru pendekatan variasi bukaan valve [M-TL] Cheng et al. (2025)
  yang memakai bukaan 22.5°/45°/60°/90° untuk mewakili ukuran bocor berbeda.
- **Variabel terikat:** status sistem (NORMAL/BOCOR) dan kebenaran deteksi (benar/salah).
- **Kontrol:** galon diisi penuh (tekanan cukup agar Flow1 & Flow2 sama-sama terbaca),
  posisi perangkat tetap, firmware & threshold sama.

Susunan (terkonfirmasi): `Galon → keran utama → Flow1 → cabang-1(bocor) → Flow2 → cabang-2(bocor) → Pressure → keluar`.

---

## 3. JUMLAH PERCOBAAN (N) — dengan dasar rujukan ⭐

> Konteks berbeda dari uji kinerja protokol. Uji kinerja = uji STATISTIK (N=30 mengacu [R1]).
> Deteksi kebocoran = uji FUNGSIONAL/KLASIFIKASI → dievaluasi dgn akurasi/confusion matrix,
> bukan sekadar rata-rata. Maka dasar N-nya berbeda.

**Pendekatan yang dipakai (berujukan):**
- Evaluasi deteksi sebagai **tugas klasifikasi** (NORMAL vs BOCOR) → gunakan
  **Confusion Matrix + Accuracy, Precision, Recall, F1-Score**, mengacu metode evaluasi
  [M-TL] Cheng et al. (2025).
- **Ulangi tiap kondisi beberapa kali** dan **variasikan besar bukaan** (kecil/sedang/besar)
  mengikuti pendekatan variasi bukaan valve [M-TL], serta validasi pola Serial per kondisi
  mengikuti [M9] Ayamga.

**Usulan N (dikonfirmasi ke dosen):**
- **Minimal 5 percobaan per kondisi** (5× NORMAL + 5× BOCOR = 10 percobaan), ATAU
- **10 percobaan per kondisi** bila dosen ingin lebih kuat (10× NORMAL + 10× BOCOR = 20).
- Bila memakai variasi bukaan: mis. 5× NORMAL + (BOCOR kecil 3× + sedang 3× + besar 3×).

> Kalimat skripsi (contoh): "Pengujian deteksi dievaluasi sebagai tugas klasifikasi
> (NORMAL/BOCOR) menggunakan confusion matrix dan metrik Accuracy, Precision, Recall, dan
> F1-Score mengacu pada Cheng dkk. (2025), dengan variasi tingkat bukaan keran untuk mewakili
> ukuran kebocoran yang berbeda. Logika deteksi berbasis perbandingan debit inlet–outlet
> mengacu pada Ayamga & Nakpih (2024) dan Sikkandhar (2026)."

**⚠️ KONFIRMASI DOSEN:** setujui jumlah N (5 atau 10 per kondisi) + apakah pakai variasi bukaan.

---

## 4. Prosedur (urut)

1. **Kalibrasi:** isi galon penuh; alirkan NORMAL (kedua cabang tutup); amati Serial sampai
   Flow1 & Flow2 stabil (buang udara). Catat selisih normal → tetapkan threshold sedikit di
   atas selisih normal maksimum.
2. **Uji NORMAL (N×):** kedua cabang tutup → catat flow1, flow2, selisih, pressure, status, benar/salah.
3. **Uji BOCOR (N×):** buka cabang-1 (variasikan bukaan bila diminta) → catat sama.
4. **Hitung metrik:** susun confusion matrix → Accuracy, Precision, Recall, F1.
5. **Dokumentasi:** screenshot dashboard NORMAL (hijau) / BOCOR (merah).

---

## 5. Metrik evaluasi (rumus — mengacu [M-TL] Cheng et al. 2025)

Dengan TP = BOCOR terdeteksi benar, TN = NORMAL terdeteksi benar,
FP = NORMAL tapi sistem bilang BOCOR (alarm palsu), FN = BOCOR tapi sistem bilang NORMAL (bocor lolos):

```
Accuracy  = (TP + TN) / (TP + TN + FP + FN)
Precision = TP / (TP + FP)
Recall    = TP / (TP + FN)
F1-Score  = 2 × (Precision × Recall) / (Precision + Recall)
```

Confusion matrix (format [M-TL]):
| | Prediksi NORMAL | Prediksi BOCOR |
|---|---|---|
| Aktual NORMAL | TN | FP |
| Aktual BOCOR | FN | TP |

---

## 6. Pemetaan ke BAB IV
- Metode ini → mengisi **4.6.2 Pengujian Deteksi Kebocoran**.
- Hasil (confusion matrix + akurasi) → **4.7 / pembahasan**.
- Keterbatasan (Flow2 under-read pada aliran lemah, debit gravitasi) → **4.8 Pembahasan**
  (didukung [M9]: aliran lemah → turbin lambat; perlu kalibrasi aliran berulang).
