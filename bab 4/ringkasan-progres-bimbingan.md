# Ringkasan Progres untuk Bimbingan — Smart Water Usage (MQTT vs CoAP)

> Lembar bantu bimbingan. Berisi APA yang sudah dikerjakan sejak pertemuan lalu,
> TEMUAN pengujian (data real alat), dan PERTANYAAN/keputusan yang diminta ke dosen.
> Catatan: semua angka = hasil pengujian alat sendiri (Serial Monitor), bukan karangan.

---

## 1. Posisi dibanding bimbingan sebelumnya

| | Bimbingan lalu | Sekarang |
|---|---|---|
| Uji kinerja protokol (latency, packet loss, throughput, reliability) | sudah selesai | sudah selesai (tetap) |
| **Deteksi kebocoran (fungsi utama)** | **belum diuji** (baru upload kode) | **sudah diuji berjam-jam**, deteksi BOCOR terbukti bekerja |
| Pemindahan ke tulisan BAB IV | — | **belum** (fokus minggu ini di pengujian dulu) |

> Inti bimbingan hari ini: **progres & temuan pengujian deteksi kebocoran**, belum tulisan jadi.

---

## 2. Yang dikerjakan minggu ini (pengujian deteksi kebocoran)

- Rangkaian: Galon → keran utama → **Flow1** → cabang-1 (ke ember) → **Flow2** → cabang-2 (ke ember) → **Pressure** → keluar.
  Dua keran cabang = titik simulasi bocor (air dibuang ke ember).
- Logika deteksi: **selisih Flow1 − Flow2**; bila > ambang → status **BOCOR** (rujukan [M9] YF-S201, [M10] flow inlet vs outlet).
- Dilakukan: alirkan air, amati Serial Monitor (CSV: waktu, flow1, flow2, selisih, pressure_adc, status), kirim ke dashboard Node-RED via MQTT.

---

## 3. Temuan pengujian (data REAL — ada di `data-pengujian-deteksi-kebocoran.csv`)

1. **Deteksi BOCOR terbukti bekerja.** Saat keran cabang dibuka (simulasi bocor), Flow1 tetap mengalir
   namun Flow2 turun drastis → selisih besar → status berubah jadi **BOCOR** (contoh: Flow1≈2.27, Flow2≈0,
   selisih≈2.27). Status kembali NORMAL saat aliran berhenti.
2. **Faktor kritis = volume air di galon.** Saat air galon sedikit, tekanan (head) rendah → turbin flow
   tidak berputar (terbaca 0). Setelah galon **diisi penuh + ditinggikan**, Flow1 naik (≈2.27 L/min) dan
   **Flow2 mulai ikut terbaca** (0.13–0.27 L/min).
3. **Keterbatasan: Flow2 cenderung under-read.** Karena posisinya di hilir (tekanan sudah turun setelah
   Flow1), nilai Flow2 jauh lebih kecil dari Flow1 bahkan saat mengalir. Ini perlu dipertimbangkan saat
   menetapkan ambang (threshold) deteksi.

---

## 4. Yang BELUM selesai (jujur)

- **Kalibrasi ambang (threshold) final** — perlu satu sesi uji **NORMAL bersih** (kedua cabang tutup, galon
  penuh, Flow1 & Flow2 mengalir stabil bersamaan) untuk menentukan selisih normal → set threshold di atasnya.
- **Uji terstruktur**: NORMAL 5× + BOCOR 5× → hitung akurasi deteksi (belum dilakukan).
- **Screenshot dashboard** NORMAL (hijau) / BOCOR (merah) untuk lampiran skripsi.
- Pemindahan hasil ke tulisan BAB IV (4.6.2).

---

## 5. Pertanyaan / keputusan yang diminta ke dosen

1. **Jumlah iterasi (N) untuk uji deteksi kebocoran** berapa? (Uji kinerja protokol memakai N=30 mengacu
   [R1]. Deteksi kebocoran belum punya acuan N — mohon arahan: misal 5×/kondisi atau lebih.)
2. **Penanganan Flow2 yang under-read**: apakah cukup dengan mengkalibrasi threshold dari kondisi normal
   (selisih normal → ambang di atasnya), atau disarankan pakai indikator pendukung **tekanan** (pressure)
   sesuai [M12] Ali 2022?
3. **Energy consumption (INA219 dipasang seri)** — apakah masuk scope final skripsi atau boleh di-drop?
4. **Metode latency** (time-to-completion/RTT tanpa NTP, mengacu [R1]) — konfirmasi disetujui.

---

## 6. Rencana setelah bimbingan

1. Uji **NORMAL bersih** (galon penuh, kedua cabang tutup) → kalibrasi threshold.
2. Uji **NORMAL 5× + BOCOR 5×** → hitung akurasi → isi Lembar 2 (`lembar-pengujian.md`).
3. Screenshot dashboard NORMAL/BOCOR.
4. Pindahkan hasil ke BAB IV 4.6.2 + pembahasan keterbatasan (debit gravitasi, Flow2 under-read).
5. (Bila disetujui) uji energy; uji payload JSON asli (STEP 13) sebagai penutup.

---

## 7. Dokumen pendukung (bukti pengujian)
- `firmware/log-pengujian.md` — catatan kronologis pengujian (termasuk STEP 14 deteksi kebocoran).
- `bab 4/data-pengujian-deteksi-kebocoran.csv` — cuplikan data Serial (bisa dibuka di Excel/Sheets).
- `bab 4/lembar-pengujian.md` — Lembar 2 (siap diisi uji NORMAL/BOCOR).
- `bab 4/rancangan-pengujian-berujukan.md` — metode + rujukan jurnal.
