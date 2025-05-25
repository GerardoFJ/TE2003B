#include <EngTrModel.h>       /* Model's header file */
#include <rtwtypes.h>
#include <serial-readline.h>
#include <ArduinoJson.h>

void received(char*);
SerialLineReader reader(Serial, received);
JsonDocument doc;
int pot = 0;
int pot_fixed = 0;
int button = 0;

void received(char *line) {
	//Serial.println(line);
  deserializeJson(doc, line);
  pot = doc["adc"];
  button = doc["button"];
}

void setup()
{
  Serial.begin(9600);
  EngTrModel_initialize();
}

void loop()
{ 
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
  Serial.print(EngTrModel_Y.VehicleSpeed);
  Serial.print("V");
  // Serial.print(EngTrModel_Y.EngineSpeed);
  // Serial.print("S");
  // Serial.print(EngTrModel_Y.Gear);
  // Serial.print("G");
  //delay(100);
}
