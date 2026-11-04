/*
 * STEP 13 — CoAP UJI THROUGHPUT (PAYLOAD JSON SENSOR ASLI)
 * Pelengkap uji throughput sintetis. Payload = JSON sensor NYATA sistem.
 * Metode SAMA: 100 pesan beruntun (tanpa jeda), hitung bps & msg/s, CoAP CON.
 * Beda HANYA payload. Ulang 3x. TIDAK perlu air (nilai statis utk uji jaringan).
 *
 * Output: msg/s, bps, success%.
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

const char* ssid     = "zee";
const char* password = "satuduatiga";
IPAddress coap_server(172, 20, 10, 3);

const int N_PESAN = 100;
const unsigned long ACK_TIMEOUT_MS = 2000;

WiFiUDP udp;
Coap coap(udp);
volatile bool ackReceived = false;

const char* PAYLOAD_JSON =
  "{\"flow1\":0.00,\"flow2\":0.00,\"pressure_adc\":461,\"voltage\":0.00,\"current\":0.0}";

void callback_response(CoapPacket &packet, IPAddress ip, int port) { ackReceived = true; }

void konekWifi() {
  WiFi.disconnect(true); delay(1000);
  WiFi.mode(WIFI_STA); WiFi.begin(ssid, password);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print(" OK, IP ESP32: "); Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200); delay(500);
  Serial.println("=== STEP 13: CoAP UJI THROUGHPUT (JSON ASLI) ===");
  int plen = strlen(PAYLOAD_JSON);
  Serial.print("Payload JSON ("); Serial.print(plen); Serial.println(" byte)");
  Serial.print("N_PESAN = "); Serial.println(N_PESAN);

  konekWifi();
  coap.response(callback_response);
  coap.start();

  int sukses = 0;
  unsigned long tMulai = millis();
  for (int it = 1; it <= N_PESAN; it++) {
    ackReceived = false;
    coap.put(coap_server, 5683, "sensor", (char*)PAYLOAD_JSON);
    unsigned long tStart = millis();
    while (!ackReceived && (millis() - tStart < ACK_TIMEOUT_MS)) { coap.loop(); }
    if (ackReceived) sukses++;
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
  Serial.print("Success rate (%): "); Serial.println((sukses*100.0)/N_PESAN, 2);
}

void loop() { }
