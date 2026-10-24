/*
 * STEP 11 — MQTT UJI KINERJA (metode [R1] Kirilov & Chikurtev 2026)
 * Mengukur: end-to-end latency (RTT via ECHO), success rate, (throughput dihitung di laptop)
 *
 * Metode:
 *  - Cold transaction: tiap iterasi reconnect broker -> publish 1 pesan -> tunggu ECHO -> disconnect.
 *  - 30 iterasi per kombinasi; jeda 4000 ms antar iterasi.
 *  - Payload 16 / 64 / 256 byte (set PAYLOAD_SIZE).
 *  - MQTT QoS 0.
 *
 * MEKANISME LATENCY (RTT yang adil vs CoAP CON):
 *  - ESP32 publish ke topik  smartwater/data  (sertakan "seq" unik)
 *  - Laptop meng-ECHO balik pesan ke topik  smartwater/ack
 *  - ESP32 subscribe smartwater/ack; saat echo tiba -> catat t1. latency = t1 - t0.
 *  - Setup echo di laptop: Node-RED (mqtt in smartwater/data -> mqtt out smartwater/ack)
 *    ATAU script Python paho (subscribe data, publish ack).
 *
 * Output Serial (CSV): "iterasi,latency_ms,status"
 */
#include <WiFi.h>
#include <PubSubClient.h>

// ====== ISI SENDIRI ======
const char* ssid        = "Nana";
const char* password    = "71727579";
const char* mqtt_server = "192.168.1.10";   // IP laptop (broker) - cek ipconfig
// ==========================

// ====== PARAMETER UJI (metode [R1]) ======
#define PAYLOAD_SIZE 16      // <-- GANTI: 16, 64, atau 256
const int   N_ITERASI       = 30;    // [R1]
const unsigned long JEDA_MS = 4000;  // [R1] inter-trial delay
const unsigned long ACK_TIMEOUT_MS = 2000;
// =========================================

const int   MQTT_PORT   = 1883;
const char* TOPIC_DATA   = "smartwater/data";
const char* TOPIC_ACK    = "smartwater/ack";

WiFiClient espClient;
PubSubClient client(espClient);

volatile bool ackReceived = false;
volatile unsigned long tAck = 0;
int curSeq = 0;

char payload[300];

// Bangun payload JSON ~PAYLOAD_SIZE byte, sertakan seq agar echo bisa dicocokkan
void buildPayload(int seq) {
  char head[48];
  int hlen = snprintf(head, sizeof(head), "{\"seq\":%d,\"pad\":\"", seq);
  const char* tail = "\"}";
  int baseLen = hlen + strlen(tail);
  int padLen = PAYLOAD_SIZE - baseLen;
  if (padLen < 0) padLen = 0;
  if (padLen > (int)sizeof(payload) - baseLen - 1) padLen = sizeof(payload) - baseLen - 1;
  int i = 0;
  i += snprintf(payload + i, sizeof(payload) - i, "%s", head);
  for (int k = 0; k < padLen; k++) payload[i++] = 'x';
  snprintf(payload + i, sizeof(payload) - i, "%s", tail);
}

void onMessage(char* topic, byte* pl, unsigned int len) {
  // echo diterima -> catat waktu (cocokkan seq bila perlu)
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

bool konekBroker() {
  String cid = "ESP32_Uji_" + String(random(0xffff), HEX);
  if (client.connect(cid.c_str())) {
    client.subscribe(TOPIC_ACK);
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== STEP 11: MQTT UJI KINERJA ===");
  Serial.print("PAYLOAD_SIZE = "); Serial.println(PAYLOAD_SIZE);
  Serial.println("Metode: [R1] cold transaction, 30 iterasi, jeda 4000ms, MQTT QoS0 + echo RTT");
  Serial.println("PASTIKAN echo aktif di laptop: smartwater/data -> smartwater/ack");

  konekWifi();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(onMessage);
  client.setKeepAlive(60);
  client.setSocketTimeout(30);

  Serial.println("iterasi,latency_ms,status");

  int berhasil = 0;
  unsigned long totalLatency = 0;

  for (int it = 1; it <= N_ITERASI; it++) {
    // cold transaction: reconnect tiap iterasi
    if (client.connected()) client.disconnect();
    if (!konekBroker()) {
      Serial.print(it); Serial.println(",,CONNECT_FAIL");
      delay(JEDA_MS);
      continue;
    }

    buildPayload(it);
    ackReceived = false;
    unsigned long t0 = millis();
    client.publish(TOPIC_DATA, payload);   // QoS 0

    // tunggu echo (ACK aplikasi) sampai timeout
    unsigned long tStart = millis();
    while (!ackReceived && (millis() - tStart < ACK_TIMEOUT_MS)) {
      client.loop();
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

    client.disconnect();   // tutup koneksi (cold transaction)
    delay(JEDA_MS);
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
  // pengujian jalan sekali di setup().
}
