/*
 * STEP 13 — CoAP UJI LATENCY (PAYLOAD JSON SENSOR ASLI)
 * Pelengkap uji latency sintetis: di sini payload = format JSON sensor NYATA sistem.
 * Tujuan: kondisi operasional realistis + validasi korelasi dgn tren payload sintetis.
 *
 * Metode SAMA dgn uji latency sintetis (agar sebanding):
 *  - Cold transaction, 30 iterasi, jeda 4000 ms, CoAP CON, latency = RTT (ms).
 * Beda HANYA di payload: JSON sensor asli (bukan "xxxx").
 *
 * Payload JSON asli (format persis sistem, nilai contoh tetap agar ukuran konsisten):
 *   {"flow1":0.00,"flow2":0.00,"pressure_adc":461,"voltage":0.00,"current":0.0}
 * Ukuran ~75 byte (dicetak di Serial). TIDAK perlu air (nilai statis utk uji jaringan).
 *
 * Output CSV: "iterasi,latency_ms,status"
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// ====== ISI SENDIRI ======
const char* ssid     = "zee";
const char* password = "satuduatiga";
IPAddress coap_server(172, 20, 10, 3);
// ==========================

const int   N_ITERASI      = 30;
const unsigned long JEDA_MS = 4000;
const unsigned long ACK_TIMEOUT_MS = 2000;

WiFiUDP udp;
Coap coap(udp);
volatile bool ackReceived = false;
volatile unsigned long tAck = 0;

// Payload JSON sensor ASLI (format persis sistem). Nilai statis -> ukuran konsisten.
const char* PAYLOAD_JSON =
  "{\"flow1\":0.00,\"flow2\":0.00,\"pressure_adc\":461,\"voltage\":0.00,\"current\":0.0}";

void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  tAck = millis(); ackReceived = true;
}

void konekWifi() {
  WiFi.disconnect(true); delay(1000);
  WiFi.mode(WIFI_STA); WiFi.begin(ssid, password);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print(" OK, IP ESP32: "); Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200); delay(500);
  Serial.println("=== STEP 13: CoAP UJI LATENCY (JSON ASLI) ===");
  Serial.print("Payload JSON ("); Serial.print(strlen(PAYLOAD_JSON)); Serial.println(" byte):");
  Serial.println(PAYLOAD_JSON);
  Serial.println("Metode: cold transaction, 30 iterasi, jeda 4000ms, CoAP CON.");

  konekWifi();
  coap.response(callback_response);
  coap.start();
  Serial.println("iterasi,latency_ms,status");

  int berhasil = 0; unsigned long totalLatency = 0;
  for (int it = 1; it <= N_ITERASI; it++) {
    ackReceived = false;
    unsigned long t0 = millis();
    coap.put(coap_server, 5683, "sensor", (char*)PAYLOAD_JSON);
    unsigned long tStart = millis();
    while (!ackReceived && (millis() - tStart < ACK_TIMEOUT_MS)) { coap.loop(); }
    if (ackReceived) {
      unsigned long lat = tAck - t0; totalLatency += lat; berhasil++;
      Serial.print(it); Serial.print(","); Serial.print(lat); Serial.println(",OK");
    } else {
      Serial.print(it); Serial.println(",,TIMEOUT");
    }
    delay(JEDA_MS);
  }
  Serial.println("=== SELESAI 30 ITERASI ===");
  Serial.print("Berhasil: "); Serial.print(berhasil); Serial.print("/"); Serial.println(N_ITERASI);
  Serial.print("Success Rate: "); Serial.print((berhasil*100.0)/N_ITERASI); Serial.println(" %");
  if (berhasil > 0) { Serial.print("Mean latency: "); Serial.print(totalLatency/(float)berhasil); Serial.println(" ms"); }
}

void loop() { }
