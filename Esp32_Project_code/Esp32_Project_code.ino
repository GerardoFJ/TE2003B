#include <EngTrModel.h>       /* Model's header file */
#include <rtwtypes.h>
#include <serial-readline.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Roborregos";
const char* password = "RoBorregos2025";
String controlMode = "dashboard";

// CONFIGURA EL BROKER MQTT
const char* mqtt_server = "192.168.0.244";  // IP o dominio de tu broker Mosquitto
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
void received(char*);
SerialLineReader reader(Serial, received);
JsonDocument doc;
int pot = 0;
int pot_fixed = 0;
int button = 0;


void received(char *line) {

  deserializeJson(doc, line);
  pot = doc["adc"];
  button = doc["button"];
}

void setup_wifi() {
  delay(10);
  // Serial.println();
  // Serial.print("Conectando a ");
  // Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }

  // Serial.println("");
  // Serial.println("WiFi conectado");
  // Serial.println("IP: ");
  // Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop hasta que esté conectado
  while (!client.connected()) {
    // Serial.print("Conectando al broker MQTT...");
    if (client.connect("stm32client")) {
      client.subscribe("tractor/control");
      // Serial.println("Conectado!");
    } else {
      // Serial.print("Fallo, rc=");
      Serial.print(client.state());
      // Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // deserialize incoming JSON
  DynamicJsonDocument doc(128);
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) return;

  String t = String(topic);
  if (t == "tractor/control") {
    const char* mode  = doc["mode"];
    bool pedal        = doc["pedal"];
    bool brake        = doc["brake"];
    controlMode = String(mode);
    // apply control to your model
    if (controlMode == "manual") {
      EngTrModel_U.Throttle    = pedal ? 200.0 : 0.0;
      EngTrModel_U.BrakeTorque = brake ? 10000.0 : 0.0;
    }
  }
}
void setup()
{
  delay(3000);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  EngTrModel_initialize();
}

void loop()
{ 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  reader.poll();
  if (controlMode == "dashboard") {
    // in dashboard mode, use UART pot/button
    pot_fixed = map(pot, 0, 4095, 0, 200);
    EngTrModel_U.Throttle = (pot_fixed > 0) ? pot_fixed : 0.0;
    EngTrModel_U.BrakeTorque = (button ? 10000.0 : 0.0);
  }
  EngTrModel_step( );
  String mqttMsg = String("{\"velocity\":") + EngTrModel_Y.VehicleSpeed +
                       ",\"rpm\":" + EngTrModel_Y.EngineSpeed +
                       ",\"gear\":" + EngTrModel_Y.Gear +"}";

  client.publish("tractor/data", mqttMsg.c_str());
  Serial.print(EngTrModel_Y.VehicleSpeed);
  Serial.print("V");
  Serial.print(EngTrModel_Y.EngineSpeed);
  Serial.print("S");
  Serial.print(EngTrModel_Y.Gear);
  Serial.print("E");
  delay(200);
  
}
