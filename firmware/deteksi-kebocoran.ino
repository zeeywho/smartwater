/*
 * STEP 14 — DETEKSI KEBOCORAN (end-to-end, FUNGSI UTAMA)
 * ESP32 baca sensor NYATA (Flow1, Flow2, Pressure, INA219) → hitung selisih Flow1-Flow2
 * → tentukan status NORMAL/BOCOR → kirim JSON via MQTT ke dashboard + cetak Serial.
 *
 * Prinsip (rujuk [M9] Ayamga YF-S201, [M10] flow inlet vs outlet, [M12] tekanan):
 *  - BOCOR bila selisih (Flow1 - Flow2) > THRESHOLD  (air "hilang" di T-junction)
 *  - (opsional) tekanan turun jadi indikator pendukung
 *
 * CARA PAKAI:
 *  1. Jalankan Mosquitto + Node-RED (dashboard) di laptop.
 *  2. Isi ssid/password/mqtt_server (IP laptop).
 *  3. KALIBRASI DULU: alirkan air kondisi NORMAL, lihat Serial → catat selisih normal
 *     → set THRESHOLD sedikit di atas selisih normal (mis. normal ~0.3 → threshold 0.5).
 *  4. Uji NORMAL (air lewat kedua sensor) & BOCOR (buka keran T-junction).
 *  5. Dashboard menampilkan gauge + status NORMAL/BOCOR.
 *
 * PERLU AIR MENGALIR. Pin: Flow1=GPIO25, Flow2=GPIO27, Pressure=GPIO34, INA219=I2C(21/22).
 */
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

// ====== ISI SENDIRI ======
const char* ssid        = "zee";
const char* password    = "satuduatiga";
const char* mqtt_server = "172.20.10.3";    // IP laptop (broker) - cek ipconfig
// ==========================

// ====== PARAMETER DETEKSI ======
// THRESHOLD selisih flow (L/min) utk dinyatakan BOCOR. KALIBRASI dari data NORMAL!
// Mulai dengan nilai sementara; sesuaikan setelah lihat selisih saat air normal.
float THRESHOLD_SELISIH = 0.5;   // <-- SESUAIKAN setelah kalibrasi
// (opsional) ambang penurunan tekanan — nonaktif dulu sampai pressure dikalibrasi
// ================================

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
  WiFi.disconnect(true); delay(1000);
  WiFi.mode(WIFI_STA); WiFi.begin(ssid, password);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print(" OK, IP ESP32: "); Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    String cid = "ESP32_Leak_" + String(random(0xffff), HEX);
    Serial.print("Connecting MQTT broker...");
    if (client.connect(cid.c_str())) { Serial.println(" terhubung!"); }
    else { Serial.print(" gagal rc="); Serial.print(client.state()); Serial.println(", coba lagi"); delay(2000); }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  delay(500);
  Serial.println("=== STEP 14: DETEKSI KEBOCORAN ===");
  Serial.print("THRESHOLD_SELISIH = "); Serial.print(THRESHOLD_SELISIH); Serial.println(" L/min");
  Serial.println("Kalibrasi: alirkan air NORMAL, catat selisih, sesuaikan threshold.");

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
  client.setBufferSize(512);

  Serial.println("waktu(s),flow1,flow2,selisih,pressure_adc,status");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) { WiFi.reconnect(); delay(1000); return; }
  if (!client.connected()) reconnect();
  client.loop();

  if (millis() - lastTime >= 1000) {   // tiap 1 detik
    lastTime = millis();
    unsigned long f1 = pulse1; pulse1 = 0;
    unsigned long f2 = pulse2; pulse2 = 0;
    float flow1 = f1 / 7.5;   // konversi pulsa->L/min (preliminary, sesuai kalibrasi)
    float flow2 = f2 / 7.5;
    int adc = analogRead(PRESSURE_PIN);
    float busV = ina219.getBusVoltage_V();
    float cur  = ina219.getCurrent_mA();
    if (isnan(busV) || isinf(busV)) busV = 0;
    if (isnan(cur)  || isinf(cur))  cur  = 0;

    float selisih = flow1 - flow2;

    // LOGIKA DETEKSI: BOCOR bila selisih flow > threshold
    const char* status = (selisih > THRESHOLD_SELISIH) ? "BOCOR" : "NORMAL";

    // kirim JSON ke dashboard (tambah field status utk node text)
    char payload[220];
    snprintf(payload, sizeof(payload),
      "{\"flow1\":%.2f,\"flow2\":%.2f,\"selisih\":%.2f,\"pressure_adc\":%d,\"voltage\":%.2f,\"current\":%.1f,\"status\":\"%s\"}",
      flow1, flow2, selisih, adc, busV, cur, status);
    client.publish(MQTT_TOPIC, payload);

    // cetak ke Serial utk dicatat (Lembar 2)
    Serial.print(millis()/1000); Serial.print(",");
    Serial.print(flow1,2); Serial.print(",");
    Serial.print(flow2,2); Serial.print(",");
    Serial.print(selisih,2); Serial.print(",");
    Serial.print(adc); Serial.print(",");
    Serial.println(status);
  }
}
