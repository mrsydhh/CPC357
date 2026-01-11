import paho.mqtt.client as mqtt
from pymongo import MongoClient
from datetime import datetime

# ===== MongoDB =====
MONGO_URI = "mongodb://localhost:27017/"
DB_NAME = "streetlight_db"
COLLECTION_NAME = "readings"

mongo_client = MongoClient(MONGO_URI)
db = mongo_client[DB_NAME]
collection = db[COLLECTION_NAME]

print(" Connected to MongoDB")

# ===== MQTT =====
MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883

TOPICS = [
    "streetlight/rain",
    "streetlight/ldr_ambient",   # Ambient light
    "streetlight/ldr_led",       # LDR under LED
    "streetlight/led_fault"
]

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(" Connected to MQTT Broker")
        for topic in TOPICS:
            client.subscribe(topic)
            print(f" Subscribed to {topic}")
    else:
        print(f" MQTT connection failed, rc={rc}")

def on_message(client, userdata, msg):
    payload = msg.payload.decode()

    try:
        value = int(payload)
    except ValueError:
        value = payload

    document = {
        "topic": msg.topic,
        "value": value,
        "timestamp": datetime.now()
    }

    collection.insert_one(document)
    print(" Stored:", document)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_forever()