#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#define RXp2 16
#define TXp2 17

const char* ssid = "YOUR WIFI NAME";
const char* password = "YOUR PASSWORD";
long startTime = 0;
long currentTime = 0;
long timePeriod = 5000;

const char* AWS_ENDPOINT = "YOUR ENDPOINT";

WiFiClientSecure net;
MQTTClient client(256);

// Root CA
const char rootCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----

)EOF";

// Device Cert
const char deviceCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";

// Private Key
const char privateKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----

)EOF";

// Connect to AWS
void connectAWS() {
  net.setCACert(rootCert);
  net.setCertificate(deviceCert);
  net.setPrivateKey(privateKey);

  client.begin(AWS_ENDPOINT, 8883, net);
  client.setKeepAlive(60);

  Serial.print("Connecting to AWS IoT...");
  while (!client.connect("esp32-client")) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected!");

  client.subscribe("test");
}



void messageHandler(String &topic, String &payload) {
  Serial.println("Incoming message:");
  Serial.println(payload);
}

void setup() {
  Serial.begin(57600);
  Serial2.begin(57600, SERIAL_8N1, RXp2, TXp2);
  startTime = millis();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi connected.");

  client.onMessage(messageHandler);

  connectAWS();
}

void loop() {
  client.loop();   // ALWAYS RUN

  currentTime = millis();

  if ((currentTime - startTime) > timePeriod) {
    startTime = currentTime;

    if (Serial2.available()) {
      String jsonStr = Serial2.readStringUntil('\n');
      client.publish("plantSalvation", jsonStr);
    }
  }
}

