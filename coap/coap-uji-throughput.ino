/*
 * STEP 12 — CoAP UJI THROUGHPUT
 * Metode mengacu [M2] Mishra 2026 (definisi throughput bps) + [M-arXiv] Krikorian 2023
 * (pengiriman kontinu). BERBEDA dari uji latency: di sini kirim BERUNTUN tanpa jeda.
 *
 * Yang diukur:
 *  - Throughput (bps)   = (pesan_sukses × payload_byte × 8) / total_waktu_detik
 *  - Throughput (msg/s) = pesan_sukses / total_waktu_detik
 *  - Packet success (sukses/total)
 *
 * N = 100 pesan per kombinasi (titik tengah realistis 50–500 ala Mishra, utk hardware).
 * Payload 16/64/256 byte (set PAYLOAD_SIZE). CoAP mode CON (tunggu ACK = sukses).
 * Ulang uji 3× (upload/EN ulang) → ambil rata-rata.
 *
 * TIDAK perlu air. Hanya ESP32 + Wi-Fi + CoAP server (aiocoap) di laptop.
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// ====== ISI SENDIRI ======
const char* ssid     = "zee";
const char* password = "satuduatiga";
IPAddress coap_server(172, 20, 10, 3);     // IP laptop (CoAP server) - cek ipconfig
// ==========================

// ====== PARAMETER UJI ======
#define PAYLOAD_SIZE 16      // <-- GANTI: 16, 64, atau 256
const int   N_PESAN        = 100;    // jumlah pesan beruntun (lihat justifikasi di dokumen)
const unsigned long ACK_TIMEOUT_MS = 2000;
// ============================

WiFiUDP udp;
Coap coap(udp);

volatile bool  ackReceived = false;
char payload[300];

void buildPayload() {
  const char* base = "{\"d\":\"";
  const char* tail = "\"}";
  int baseLen = strlen(base) + strlen(tail);   // 8
  int padLen = PAYLOAD_SIZE - baseLen;
  if (padLen < 0) padLen = 0;
  if (padLen > (int)sizeof(payload) - baseLen - 1) padLen = sizeof(payload) - baseLen - 1;
  int i = 0;
  i += snprintf(payload + i, sizeof(payload) - i, "%s", base);
  for (int k = 0; k < padLen; k++) payload[i++] = 'x';
  snprintf(payload + i, sizeof(payload) - i, "%s", tail);
}

void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  ackReceived = true;
}

void konekWifi() {
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print(" OK, IP ESP32: "); Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== STEP 12: CoAP UJI THROUGHPUT ===");
  Serial.print("PAYLOAD_SIZE = "); Serial.println(PAYLOAD_SIZE);
  Serial.print("N_PESAN = "); Serial.println(N_PESAN);
  Serial.println("Metode: kirim beruntun (tanpa jeda), hitung bps & msg/s. CoAP CON.");

  konekWifi();
  coap.response(callback_response);
  coap.start();
  buildPayload();
  int plen = strlen(payload);
  Serial.print("Payload ("); Serial.print(plen); Serial.println(" byte)");

  int sukses = 0;
  unsigned long tMulai = millis();

  for (int it = 1; it <= N_PESAN; it++) {
    ackReceived = false;
    coap.put(coap_server, 5683, "sensor", payload);
    unsigned long tStart = millis();
    while (!ackReceived && (millis() - tStart < ACK_TIMEOUT_MS)) {
      coap.loop();
    }
    if (ackReceived) sukses++;
    // tanpa jeda antar pesan (ukur kapasitas maksimum)
  }

  unsigned long tSelesai = millis();
  float totalDetik = (tSelesai - tMulai) / 1000.0;
  float msgRate = sukses / totalDetik;
  float bps = (sukses * (float)plen * 8.0) / totalDetik;

  Serial.println("=== HASIL THROUGHPUT ===");
  Serial.print("Pesan sukses: "); Serial.print(sukses); Serial.print("/"); Serial.println(N_PESAN);
  Serial.print("Total waktu (s): "); Serial.println(totalDetik, 3);
  Serial.print("Throughput (msg/s): "); Serial.println(msgRate, 2);
  Serial.print("Throughput (bps): "); Serial.println(bps, 2);
  Serial.print("Success rate (%): "); Serial.println((sukses * 100.0) / N_PESAN, 2);
  Serial.println("Catat ke lembar. Ulang 3x, lalu ganti PAYLOAD_SIZE.");
}

void loop() { }
