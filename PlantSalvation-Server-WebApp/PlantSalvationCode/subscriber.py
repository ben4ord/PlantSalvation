import psycopg2
from awscrt import io, mqtt, auth, http
from awsiot import mqtt_connection_builder
import time as t
import json
import time



# Define ENDPOINT, CLIENT_ID, PATH_TO_CERTIFICATE, PATH_TO_PRIVATE_KEY, PATH_TO_AMAZON_ROOT_CA_1, MESSAGE, TOPIC, and RANGE
ENDPOINT = "YOUR ENDPOINT"
CLIENT_ID = "listener"
PATH_TO_CERTIFICATE = "certificates/YOUR CERT PATH"
PATH_TO_PRIVATE_KEY = "certificates/YOUR PRIVATE KEY PATH"
PATH_TO_AMAZON_ROOT_CA_1 = "certificates/AmazonRootCA1.pem"
TOPIC = "plantSalvation"
RANGE = 10


# Database connection helper
def insert_reading(sensor_type, value):
    conn = psycopg2.connect(
        database="plantsalvation",
        user="postgres",
        password="YOUR PASSWORD",
        host="localhost",
        port="5432"
    )
    cur = conn.cursor()

    # Lookup sensor ID by type
    cur.execute("SELECT sensor_id FROM sensor WHERE type = %s", (sensor_type,))
    result = cur.fetchone()

    if result is None:
        print(f"Unknown sensor type: {sensor_type}")
        cur.close()
        conn.close()
        return

    sensor_id = result[0]
    print(f"inserting Sensor ID: {sensor_id} \nValue:{value}")

    # Insert reading
    cur.execute(
        "INSERT INTO sensor_reading (sensor_id, reading_time, reading_value) VALUES (%s, NOW(), %s)",
        (sensor_id, value)
    )

    conn.commit()
    cur.close()
    conn.close()



# MQTT message callback
def message_callback_unpack(topic, payload, **kwargs):
    print("MESSAGE RECEIVED")
    try:
        data = json.loads(payload.decode("utf-8"))
        print(data)
    except json.JSONDecodeError as e:
        print("Invalid JSON:", e)
        return

    for sensor_type in ["temp", "humidity", "moisturePercent", "brightnessPercent"]:
        if sensor_type in data:
            insert_reading(sensor_type, data[sensor_type])





# Configure MQTT client Subscribe to topic
def subscribe_to_topic():
    # Spin up resources
    event_loop_group = io.EventLoopGroup(1)
    host_resolver = io.DefaultHostResolver(event_loop_group)
    client_bootstrap = io.ClientBootstrap(event_loop_group, host_resolver)
    mqtt_connection = mqtt_connection_builder.mtls_from_path(
            endpoint=ENDPOINT,
            cert_filepath=PATH_TO_CERTIFICATE,
            pri_key_filepath=PATH_TO_PRIVATE_KEY,
            client_bootstrap=client_bootstrap,
            ca_filepath=PATH_TO_AMAZON_ROOT_CA_1,
            client_id=CLIENT_ID,
            clean_session=False,
            keep_alive_secs=30
            )
    # Connect to AWS IoT Core
    print(f"Connecting to {ENDPOINT}...")
    connect_future = mqtt_connection.connect()
    connect_future.result()
    print("Connected!")

    print(f"Subscribing to topic '{TOPIC}'...")
    subscribe_future, packet_id = mqtt_connection.subscribe(
        topic=TOPIC,
        qos=mqtt.QoS.AT_LEAST_ONCE,
        callback=message_callback_unpack
        )
    subscribe_result = subscribe_future.result()
    print(f"Subscribed with QoS: {subscribe_result['qos']}")


    # Keep the script running to listen for messages
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Disconnecting...")
        mqtt_connection.disconnect()
        print("Disconnected.")

if __name__ == '__main__':
    subscribe_to_topic()