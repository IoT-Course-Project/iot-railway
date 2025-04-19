#include <WiFi.h>
#include <PubSubClient.h>
#include "Model.h"

const int buzzerPin = 26;
const int greenLEDPin = 25;
const int redLEDPin = 33;

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "mqtt3.thingspeak.com";
const char* mqttUser = "YOUR_MQTT_USERNAME";
const char* mqttPass = "YOUR_MQTT_API_KEY";
const char* clientID = "YourClientID123";
const char* topic = "channels/YOUR_CHANNEL_ID/publish/YOUR_WRITE_API_KEY";

WiFiClient espClient;
PubSubClient client(espClient);

const int trigL = 13, echoL = 12;
const int trigR = 14, echoR = 27;
const int irL = 34, irR = 35;
const int tempPin = 32;

float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 20000);
  float dist = duration * 0.034 / 2;
  return (dist > 0 && dist < 400) ? dist : 0.0;
}

float analogToMilliVolts(int pin) {
  int val = analogRead(pin);
  return (val * 3300.0) / 4095.0;
}

float readTempC(int pin) {
  return analogToMilliVolts(pin) / 10.0;
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void reconnectMQTT() {
  while (!client.connected()) {
    client.connect(clientID, mqttUser, mqttPass);
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(trigL, OUTPUT); pinMode(echoL, INPUT);
  pinMode(trigR, OUTPUT); pinMode(echoR, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  setupWiFi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  float uL = getDistance(trigL, echoL);
  float uR = getDistance(trigR, echoR);
  float irL_mv = analogToMilliVolts(irL);
  float irR_mv = analogToMilliVolts(irR);
  float temp = readTempC(tempPin);

  float features[] = { uL, uR, irL_mv, irR_mv, temp };
  String status = predict(features);
  int label = (status == "misaligned") ? 0 : 1;

  if (label == 0) {  // Misaligned
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, HIGH);
    digitalWrite(buzzerPin, LOW);
  }


  Serial.printf("UL: %.1f | UR: %.1f | IRL: %.1f | IRR: %.1f | T: %.1f => %s\n",
                uL, uR, irL_mv, irR_mv, temp, status.c_str());

  String payload = "field1=" + String(uL) +
                   "&field2=" + String(uR) +
                   "&field3=" + String(irL_mv) +
                   "&field4=" + String(irR_mv) +
                   "&field5=" + String(temp) +
                   "&field6=" + String(label);
  client.publish(topic, payload.c_str());

  delay(2000);
}
