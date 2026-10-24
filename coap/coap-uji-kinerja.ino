/*
 * STEP 11 — CoAP UJI KINERJA (metode [R1] Kirilov & Chikurtev 2026)
 * Mengukur: end-to-end latency (RTT via ACK CON), success rate, (throughput dihitung di laptop)
 *
 * Metode:
 *  - Cold transaction: tiap iterasi connect state baru -> kirim 1 PUT (CON) -> tunggu ACK -> catat.
 *  - 30 iterasi per kombinasi; jeda 4000 ms antar iterasi.
 *  - Payload 16 / 64 / 256 byte (set PAYLOAD_SIZE).
 *  - CoAP mode CON (Confirmable) -> server balas -> RTT.
 *
 * Output Serial (CSV): "iterasi,latency_ms,status"
 * Salin ke Lembar 3.A (bab 4/lembar-pengujian.md).
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// ====== ISI SENDIRI ======
const char* ssid     = "Nana";                 // Wi-Fi 2.4 GHz
const char* password = "71727579";
IPAddress coap_server(192, 168, 1, 10);        // IP laptop (CoAP server) - cek ipconfig
// ==========================

// ====== PARAMETER UJI (metode [R1]) ======
#define PAYLOAD_SIZE 16      // <-- GANTI: 16, 64, atau 256
const int   N_ITERASI      = 30;     // [R1]
const unsigned long JEDA_MS = 4000;  // [R1] inter-trial delay
const unsigned long ACK_TIMEOUT_MS = 2000; // batas tunggu ACK per iterasi
// =========================================

WiFiUDP udp;
Coap coap(udp);

volatile bool  ackReceived = false;
volatile unsigned long tAck = 0;

char payload[300];

// Bangun payload JSON berukuran ~PAYLOAD_SIZE byte (diisi field pad agar pas)
void buildPayload() {
  // basis JSON; sisanya diisi karakter 'x' pada field "pad" supaya total ~ PAYLOAD_SIZE
  // contoh basis ~ {"id":1,"v":0,"pad":""}
  const char* base = "{\"id\":1,\"v\":0,\"pad\":\"";
  const char* tail = "\"}";
  int baseLen = strlen(base) + strlen(tail);
  int padLen = PAYLOAD_SIZE - baseLen;
  if (padLen < 0) padLen = 0;
  if (padLen > (int)sizeof(payload) - baseLen - 1) padLen = sizeof(payload) - baseLen - 1;
  int i = 0;
  i += snprintf(payload + i, sizeof(payload) - i, "%s", base);
  for (int k = 0; k < padLen; k++) payload[i++] = 'x';
  snprintf(payload + i, sizeof(payload) - i, "%s", tail);
}

void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  tAck = millis();
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
  Serial.println("=== STEP 11: CoAP UJI KINERJA ===");
  Serial.print("PAYLOAD_SIZE = "); Serial.println(PAYLOAD_SIZE);
  Serial.println("Metode: [R1] cold transaction, 30 iterasi, jeda 4000ms, CoAP CON");

  konekWifi();
  coap.response(callback_response);
  coap.start();
  buildPayload();
  Serial.print("Payload ("); Serial.print(strlen(payload)); Serial.print(" byte): ");
  Serial.println(payload);
  Serial.println("iterasi,latency_ms,status");

  int berhasil = 0;
  unsigned long totalLatency = 0;

  for (int it = 1; it <= N_ITERASI; it++) {
    ackReceived = false;
    unsigned long t0 = millis();
    coap.put(coap_server, 5683, "sensor", payload);

    // tunggu ACK (CON) sampai timeout
    unsigned long tStart = millis();
    while (!ackReceived && (millis() - tStart < ACK_TIMEOUT_MS)) {
      coap.loop();
      delay(1);
    }

    if (ackReceived) {
      unsigned long latency = tAck - t0;
      totalLatency += latency;
      berhasil++;
      Serial.print(it); Serial.print(","); Serial.print(latency); Serial.println(",OK");
    } else {
      Serial.print(it); Serial.println(",,TIMEOUT");
    }

    delay(JEDA_MS); // jeda antar iterasi [R1]
  }

  Serial.println("=== SELESAI 30 ITERASI ===");
  Serial.print("Berhasil: "); Serial.print(berhasil); Serial.print("/"); Serial.println(N_ITERASI);
  Serial.print("Success Rate: "); Serial.print((berhasil * 100.0) / N_ITERASI); Serial.println(" %");
  if (berhasil > 0) {
    Serial.print("Mean latency: "); Serial.print(totalLatency / (float)berhasil); Serial.println(" ms");
  }
  Serial.println("Salin baris CSV di atas ke Lembar 3.A. Ganti PAYLOAD_SIZE untuk kombinasi berikutnya.");
}

void loop() {
  // pengujian jalan sekali di setup(); loop dibiarkan kosong.
}
