import asyncio
import socket
import json
from aiocoap import resource, Message
import aiocoap

# ===== TAMBAHAN: jembatan CoAP -> MQTT =====
# Setiap data CoAP yang diterima dari ESP32 akan di-PUBLISH ULANG ke broker MQTT,
# sehingga Node-RED (dashboard) bisa menampilkan data CoAP seperti data MQTT.
# Butuh: pip install paho-mqtt
import paho.mqtt.client as mqtt

MQTT_BROKER = "127.0.0.1"        # broker Mosquitto di laptop yang sama (localhost)
MQTT_PORT = 1883
MQTT_TOPIC_COAP = "smartwater/data-coap"   # topik KHUSUS CoAP (beda dari MQTT 'smartwater/data')

# siapkan satu MQTT client global untuk re-publish
mqtt_client = mqtt.Client(client_id="coap_bridge")

def setup_mqtt():
    try:
        mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
        mqtt_client.loop_start()   # jalan di background
        print(f"[BRIDGE] Terhubung ke broker MQTT {MQTT_BROKER}:{MQTT_PORT} "
              f"-> re-publish ke topik '{MQTT_TOPIC_COAP}'")
    except Exception as e:
        print(f"[BRIDGE] PERINGATAN: gagal konek MQTT ({e}). "
              f"Data CoAP tetap diterima, tapi tidak tampil di dashboard.")

def republish_to_mqtt(payload_str):
    """Kirim ulang payload CoAP (string JSON) ke MQTT agar tampil di Node-RED."""
    try:
        # validasi ringan: pastikan JSON valid (tidak mengubah isinya)
        json.loads(payload_str)
        mqtt_client.publish(MQTT_TOPIC_COAP, payload_str)
        print(f"[BRIDGE] -> MQTT '{MQTT_TOPIC_COAP}': {payload_str}")
    except json.JSONDecodeError:
        # kalau bukan JSON, tetap kirim apa adanya (jaga-jaga)
        mqtt_client.publish(MQTT_TOPIC_COAP, payload_str)
        print(f"[BRIDGE] -> MQTT (non-JSON) '{MQTT_TOPIC_COAP}': {payload_str}")
    except Exception as e:
        print(f"[BRIDGE] gagal publish MQTT: {e}")
# ===== /TAMBAHAN =====


def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    except Exception:
        ip = '127.0.0.1'
    finally:
        s.close()
    return ip

class SensorResource(resource.Resource):
    # terima PUT (yang dikirim ESP32)
    async def render_put(self, request):
        payload = request.payload.decode('utf-8')
        print(f"[CoAP] Diterima (PUT): {payload}")
        republish_to_mqtt(payload)          # <-- jembatan: teruskan ke dashboard
        return Message(code=aiocoap.CHANGED, payload=b"OK")

    # terima POST juga (jaga-jaga)
    async def render_post(self, request):
        payload = request.payload.decode('utf-8')
        print(f"[CoAP] Diterima (POST): {payload}")
        republish_to_mqtt(payload)          # <-- jembatan: teruskan ke dashboard
        return Message(code=aiocoap.CHANGED, payload=b"OK")

async def main():
    setup_mqtt()                            # <-- siapkan koneksi MQTT dulu
    local_ip = get_local_ip()
    root = resource.Site()
    root.add_resource(['sensor'], SensorResource())
    print("=== CoAP Server (+ jembatan MQTT) berjalan ===")
    print(f"IP server (laptop): {local_ip}  |  Port: 5683")
    print("Menunggu data di resource /sensor (PUT/POST) ...")
    print(f"Data CoAP akan diteruskan ke MQTT topik '{MQTT_TOPIC_COAP}' untuk dashboard.")
    await aiocoap.Context.create_server_context(root, bind=(local_ip, 5683))
    await asyncio.get_running_loop().create_future()

if __name__ == "__main__":
    asyncio.run(main())
