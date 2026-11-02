/*
 * STEP 14 (v2) — DETEKSI KEBOCORAN per-TITIK (sesuai karakteristik alat)
 *
 * KONSEP (per-titik / point-based leak detection):
 *   Sistem pipa TERTUTUP (ujung = pressure sensor, bukan outlet). Saat NORMAL
 *   (semua keran cabang TUTUP) tidak ada air mengalir keluar -> flow sensor = 0 &
 *   tekanan terbangun (tinggi). Saat terjadi KEBOCORAN pada salah satu titik cabang,
 *   air mengalir keluar lewat cabang itu -> flow sensor di hulu cabang mendeteksi
 *   aliran, dan tekanan di ujung cenderung turun (indikator PENDUKUNG).
 *
 *   Susunan: Galon -> Kran Utama -> [Flow1] -> Cabang-1(titik bocor 1) -> [Flow2]
 *            -> Cabang-2(titik bocor 2) -> [Pressure] (ujung, tertutup)
 *
 *   LOGIKA:
 *     - Flow1 mengalir (> AMBANG)  -> BOCOR di TITIK 1 (cabang-1)
 *     - Flow2 mengalir (> AMBANG)  -> BOCOR di TITIK 2 (cabang-2)
 *     - keduanya ~0                -> NORMAL
 *     - pressure (ADC) = indikator PENDUKUNG (drop saat ada aliran keluar/bocor)
 *
 * Rujukan: [M9] Ayamga & Nakpih (2024) — deteksi berbasis flow YF-S201 + lokalisasi;
 *          [M10] Sikkandhar (2026) — sensor flow pada titik distribusi;
 *          [M12] Ali (2022) — tekanan sebagai indikator pendukung kebocoran.
 *
 * CATATAN: deteksi UTAMA = flow (menentukan ADA bocor + DI MANA). Pressure = pendukung.
 * Pin: Flow1=GPIO25, Flow2=GPIO27, Pressure=GPIO34(ADC), INA219=I2C(21/22).
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
// AMBANG aliran (L/min). Di ATAS ini dianggap "ada aliran = bocor di titik tsb".
// Dipilih di atas noise sporadis (~0.1-0.3) & di bawah aliran bocor nyata (>1.0).
// Dari data uji: noise 0.13-0.27 ; aliran bocor 1.0-2.5 -> AMBANG 0.5 memisahkan dgn baik.
float AMBANG_FLOW = 0.5;   // L/min
// (opsional) ambang tekanan rendah sbg pendukung; nonaktif-default (0 = tak dipakai sbg penentu)
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
  Serial.println("=== STEP 14 (v2): DETEKSI KEBOCORAN per-TITIK ===");
  Serial.print("AMBANG_FLOW = "); Serial.print(AMBANG_FLOW); Serial.println(" L/min");
  Serial.println("NORMAL=kedua flow 0 ; BOCOR TITIK 1=Flow1 mengalir ; BOCOR TITIK 2=Flow2 mengalir.");

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

  Serial.println("waktu(s),flow1,flow2,pressure_adc,status,titik");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) { WiFi.reconnect(); delay(1000); return; }
  if (!client.connected()) reconnect();
  client.loop();

  if (millis() - lastTime >= 1000) {   // tiap 1 detik
    lastTime = millis();
    unsigned long f1 = pulse1; pulse1 = 0;
    unsigned long f2 = pulse2; pulse2 = 0;
    float flow1 = f1 / 7.5;   // konversi pulsa->L/min (preliminary)
    float flow2 = f2 / 7.5;
    int adc = analogRead(PRESSURE_PIN);
    float busV = ina219.getBusVoltage_V();
    float cur  = ina219.getCurrent_mA();
    if (isnan(busV) || isinf(busV)) busV = 0;
    if (isnan(cur)  || isinf(cur))  cur  = 0;

    // ===== LOGIKA DETEKSI per-TITIK =====
    // BOCOR bila ada aliran terdeteksi di salah satu flow sensor (titik rawan bocor).
    // Flow1 > Flow2 prioritas titik 1; bila hanya Flow2 -> titik 2.
    const char* status;   // untuk dashboard: NORMAL / BOCOR
    const char* titik;    // lokasi: "-" / "TITIK_1" / "TITIK_2"
    if (flow1 > AMBANG_FLOW && flow1 >= flow2) {
      status = "BOCOR"; titik = "TITIK_1";
    } else if (flow2 > AMBANG_FLOW) {
      status = "BOCOR"; titik = "TITIK_2";
    } else {
      status = "NORMAL"; titik = "-";
    }
    // (pressure = indikator PENDUKUNG; tidak dipakai sbg penentu status. Tetap dikirim
    //  agar bisa dianalisis: cenderung turun saat ada aliran keluar/bocor.)

    // kirim JSON ke dashboard (ada status + titik + pressure pendukung)
    char payload[240];
    snprintf(payload, sizeof(payload),
      "{\"flow1\":%.2f,\"flow2\":%.2f,\"pressure_adc\":%d,\"voltage\":%.2f,\"current\":%.1f,\"status\":\"%s\",\"titik\":\"%s\"}",
      flow1, flow2, adc, busV, cur, status, titik);
    client.publish(MQTT_TOPIC, payload);

    // cetak ke Serial utk dicatat
    Serial.print(millis()/1000); Serial.print(",");
    Serial.print(flow1,2); Serial.print(",");
    Serial.print(flow2,2); Serial.print(",");
    Serial.print(adc); Serial.print(",");
    Serial.print(status); Serial.print(",");
    Serial.println(titik);
  }
}
