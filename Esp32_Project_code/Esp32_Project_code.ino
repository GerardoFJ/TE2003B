#include <EngTrModel.h>       /* Model's header file */
#include <rtwtypes.h>
#include <serial-readline.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Roborregos";
const char* password = "RoBorregos2025";

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
      // Serial.println("Conectado!");
    } else {
      // Serial.print("Fallo, rc=");
      Serial.print(client.state());
      // Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}
void setup()
{
  delay(3000);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  EngTrModel_initialize();
}

void loop()
{ 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  reader.poll();
  pot_fixed = map(pot, 0, 4095, 0,200);
  EngTrModel_U.Throttle = pot_fixed;
  if(pot_fixed <= 0){
    EngTrModel_U.Throttle = 0.0;
  }
  if(button){
    EngTrModel_U.BrakeTorque = 10000.0;
  }
  else{
    EngTrModel_U.BrakeTorque = 0.0;
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
