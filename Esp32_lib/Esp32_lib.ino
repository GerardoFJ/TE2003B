#include <EngTrModel.h>       /* Model's header file */
#include <rtwtypes.h>

void setup()
{
  Serial.begin(115200);
  EngTrModel_initialize();
}

void loop()
{
  EngTrModel_U.Throttle = 50.0;
	EngTrModel_U.BrakeTorque = 0.0;
  EngTrModel_step( );
  Serial.print("Vehicle speed = ");
  Serial.print(EngTrModel_Y.VehicleSpeed);
  Serial.print(" Engine Speed = ");
  Serial.print(EngTrModel_Y.EngineSpeed);
  Serial.print(" Gear = ");
  Serial.println(EngTrModel_Y.Gear);
  delay(100);
}
