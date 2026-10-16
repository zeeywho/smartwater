/*
 * STEP 9 — CoAP Send Data Sensor
 * ESP32 kirim JSON sensor ke CoAP server (laptop) via POST ke resource /sensor
 * Payload JSON IDENTIK dengan MQTT (untuk perbandingan adil)
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

// ====== ISI SENDIRI (WI-FI RUMAH) ======
const char* ssid     = "Nana";       // <-- nama Wi-Fi rumah
const char* password = "71727579";   // <-- password Wi-Fi rumah
IPAddress coap_server(192, 168, 1, 10);        // IP laptop (CoAP server)
// ========================================

WiFiUDP udp;
Coap coap(udp);
Adafruit_INA219 ina219;

const int FLOW1_PIN = 25, FLOW2_PIN = 27, PRESSURE_PIN = 34;
volatile unsigned long pulse1 = 0, pulse2 = 0;
void IRAM_ATTR cp1() { pulse1++; }
void IRAM_ATTR cp2() { pulse2++; }
unsigned long lastTime = 0;

// callback saat server membalas (response)
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = '\0';
  Serial.print("Response dari server: "); Serial.println(p);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  delay(500);
  Serial.println("=== STEP 9: CoAP SEND ===");

  pinMode(FLOW1_PIN, INPUT_PULLUP);
  pinMode(FLOW2_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW1_PIN), cp1, FALLING);
  attachInterrupt(digitalPinToInterrupt(FLOW2_PIN), cp2, FALLING);
  analogReadResolution(12);
  Wire.begin(21, 22);
  if (!ina219.begin()) Serial.println("PERINGATAN: INA219 tidak terdeteksi.");

  // Wi-Fi
  Serial.print("Connecting WiFi: "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println();
  Serial.print("WiFi OK. IP ESP32: "); Serial.println(WiFi.localIP());

  // CoAP
  coap.response(callback_response);
  coap.start();
  Serial.println("CoAP client siap.");
}

void loop() {
  coap.loop();

  if (millis() - lastTime >= 1000) {
    lastTime = millis();
    unsigned long f1 = pulse1; pulse1 = 0;
    unsigned long f2 = pulse2; pulse2 = 0;
    float flow1 = f1 / 7.5, flow2 = f2 / 7.5;
    int adc = analogRead(PRESSURE_PIN);
    float busV = ina219.getBusVoltage_V();
    float cur = ina219.getCurrent_mA();

    // PENGAMAN: cegah nan/inf masuk ke JSON (keduanya bikin JSON invalid
    // -> server/parser gagal baca). INA219 longgar/tak terbaca sering
    // menghasilkan nan atau inf.
    if (isnan(busV) || isinf(busV)) busV = 0;
    if (isnan(cur)  || isinf(cur))  cur  = 0;

    char payload[200];
    snprintf(payload, sizeof(payload),
      "{\"flow1\":%.2f,\"flow2\":%.2f,\"pressure_adc\":%d,\"voltage\":%.2f,\"current\":%.1f}",
      flow1, flow2, adc, busV, cur);

    // kirim via CoAP POST ke resource "sensor"
    coap.put(coap_server, 5683, "sensor", payload);
    Serial.print("CoAP sent: "); Serial.println(payload);
  }
}
