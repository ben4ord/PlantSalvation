/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2652

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

// Final Project
// Braxton Walk and Ben Ford

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

int bluePin = 2;
int greenPin = 3;

unsigned long startTime;
unsigned long currentTime;
unsigned long delayTime = 60000;
bool sleepCheck = false;

// Calibration values to match specific sensor 
const int dry = 434;
const int wet = 115;

// Calibration values to match specific sensor
const int bright = 10;
const int dark = 400;

float temp;
float altitude;
float pressure;
float humidity;
int moisturePercent;
int brightnessPercent;

enum systemState {
    SLEEP,
    COLLECT_DATA,
    SEND_DATA
};

systemState STATE = COLLECT_DATA;

void setup() {
    Serial.begin(57600);
    while(!Serial);    // time to get serial running

    unsigned status;
    
    // default settings
    status = bme.begin(0x76);  

    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    
    pinMode(A0, INPUT); //Moisture sensor
    pinMode(A1, INPUT); //Light sensor

    pinMode(bluePin, OUTPUT); //blue light
    pinMode(greenPin, OUTPUT); //green light

    startTime = millis();
}


void loop() {
    currentTime = millis();

    switch(STATE) {
        case SLEEP:
            if (!sleepCheck) {
                digitalWrite(bluePin, HIGH);
                sleepCheck = true;
            }
            if (currentTime - startTime >= delayTime) {
                startTime = currentTime;
                digitalWrite(bluePin, LOW);
                sleepCheck = false;
                STATE = COLLECT_DATA;
            }
            break;

        case COLLECT_DATA:
            collectData();
            STATE = SEND_DATA;
            break;
        
        case SEND_DATA:
            convertJsonAndSend();
            digitalWrite(greenPin, HIGH);

            delay(1000);
            digitalWrite(greenPin, LOW);
            STATE = SLEEP;
            break;
    }
}

void collectData() {
    temp = (bme.readTemperature() * 9/5) + 32;
    pressure = bme.readPressure() / 100.0F;
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA) * 3.281;
    humidity = bme.readHumidity();

    int rawMoisture = analogRead(A0);
    moisturePercent = map(rawMoisture, dry, wet, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);
    
    int rawLight = analogRead(A1);
    brightnessPercent = map(rawLight, dark, bright, 0, 100);
    brightnessPercent = constrain(brightnessPercent, 0, 100);
}

void convertJsonAndSend() {
    StaticJsonDocument<128> doc;
    doc["temp"] = temp;
    doc["humidity"] = humidity;
    doc["moisturePercent"] = moisturePercent;
    doc["brightnessPercent"] = brightnessPercent;

    serializeJson(doc, Serial);
}