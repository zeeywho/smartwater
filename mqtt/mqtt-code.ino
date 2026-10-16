/*
 * STEP 8 — MQTT Publish Data Sensor
 * ESP32 publish JSON sensor ke broker Mosquitto di laptop
 */
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

// ====== ISI SENDIRI ======
const char* ssid        = "Nana";              // nama hotspot
const char* password    = "71727579";     // <-- password hotspot kamu
const char* mqtt_server = "192.168.1.10";      // IP laptop (broker)
// ==========================

const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "smartwater/data";

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_INA219 ina219;

const int FLOW1_PIN = 25, FLOW2_PIN = 27, PRESSURE_PIN = 34;
volatile unsigned long pulse1 = 0, pulse2 = 0;
void IRAM_ATTR cp1() { pulse1++; }
void IRAM_ATTR cp2() { pulse2++; }
unsigned long lastTime = 0;

void setupWifi() {
  Serial.print("Connecting WiFi: "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println();
  Serial.print("WiFi OK. IP ESP32: "); Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT broker...");
    String clientId = "ESP32_SmartWater_" + String(random(0xffff), HEX);
    if (client.connect("ESP32_SmartWater")) {
      Serial.println(" terhubung!");
    } else {
      Serial.print(" gagal, rc="); Serial.print(client.state());
      Serial.println(" coba lagi 2 detik...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  delay(500);
  Serial.println("=== STEP 8: MQTT PUBLISH ===");

  pinMode(FLOW1_PIN, INPUT_PULLUP);
  pinMode(FLOW2_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW1_PIN), cp1, FALLING);
  attachInterrupt(digitalPinToInterrupt(FLOW2_PIN), cp2, FALLING);
  analogReadResolution(12);
  Wire.begin(21, 22);
  if (!ina219.begin()) Serial.println("PERINGATAN: INA219 tidak terdeteksi.");

  setupWifi();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setKeepAlive(60);
  client.setSocketTimeout(30);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(1000);
    return;
  }
  if (!client.connected()) reconnect();
  client.loop();

  if (millis() - lastTime >= 1000) {
    lastTime = millis();
    unsigned long f1 = pulse1; pulse1 = 0;
    unsigned long f2 = pulse2; pulse2 = 0;
    float flow1 = f1 / 7.5, flow2 = f2 / 7.5;
    int adc = analogRead(PRESSURE_PIN);
    float busV = ina219.getBusVoltage_V();
    float cur = ina219.getCurrent_mA();

    // PENGAMAN: cegah nan/inf masuk ke JSON (keduanya bikin JSON invalid
    // -> Node-RED gagal parse -> dashboard diam di 0). INA219 longgar/tak
    // terbaca sering menghasilkan nan atau inf.
    if (isnan(busV) || isinf(busV)) busV = 0;
    if (isnan(cur)  || isinf(cur))  cur  = 0;

    char payload[200];
    snprintf(payload, sizeof(payload),
      "{\"flow1\":%.2f,\"flow2\":%.2f,\"pressure_adc\":%d,\"voltage\":%.2f,\"current\":%.1f}",
      flow1, flow2, adc, busV, cur);

    client.publish(MQTT_TOPIC, payload);
    Serial.print("Published: "); Serial.println(payload);
  }
}
