# 📦 PAKET PENYAJIAN DATA PROPER (v2) — Smart Water Usage

> Paket BARU berisi metodologi + sistem pengumpulan + template penyajian data yang
> terstruktur dan berujukan, meniru cara penyajian jurnal ([M9] Ayamga 2024,
> [M10] Sikkandhar, [M12] Ali 2022, [M-TL] Cheng et al. 2025, [R2]).
>
> ⚠️ CATATAN PENTING:
> - Paket ini TIDAK mengubah/menghapus file lama. Semua file lama tetap utuh.
> - Ini "kandidat" — dibuat agar penyajian data terlihat profesional & bukan "data asal".
>   Setelah dirasa OK (dan dikonfirmasi dosen), baru diputuskan apakah menggantikan yang lama.
> - SEMUA angka di template masih KOSONG (`___`) — diisi dari data REAL uji galon-penuh.
>   TIDAK ada angka karangan. (Aturan dosen: angka harus PERSIS dari Serial Monitor.)

---

## Isi paket

| File | Fungsi |
|------|--------|
| `01-metodologi-deteksi-kebocoran-berujukan.md` | Metode + rujukan + dasar jumlah N (jawaban "N berujukan" utk dosen) |
| `02-setup-logging-csv-otomatis.md` | Cara data Arduino → CSV otomatis via Node-RED (tak perlu copas manual) |
| `03-template-hasil-deteksi-kebocoran.md` | Kerangka penyajian hasil (tabel + confusion matrix + grafik), angka kosong |
| `04-lembar-pengujian-deteksi-v2.md` | Lembar isi saat uji (confusion-matrix-ready) |
| `05-rencana-figure-skripsi.md` | Daftar figure (ala [R2]) + data sumbernya (biar tak kerja 2×) |
| `06-PANDUAN-UJI-FINAL.md` | Panduan uji lengkap: setup+verifikasi → deteksi → JSON → energy(nanti) + pemetaan outline |
| `07-setup-jembatan-coap-dashboard.md` | Jembatan CoAP → dashboard (CoAP→MQTT re-publish); pakai `coap/coap_server_bridge.py` |

## Urutan pakai
1. Baca `01` → paham metode & N (bawa ke dosen utk konfirmasi angka N).
2. Pasang `02` → Node-RED nulis CSV otomatis.
3. Uji galon-penuh: NORMAL ×N + BOCOR ×N (variasi bukaan) → isi `04`.
4. Tuang hasil ke `03` (confusion matrix + akurasi) → bahan BAB IV 4.6.2.
5. `05` = peta figure utk penulisan (sebagian datanya sudah ada dari uji kinerja).

## Status keputusan
- [ ] Dosen konfirmasi jumlah N deteksi kebocoran
- [ ] Uji galon-penuh dilakukan (data real masuk)
- [ ] Putuskan: paket v2 ini menggantikan file lama, atau digabung
