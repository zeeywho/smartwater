/*
 * STEP 13 — MQTT UJI LATENCY (PAYLOAD JSON SENSOR ASLI)
 * Pelengkap uji latency sintetis. Payload = JSON sensor NYATA sistem.
 * Metode SAMA: cold transaction, 30 iterasi, jeda 4000 ms, QoS0 + echo RTT.
 * Beda HANYA payload. PASTIKAN echo_mqtt.py JALAN. TIDAK perlu air.
 *
 * Output CSV: "iterasi,latency_ms,status"
 */
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid        = "zee";
const char* password    = "satuduatiga";
const char* mqtt_server = "172.20.10.3";

const int   N_ITERASI       = 30;
const unsigned long JEDA_MS = 4000;
const unsigned long ACK_TIMEOUT_MS = 2000;

const int   MQTT_PORT  = 1883;
const char* TOPIC_DATA = "smartwater/data";
const char* TOPIC_ACK  = "smartwater/ack";

WiFiClient espClient;
PubSubClient client(espClient);
volatile bool ackReceived = false;
volatile unsigned long tAck = 0;

const char* PAYLOAD_JSON =
  "{\"flow1\":0.00,\"flow2\":0.00,\"pressure_adc\":461,\"voltage\":0.00,\"current\":0.0}";

void onMessage(char* t, byte* p, unsigned int l) { tAck = millis(); ackReceived = true; }

void konekWifi() {
  WiFi.disconnect(true); delay(1000);
  WiFi.mode(WIFI_STA); WiFi.begin(ssid, password);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print(" OK, IP ESP32: "); Serial.println(WiFi.localIP());
}

bool konekBroker() {
  String cid = "ESP32_LatJson_" + String(random(0xffff), HEX);
  if (client.connect(cid.c_str())) { client.subscribe(TOPIC_ACK); return true; }
  return false;
}

void setup() {
  Serial.begin(115200); delay(500);
  Serial.println("=== STEP 13: MQTT UJI LATENCY (JSON ASLI) ===");
  Serial.print("Payload JSON ("); Serial.print(strlen(PAYLOAD_JSON)); Serial.println(" byte):");
  Serial.println(PAYLOAD_JSON);
  Serial.println("PASTIKAN echo_mqtt.py JALAN. Metode: cold transaction, 30 iterasi, jeda 4000ms.");

  konekWifi();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(onMessage);
  client.setKeepAlive(60);
  client.setSocketTimeout(30);
  client.setBufferSize(1024);

  Serial.println("iterasi,latency_ms,status");
  int berhasil = 0; unsigned long totalLatency = 0;

  for (int it = 1; it <= N_ITERASI; it++) {
    if (client.connected()) client.disconnect();
    if (!konekBroker()) { Serial.print(it); Serial.println(",,CONNECT_FAIL"); delay(JEDA_MS); continue; }
    for (int w = 0; w < 50; w++) { client.loop(); delay(2); }
    ackReceived = false;
    unsigned long t0 = millis();
    client.publish(TOPIC_DATA, PAYLOAD_JSON);
    unsigned long tStart = millis();
    while (!ackReceived && (millis() - tStart < ACK_TIMEOUT_MS)) { client.loop(); }
    if (ackReceived) {
      unsigned long lat = tAck - t0; totalLatency += lat; berhasil++;
      Serial.print(it); Serial.print(","); Serial.print(lat); Serial.println(",OK");
    } else {
      Serial.print(it); Serial.println(",,TIMEOUT");
    }
    client.disconnect();
    delay(JEDA_MS);
  }
  Serial.println("=== SELESAI 30 ITERASI ===");
  Serial.print("Berhasil: "); Serial.print(berhasil); Serial.print("/"); Serial.println(N_ITERASI);
  Serial.print("Success Rate: "); Serial.print((berhasil*100.0)/N_ITERASI); Serial.println(" %");
  if (berhasil > 0) { Serial.print("Mean latency: "); Serial.print(totalLatency/(float)berhasil); Serial.println(" ms"); }
}

void loop() { }
