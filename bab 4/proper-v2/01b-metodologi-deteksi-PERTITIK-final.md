# Metodologi Deteksi Kebocoran — KONSEP PER-TITIK (FINAL) — v2

> Menggantikan konsep "selisih Flow1−Flow2" (file `01-...`) yang TIDAK cocok dengan
> karakteristik alat (pipa ujung tertutup, aliran tidak kontinu saat normal).
> Konsep ini SESUAI kondisi alat apa adanya & sudah TERBUKTI pada data uji.
> Prinsip dosen: angka = PERSIS dari Serial; metode = berujukan.

---

## 1. Latar: kenapa ganti dari "selisih" ke "per-titik"

Rancangan awal memakai deteksi **selisih debit inlet–outlet** (Flow1 − Flow2), mengacu
[M10]. Namun pengujian menunjukkan metode ini **tidak dapat divalidasi** pada alat ini:

- Ujung pipa **tertutup** (terpasang sensor tekanan, bukan outlet) → pada kondisi NORMAL
  (semua keran cabang tutup) **air tidak mengalir keluar** → Flow1 = Flow2 = 0 → selisih
  tidak bermakna. Kondisi "Flow1 ≈ Flow2 keduanya mengalir" (syarat metode selisih) tidak
  pernah tercapai.
- Aliran hanya terjadi saat salah satu keran cabang dibuka (air mendapat jalan keluar).

→ Karena itu dipilih metode yang **sesuai karakteristik sistem tertutup** ini: **deteksi
per-titik berbasis aliran cabang**, dengan tekanan sebagai pendukung.

> Catatan kejujuran (untuk pembahasan/BAB IV): perubahan metode ini adalah keputusan
> metodologis berdasarkan temuan karakteristik alat — BUKAN kegagalan. Didokumentasikan apa adanya.

---

## 2. Konsep PER-TITIK

Susunan: `Galon → Kran Utama → [Flow1] → Cabang-1 → [Flow2] → Cabang-2 → [Pressure] (ujung tertutup)`

```
NORMAL          : semua keran cabang TUTUP → air tak mengalir keluar → Flow1=0, Flow2=0
                  (tekanan cenderung TINGGI karena sistem tertutup)
BOCOR TITIK 1   : keran cabang-1 BUKA → air mengalir keluar via cabang-1 → Flow1 mendeteksi aliran
BOCOR TITIK 2   : keran cabang-2 BUKA → air mengalir keluar via cabang-2 → Flow2 mendeteksi aliran
```

- **Deteksi UTAMA = flow** (menentukan ADA bocor + LOKASI titik).
- **Pendukung = pressure** (cenderung DROP saat ada aliran keluar/bocor; sistem tertutup →
  tekanan tinggi saat normal, turun saat bocor). Tidak dipakai sebagai penentu status.

Logika (di `firmware/deteksi-kebocoran-v2.ino`):
```
if (flow1 > AMBANG && flow1 >= flow2) -> BOCOR, TITIK_1
else if (flow2 > AMBANG)              -> BOCOR, TITIK_2
else                                  -> NORMAL
```
AMBANG = 0.5 L/min (di atas noise 0.1–0.3, di bawah aliran bocor nyata 1.0–2.5 — dari data uji).

Rujukan: [M9] Ayamga & Nakpih (2024) — deteksi berbasis flow YF-S201 + lokalisasi titik;
[M10] Sikkandhar (2026) — sensor flow pada titik distribusi; [M12] Ali (2022) — tekanan
sebagai indikator pendukung.

---

## 3. Kelebihan metode per-titik untuk alat ini

- ✅ Sesuai sistem tertutup (tak butuh aliran seri kontinu saat normal).
- ✅ Dapat **melokalisasi** kebocoran (titik 1 / titik 2) — nilai tambah.
- ✅ Tiap sensor independen (tak perlu dua sensor seimbang seperti metode selisih).
- ✅ Sudah terbukti pada data uji (keran-1 buka → Flow1 baca; keran-2 buka → Flow2 baca).

Keterbatasan (diakui jujur di pembahasan):
- Mendeteksi "ada aliran di titik rawan", mengasumsikan titik bocor berada pada cabang
  yang dipasangi sensor. Bocor di luar titik bersensor tidak terdeteksi langsung (pressure
  jadi cadangan). Wajar untuk prototipe.

---

## 4. Apakah TUJUAN & RUMUSAN MASALAH tetap terjawab? → YA

Judul skripsi: "Analisis Kinerja Protokol Komunikasi IoT (MQTT & CoAP) pada Sistem Smart
Water Usage untuk Deteksi Kebocoran Pipa."

- **Fokus UTAMA = perbandingan kinerja MQTT vs CoAP** (latency, throughput, packet loss,
  reliability) → SUDAH selesai, data real, TIDAK terpengaruh perubahan metode deteksi.
- **Deteksi kebocoran = konteks/aplikasi** tempat data sensor dihasilkan & dikirim via 2
  protokol. Dengan metode per-titik, sistem **tetap berfungsi mendeteksi (dan melokalisasi)
  kebocoran** → konteks terpenuhi.

→ Tujuan tercapai; rumusan masalah (kinerja protokol + kemampuan deteksi) tetap terjawab.

---

## 5. Prosedur uji (konsep per-titik)

Gunakan `firmware/deteksi-kebocoran-v2.ino`. N = 10 per kondisi (konfirmasi dosen).
Evaluasi: confusion matrix 3 kelas + akurasi (mengacu [M-TL] Cheng 2025).

| Kondisi | Aksi | Ekspektasi |
|---------|------|------------|
| NORMAL | kran utama buka, cabang-1 & cabang-2 TUTUP | Flow1=0, Flow2=0 → NORMAL |
| BOCOR TITIK 1 | buka cabang-1 | Flow1 mengalir → BOCOR TITIK 1 |
| BOCOR TITIK 2 | buka cabang-2 | Flow2 mengalir → BOCOR TITIK 2 |

Catat: flow1, flow2, pressure_adc, status, titik, benar/salah. Hitung akurasi deteksi +
akurasi lokalisasi. Pressure dicatat sebagai pendukung (amati drop saat bocor).
