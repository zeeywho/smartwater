"""
Echo MQTT untuk pengujian latency (RTT) — STEP 11.
Dijalankan di LAPTOP (gateway). Tugasnya: tiap pesan yang masuk di topik
'smartwater/data' langsung di-publish balik ke 'smartwater/ack', sehingga
ESP32 bisa mengukur round-trip time (RTT) secara adil dibanding CoAP CON.

Butuh: pip install paho-mqtt
Jalankan: python echo_mqtt.py
Broker: Mosquitto lokal (localhost:1883) harus sudah jalan.
"""
import paho.mqtt.client as mqtt

BROKER = "localhost"
PORT = 1883
TOPIC_DATA = "smartwater/data"
TOPIC_ACK = "smartwater/ack"


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"Echo terhubung ke broker {BROKER}:{PORT}")
        client.subscribe(TOPIC_DATA)
        print(f"Subscribe '{TOPIC_DATA}' -> echo ke '{TOPIC_ACK}'")
    else:
        print(f"Gagal connect, rc={rc}")


def on_message(client, userdata, msg):
    # echo balik secepat mungkin (payload sama)
    client.publish(TOPIC_ACK, msg.payload)
    print(f"echo: {msg.payload[:60]!r} ...")


def main():
    # Kompatibel paho-mqtt v1 & v2
    try:
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, client_id="echo_laptop")
    except (AttributeError, TypeError):
        client = mqtt.Client(client_id="echo_laptop")
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT, 60)
    print("Echo MQTT berjalan. Tekan Ctrl+C untuk berhenti.")
    client.loop_forever()


if __name__ == "__main__":
    main()
