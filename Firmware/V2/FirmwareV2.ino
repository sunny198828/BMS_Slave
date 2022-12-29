#include "define.h"
#include "ATtiny_TimerInterrupt.h"
void TimerHandler1(){
    balance();
    }
void setup() {
  pinMode(PWM1,OUTPUT);
  digitalWrite(PWM1,LOW);
  pinMode(PWM2,OUTPUT);
  digitalWrite(PWM2,LOW);
  //Serielle schnitstellen einstellen
  BMS_communication.begin(9600);
  BMS_debug.begin(19200);
  BMS_communication.setTimeout(500);
  pinMode(LED_BAL, OUTPUT);
  pinMode(EXT_INPUT,INPUT);
  digitalWrite(LED_BAL, LOW);
  read_eeprom();
  read_id_ee();
  analogReference(INTERNAL1V024);
  delay(20);
  calc_offset_adc();
  delay(100);
  setup_LM75();
  setup_PWM();
  turnOFF_PWM();
  if(bal_delay >1 && bal_delay < 2000){
    CurrentTimer.init();
    TimerSetup_OK = CurrentTimer.attachInterruptInterval(bal_delay, TimerHandler1);
    }
  }

void loop() {
  measure();
  measure_tempertures();
  //balance();
// Diese Routine überprüft ob "ALERT_IN" aktiv ist (low) wenn ja wird so lange die Serielle schnitstelle abgeschaltet
// Da gerade ein Firmwareupdate auf einem anderen Slave läuft.
  if(!digitalRead(ALERT_IN) && !Serial_OFF){
    Serial_OFF = true;
    BMS_communication.flush();
    BMS_communication.end();
    }
  if(digitalRead(ALERT_IN) && Serial_OFF){
    BMS_communication.begin(9600);

    Serial_OFF = false;
    }
  if(!Serial_OFF){
    if(BMS_communication_data_available){
      BMS_communication_data_available = false;
      BMS_communication_data_available_handler();
    }
    Serial_poll();
  }
// Ende Routine Serial ON/OFF
}