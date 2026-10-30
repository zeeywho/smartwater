/*
 * STEP 12 — MQTT UJI THROUGHPUT
 * Metode mengacu [M2] Mishra 2026 (definisi throughput bps) + [M-arXiv] Krikorian 2023
 * (pengiriman kontinu). BERBEDA dari uji latency: di sini publish BERUNTUN tanpa jeda,
 * pada satu koneksi aktif (bukan cold transaction per pesan).
 *
 * Yang diukur:
 *  - Throughput (bps)   = (pesan_terkonfirmasi × payload_byte × 8) / total_waktu_detik
 *  - Throughput (msg/s) = pesan_terkonfirmasi / total_waktu_detik
 *
 * Konfirmasi (sukses) = pesan yang kembali via ECHO (laptop echo smartwater/data ->
 * smartwater/ack). Pastikan echo_mqtt.py JALAN.
 *
 * N = 100 pesan per kombinasi. Payload 16/64/256 byte. MQTT QoS 0. Ulang 3x.
 * TIDAK perlu air. ESP32 + Wi-Fi + Mosquitto + echo_mqtt.py di laptop.
 */
#include <WiFi.h>
#include <PubSubClient.h>

// ====== ISI SENDIRI ======
const char* ssid        = "zee";
const char* password    = "satuduatiga";
const char* mqtt_server = "172.20.10.3";   // IP laptop (broker) - cek ipconfig
// ==========================

// ====== PARAMETER UJI ======
#define PAYLOAD_SIZE 16      // <-- GANTI: 16, 64, atau 256
const int   N_PESAN        = 100;
const unsigned long WAIT_ACK_MS = 3000;   // tunggu sisa echo setelah publish selesai
// ============================

const int   MQTT_PORT  = 1883;
const char* TOPIC_DATA = "smartwater/data";
const char* TOPIC_ACK  = "smartwater/ack";

WiFiClient espClient;
PubSubClient client(espClient);

volatile int ackCount = 0;   // jumlah echo diterima
char payload[300];

void buildPayload(int seq) {
  char head[32];
  int hlen = snprintf(head, sizeof(head), "{\"s\":%d,\"d\":\"", seq);
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
  ackCount++;
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
  String cid = "ESP32_Thr_" + String(random(0xffff), HEX);
  if (client.connect(cid.c_str())) {
    client.subscribe(TOPIC_ACK);
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== STEP 12: MQTT UJI THROUGHPUT ===");
  Serial.print("PAYLOAD_SIZE = "); Serial.println(PAYLOAD_SIZE);
  Serial.print("N_PESAN = "); Serial.println(N_PESAN);
  Serial.println("Metode: publish beruntun (tanpa jeda) pd 1 koneksi, hitung bps & msg/s. QoS0 + echo.");
  Serial.println("PASTIKAN echo_mqtt.py JALAN di laptop.");

  konekWifi();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(onMessage);
  client.setKeepAlive(60);
  client.setSocketTimeout(30);
  client.setBufferSize(1024);   // agar payload 256 muat (data + ack)

  if (!konekBroker()) {
    Serial.println("GAGAL connect broker. Cek IP/Mosquitto.");
    return;
  }
  // beri waktu subscribe ack aktif
  for (int w = 0; w < 100; w++) { client.loop(); delay(2); }

  int plen;
  { buildPayload(1); plen = strlen(payload); }
  Serial.print("Payload ("); Serial.print(plen); Serial.println(" byte)");

  ackCount = 0;
  unsigned long tMulai = millis();

  // publish beruntun tanpa jeda
  for (int it = 1; it <= N_PESAN; it++) {
    buildPayload(it);
    client.publish(TOPIC_DATA, payload);   // QoS 0
    client.loop();                         // proses echo yang masuk
  }

  // tunggu sisa echo tiba
  unsigned long tWait = millis();
  while (millis() - tWait < WAIT_ACK_MS) { client.loop(); }

  unsigned long tSelesai = millis();
  float totalDetik = (tSelesai - tMulai) / 1000.0;   // termasuk window tunggu echo
  int sukses = ackCount;
  float msgRate = sukses / totalDetik;
  float bps = (sukses * (float)plen * 8.0) / totalDetik;

  Serial.println("=== HASIL THROUGHPUT ===");
  Serial.print("Pesan dikirim: "); Serial.println(N_PESAN);
  Serial.print("Echo diterima (sukses): "); Serial.println(sukses);
  Serial.print("Total waktu (s): "); Serial.println(totalDetik, 3);
  Serial.print("Throughput (msg/s): "); Serial.println(msgRate, 2);
  Serial.print("Throughput (bps): "); Serial.println(bps, 2);
  Serial.print("Success rate (%): "); Serial.println((sukses * 100.0) / N_PESAN, 2);
  Serial.println("Catat ke lembar. Ulang 3x, lalu ganti PAYLOAD_SIZE.");

  client.disconnect();
}

void loop() { }
