#include <Arduino.h>
#include <WiFi.h>

// Aumentar tamaño del paquete MQTT si vas a enviar más de 128 bytes
#define MQTT_MAX_PACKET_SIZE 256
#include <PubSubClient.h>

// Configuración WiFi
#define WIFI_SSID     "esp322025"
#define WIFI_PASS     "esp322025"

// Configuración MQTT
#define MQTT_SERVER   "broker.hivemq.com"
#define MQTT_PORT     1883
#define MQTT_USER     ""
#define MQTT_PASS     ""

// Objetos globales
WiFiClient espClient;
PubSubClient mqttClient(espClient);
char buffer[50] = {0};
int counter = 0;

// Temporizadores
unsigned long lastMsg = 0;
const long interval = 1000; // 1 segundo para enviar mensajes
unsigned long lastReconnectAttempt = 0;

// Reconexión no bloqueante a MQTT
void reconnectMQTT() {
  unsigned long now = millis();
  if (now - lastReconnectAttempt > 5000) {
    lastReconnectAttempt = now;
    Serial.print("Intentando conectar a MQTT... ");
    if (mqttClient.connect("ESP32Client2", MQTT_USER, MQTT_PASS)) {
      Serial.println("¡Conectado!");
      mqttClient.setCallback(callback_message);
      mqttClient.subscribe("CMD");
      mqttClient.subscribe("CMD2");
    } else {
      Serial.print("Fallo, rc=");
      Serial.println(mqttClient.state());
    }
  }
  }


void callback_message(char * topic, uint8_t * buffer, unsigned int len){
  Serial.printf("[%s] %s. Bytes:%d\n",topic,buffer,len);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Conectando al WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  lastReconnectAttempt = 0;
    mqttClient.setCallback(callback_message);
    mqttClient.subscribe("CMD");
    mqttClient.subscribe("CMD2");


}

void loop() {
  // Reintento de conexión si no está conectado
  if (!mqttClient.connected()) {
    reconnectMQTT();
    return; // Esperar a conectar antes de continuar
  }

  mqttClient.loop(); // Mantiene viva la conexión MQTT

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;
    sprintf(buffer, "counter:%d", counter++);
    
    bool success = mqttClient.publish("IFTS24", buffer);
    Serial.print(buffer);
    Serial.println(success ? " -> OK" : " -> ERROR");
  }
}