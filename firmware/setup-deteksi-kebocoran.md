# Setup & Prosedur DETEKSI KEBOCORAN (STEP 14 — FUNGSI UTAMA)

> End-to-end: sensor nyata → deteksi NORMAL/BOCOR → tampil di dashboard.
> PERLU AIR MENGALIR + keran T-junction untuk simulasi bocor.
> Rujukan metode: [M9] Ayamga (YF-S201), [M10] Sikkandhar (flow inlet vs outlet),
> [M12] Ali (tekanan). Lembar catat: `bab 4/lembar-pengujian.md` (Lembar 2).

---

## A. Refresh cara nyalakan DASHBOARD (Node-RED)

### 1. Jalankan broker Mosquitto (biar data MQTT masuk)
```
cd "C:\Program Files\mosquitto"
mosquitto -c C:\mqtt\mosquitto.conf -v
```

### 2. Jalankan Node-RED (CMD baru)
```
node-red
```
→ Editor flow: http://localhost:1880
→ Dashboard: http://localhost:1880/dashboard

### 3. Flow sudah ada (dari STEP 10): `dashboard/flow-dashboard.json`
```
mqtt in (smartwater/data) → json → function → gauge Flow1/Flow2/Pressure + chart + text STATUS
```
Kalau flow hilang → menu ☰ → Import → pilih `dashboard/flow-dashboard.json`.

---

## B. Penyesuaian dashboard utk status BOCOR

Kode deteksi (`deteksi-kebocoran.ino`) sekarang mengirim JSON yang SUDAH berisi field
`status` ("NORMAL"/"BOCOR") dan `selisih`. Jadi node text tinggal menampilkan `msg.payload.status`.

Cek node **text STATUS** di dashboard:
- Value/format: `{{msg.payload.status}}`  (status sudah dihitung di ESP32)
- (opsional) beri warna: hijau bila NORMAL, merah bila BOCOR

> Catatan: dulu node `function` di dashboard menghitung status dari selisih. Sekarang
> status dihitung di ESP32 (lebih sederhana). Dua-duanya boleh — yang penting status muncul.
> Kalau mau tetap hitung di Node-RED, pakai `msg.payload.selisih` di function.

Gauge yang relevan utk deteksi:
- Gauge Flow1 → `msg.payload.flow1`
- Gauge Flow2 → `msg.payload.flow2`
- (opsional) Gauge/teks Selisih → `msg.payload.selisih`
- Text Status → `msg.payload.status`

---

## C. Prosedur Pengujian Deteksi Kebocoran (urut)

### C.1 KALIBRASI dulu (tentukan threshold) — PENTING
1. Upload `deteksi-kebocoran.ino` (isi ssid/password/IP dulu).
2. Alirkan air **kondisi NORMAL** (semua air lewat kedua sensor, keran T-junction TUTUP).
3. Lihat Serial → amati kolom `selisih` saat normal (idealnya kecil, mendekati 0).
4. Catat selisih normal (mis. 0.2–0.3 L/min karena sensor tidak 100% identik).
5. Set `THRESHOLD_SELISIH` sedikit DI ATAS selisih normal (mis. normal 0.3 → threshold 0.5).
6. Upload ulang dengan threshold final.

### C.2 Uji SKENARIO (catat ke Lembar 2)
| Uji | Aksi | Ekspektasi |
|-----|------|------------|
| NORMAL | air lewat kedua sensor, keran T-junction TUTUP | Flow1≈Flow2, status NORMAL |
| BOCOR | buka keran T-junction (air keluar di tengah) | Flow1>Flow2, status BOCOR |

- Ulang beberapa kali (mis. 5× tiap kondisi) → hitung akurasi deteksi.
- Variasikan besar bukaan (bocor kecil vs besar) bila bisa.
- Amati di DASHBOARD: gauge Flow bergerak + text Status berubah NORMAL↔BOCOR.

### C.3 Yang dicatat (Lembar 2)
- Flow1, Flow2, selisih, pressure_adc, status sistem, benar/tidak.
- Akurasi = (deteksi benar / total uji) × 100%.

---

## D. Format data Serial (CSV, utk dicatat)
```
waktu(s),flow1,flow2,selisih,pressure_adc,status
12,5.20,4.85,0.35,461,NORMAL
13,5.30,2.10,3.20,430,BOCOR
```

## E. Validasi fungsional sekalian (Black Box)
- F10 Tampil dashboard → data + status muncul di dashboard ✅
- F11 Deteksi kebocoran → status BOCOR muncul saat keran dibuka ✅

## F. Checklist sebelum uji
- [ ] Air & pipa siap (keran T-junction bisa dibuka/tutup)
- [ ] Flow1 (GPIO25), Flow2 (GPIO27), Pressure (GPIO34) terpasang di pipa
- [ ] Mosquitto + Node-RED jalan; dashboard terbuka
- [ ] IP laptop benar di kode (ipconfig)
- [ ] Threshold dikalibrasi dari kondisi NORMAL

## G. Troubleshooting
| Isu | Solusi |
|-----|--------|
| Flow 0 terus saat air mengalir | cek wiring flow sensor (GPIO25/27), keran benar terbuka |
| selisih besar padahal normal | sensor tidak identik → naikkan threshold sesuai kalibrasi |
| status BOCOR terus | threshold terlalu kecil → naikkan |
| status NORMAL terus saat bocor | threshold terlalu besar → turunkan |
| data tak masuk dashboard | cek Mosquitto jalan + IP + node json di flow |
