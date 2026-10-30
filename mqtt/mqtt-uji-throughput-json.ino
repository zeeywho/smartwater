/*
 * STEP 13 — MQTT UJI THROUGHPUT (PAYLOAD JSON SENSOR ASLI)
 * Pelengkap uji throughput sintetis. Payload = JSON sensor NYATA sistem.
 * Metode SAMA: 100 pesan beruntun (1 koneksi), hitung bps & msg/s, QoS0 + echo.
 * Beda HANYA payload. PASTIKAN echo_mqtt.py JALAN. Ulang 3x. TIDAK perlu air.
 */
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid        = "zee";
const char* password    = "satuduatiga";
const char* mqtt_server = "172.20.10.3";

const int N_PESAN = 100;
const unsigned long WAIT_ACK_MS = 3000;

const int   MQTT_PORT  = 1883;
const char* TOPIC_DATA = "smartwater/data";
const char* TOPIC_ACK  = "smartwater/ack";

WiFiClient espClient;
PubSubClient client(espClient);
volatile int ackCount = 0;

const char* PAYLOAD_JSON =
  "{\"flow1\":0.00,\"flow2\":0.00,\"pressure_adc\":461,\"voltage\":0.00,\"current\":0.0}";

void onMessage(char* t, byte* p, unsigned int l) { ackCount++; }

void konekWifi() {
  WiFi.disconnect(true); delay(1000);
  WiFi.mode(WIFI_STA); WiFi.begin(ssid, password);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print(" OK, IP ESP32: "); Serial.println(WiFi.localIP());
}

bool konekBroker() {
  String cid = "ESP32_ThrJson_" + String(random(0xffff), HEX);
  if (client.connect(cid.c_str())) { client.subscribe(TOPIC_ACK); return true; }
  return false;
}

void setup() {
  Serial.begin(115200); delay(500);
  Serial.println("=== STEP 13: MQTT UJI THROUGHPUT (JSON ASLI) ===");
  int plen = strlen(PAYLOAD_JSON);
  Serial.print("Payload JSON ("); Serial.print(plen); Serial.println(" byte)");
  Serial.print("N_PESAN = "); Serial.println(N_PESAN);
  Serial.println("PASTIKAN echo_mqtt.py JALAN.");

  konekWifi();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(onMessage);
  client.setKeepAlive(60);
  client.setSocketTimeout(30);
  client.setBufferSize(1024);

  if (!konekBroker()) { Serial.println("GAGAL connect broker. Cek IP/Mosquitto."); return; }
  for (int w = 0; w < 100; w++) { client.loop(); delay(2); }

  ackCount = 0;
  unsigned long tMulai = millis();
  for (int it = 1; it <= N_PESAN; it++) {
    client.publish(TOPIC_DATA, PAYLOAD_JSON);
    for (int k = 0; k < 3; k++) { client.loop(); delay(1); }
  }
  unsigned long tWait = millis();
  while (millis() - tWait < WAIT_ACK_MS) { client.loop(); }

  unsigned long tSelesai = millis();
  float totalDetik = (tSelesai - tMulai) / 1000.0;
  int sukses = ackCount;
  float msgRate = sukses / totalDetik;
  float bps = (sukses * (float)plen * 8.0) / totalDetik;

  Serial.println("=== HASIL THROUGHPUT ===");
  Serial.print("Pesan dikirim: "); Serial.println(N_PESAN);
  Serial.print("Echo diterima (sukses): "); Serial.println(sukses);
  Serial.print("Total waktu (s): "); Serial.println(totalDetik, 3);
  Serial.print("Throughput (msg/s): "); Serial.println(msgRate, 2);
  Serial.print("Throughput (bps): "); Serial.println(bps, 2);
  Serial.print("Success rate (%): "); Serial.println((sukses*100.0)/N_PESAN, 2);
  client.disconnect();
}

void loop() { }
