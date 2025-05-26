#include <WiFi.h>
#include <PubSubClient.h>

// CONFIGURA TU WIFI
const char* ssid = "Roborregos";
const char* password = "RoBorregos2025";

// CONFIGURA EL BROKER MQTT
const char* mqtt_server = "192.168.0.244";  // IP o dominio de tu broker Mosquitto
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop hasta que esté conectado
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect("stm32/connect")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publicar un mensaje cada 5 segundos
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    String msg = "Hola desde ESP32";
    Serial.print("Publicando mensaje: ");
    Serial.println(msg);
    client.publish("esp32/estado", msg.c_str());
  }
}
