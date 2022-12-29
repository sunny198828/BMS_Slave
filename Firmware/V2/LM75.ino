#include <Temperature_LM75_Derived.h>
Generic_LM75 temperature_shunt1(0x49);
Generic_LM75 temperature_shunt2(0x4B);
Generic_LM75 temperature_cell(0x4C);
bool sensor_shunt1 = false;
bool sensor_shunt2 = false;

unsigned int long millis_LM75;
void setup_LM75(){
    Wire.begin();     
    temperature_shunt1.setAlertActiveHigh();
    temperature_shunt1.setTemperatureHighC(80.0);
    temperature_shunt1.setTemperatureLowC(50.0);    
    temperature_shunt2.setAlertActiveHigh();
    temperature_shunt2.setTemperatureHighC(80.0);
    temperature_shunt2.setTemperatureLowC(50.0);
  pinMode(LM75_INT,INPUT_PULLUP);
}

void measure_tempertures(){
  if(millis()-millis_LM75 >= 1000){
    millis_LM75 = millis();
      TEMP_SHUNT1 = temperature_shunt1.readTemperatureC();
      TEMP_SHUNT2 = temperature_shunt2.readTemperatureC();

  }
  if(digitalRead(LM75_INT)){
    bal_thermal_shutdown = true;
  }else{
    bal_thermal_shutdown = false;
  }
}
