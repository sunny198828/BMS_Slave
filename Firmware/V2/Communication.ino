#include <ArduinoJson.h>

  StaticJsonDocument<250> BMS_data;
void sendBMS_data(){
    byte count = 0;
    BMS_data.clear();
    BMS_data["ID"] = SLAVEID;
    BMS_data["DATA"][count] = vdd_voltagedivider;             count++;
    BMS_data["DATA"][count] = BAL_CURRENT_calced;             count++;
    BMS_data["DATA"][count] = current_bal_shunt;              count++;
    BMS_data["DATA"][count] = vdd_chip;                       count++;
    BMS_data["DATA"][count] = temp_chip;                      count++;
    BMS_data["DATA"][count] = int((100.00/pwm_periode)*pwm1); count++;
    BMS_data["DATA"][count] = int((100.00/pwm_periode)*pwm2); count++;
    BMS_data["DATA"][count] = TEMP_SHUNT1;                    count++;
    BMS_data["DATA"][count] = TEMP_SHUNT2;                    count++;
    BMS_data["DATA"][count] = balance_activ;                  count++;
    BMS_data["DATA"][count] = bal_thermal_shutdown;           count ++;
    if(ext_shunt){
      BMS_data["DATA"][count] = current_ext_shunt; count++;
      BMS_data["DATA"][count] = ADC_extShunt_voltage; count++;
    }
//    BMS_data["CURRENT_BAL_CALCED"] = BAL_CURRENT_calced;
//    BMS_data["VOLT"] = vdd_voltagedivider;
//    BMS_data["VDD_CHIP"] = vdd_chip;
//    BMS_data["TEMP_CHIP"] = temp_chip;
//    BMS_data["PWM1%"] = int((100.00/pwm_periode)*pwm1);
//    BMS_data["PWM2%"] = int((100.00/pwm_periode)*pwm2);
//    BMS_data["RES1_TEMP"] = TEMP_SHUNT1;
//    BMS_data["RES2_TEMP"] = TEMP_SHUNT2;
//    BMS_data["BAL_BEGIN"] = bal_begin;
//    BMS_data["BAL_STOP"] = bal_stop;
//    BMS_data["INITIAL_start"] = inital_voltage_start;
//    BMS_data["INITIAL_stop"] = inital_voltage_stop;
    serializeJson(BMS_data,BMS_communication);
  }

void send_balancer_Setup(){
  char eeprom_error_char[100] = {"no Error"};
  if(eeprom_error == 1){
     strcpy(eeprom_error_char,"error Balancing data invalid");
  }
  if(eeprom_error == 3){
    strcpy(eeprom_error_char,"Balancing data invalid | Inital data invalid");
  }
  if(eeprom_error == 2){
    strcpy(eeprom_error_char,"Inital data invalid");
  }
    BMS_data.clear();
    BMS_data["COUNT_SHUNT"] = NUM_resistors;
    BMS_data["MAX_CURRENT"] = MAX_current;
    BMS_data["BAL_VOLT"][0] = bal_begin;
    BMS_data["BAL_VOLT"][1] = bal_stop;
    BMS_data["BAL_VOLT_INIT"][0] = inital_voltage_start;
    BMS_data["BAL_VOLT_INIT"][1] = inital_voltage_stop;
    BMS_data["CURRENT_BAL_MEASURED"] = current_bal_shunt;
    BMS_data["BAL_DELAY"] = bal_delay;
    BMS_data["BAL_DELAY_TIMER"] = TimerSetup_OK;
    BMS_data["INITAL_LOAD"] = inital_loading;
    BMS_data["EXT_SHUNT"] = ext_shunt;
    if(ext_shunt){
    BMS_data["EXT_SHUNT_RES"] = ext_shunt_res;
    BMS_data["EXT_SHUNT_ADC_OFFSET"] = ADC_extShunt_raw_offset;
    }
    BMS_data["Error EEPROM"] = eeprom_error_char;
    serializeJson(BMS_data,BMS_communication);
}

void Serial_poll(){
  if (BMS_communication.available() > 0) {
    // read the incoming string:
    String data = BMS_communication.readString();
    data.trim();
    data.toCharArray(BMS_communication_data, data.length()+1);
    BMS_communication_data_available = true;
  }
}
void BMS_communication_data_available_handler(){
     BMS_data.clear();
     DeserializationError error = deserializeJson(BMS_data, BMS_communication_data);
    if (error) {
      BMS_data["ID"] = SLAVEID;
      BMS_data["ERROR"] = "deserializeJson() failed:";
      BMS_data["DATA"] = error.f_str();
      serializeJson(BMS_data,BMS_communication);
      //BMS_communication.println(BMS_communication_data);
      //BMS_communication.print(F("deserializeJson() failed: "));
      //BMS_communication.println(error.f_str());
      return;
      }
      BMS_debug.println("JSON Daten da und Gültig");
    byte id = BMS_data["ID"];
    if(id == 0 && !SLAVEID_set){
      SLAVEID = BMS_data["NEW_ID"];
      if(SLAVEID > 0 && SLAVEID < 254){
        write_id_ee(SLAVEID);
        read_id_ee();
        sendBMS_data();
      }
    }
    if(id == SLAVEID && SLAVEID_set){
      const char* command = BMS_data["COMMAND"];
      if(strcmp(command,"status") == 0){
        sendBMS_data();
      }
    if(strcmp(command,"dataSet") == 0){
      send_balancer_Setup();
    }
    if(strcmp(command,"clearEE") == 0){
      clear_EEPROM();
    }
    if(strcmp(command,"zero_extShuntADC") == 0){
      if(ext_shunt){
        for(int i = 0;i<100;i++){
          digitalWrite(LED_BAL,HIGH);
          delay(1);
          measure();
          digitalWrite(LED_BAL,LOW);
          }
          digitalWrite(LED_BAL,LOW);
        ADC_extShunt_raw_offset = ADC_extShunt_raw;
        BMS_data.clear();
        BMS_data["ID"] = SLAVEID;
        BMS_data["extShuntADC_offset"] = ADC_extShunt_raw_offset;
        serializeJson(BMS_data,BMS_communication);
      }else{
        BMS_data.clear();
        BMS_data["ID"] = SLAVEID;
        BMS_data["extShuntADC_offset"] = "Error no ext shunt set";
        serializeJson(BMS_data,BMS_communication);        
      }
    }
    if(strcmp(command,"calc_extSHUNT") == 0){
      bool current = BMS_data.containsKey("CURRENT");
      if(current){
        for(int i = 0;i<100;i++){
          digitalWrite(LED_BAL,HIGH);
          measure();
          delay(1);
          digitalWrite(LED_BAL,LOW);
          }
        digitalWrite(LED_BAL,LOW);
        long meterCURRENT = BMS_data["CURRENT"];
        ext_shunt_res = ADC_extShunt_voltage/meterCURRENT;
        BMS_data.clear();
        BMS_data["ID"] = SLAVEID;
        BMS_data["ext_SHUNT"] = ext_shunt_res;
        serializeJson(BMS_data,BMS_communication);
      }else{
        BMS_data.clear();
        BMS_data["ID"] = SLAVEID;
        BMS_data["ext_SHUNT"] = "ERROR: no CURRENT set";
        serializeJson(BMS_data,BMS_communication);
      }
    }
      if(strcmp(command, "bootloader") == 0){
        BMS_data.clear();
        BMS_data["ID"] = SLAVEID;
        BMS_data["DATEN"] = "Warte auf startzeichen und resete dann den controller";
        serializeJson(BMS_data,BMS_communication);
        BMS_communication.setTimeout(0);
        //BMS_communication.begin(9600);
        uint16_t counter = 0;
        while(counter < 8128){
          digitalWrite(LED_BAL,HIGH);
          if(!digitalRead(ALERT_IN)){
            _PROTECTED_WRITE(RSTCTRL.SWRR,1);
            }
          if(BMS_communication.available() > 0){
            String data = BMS_communication.readString();
            if(data == "u"){
                      _PROTECTED_WRITE(RSTCTRL.SWRR,1);
              }
            }
          delay(10);
          counter++;
          digitalWrite(LED_BAL,LOW);
        }
        BMS_data.clear();
        BMS_data["ID"] = SLAVEID;
        BMS_data["DATEN"] = "bootloader abgebrochen";
        serializeJson(BMS_data,BMS_communication);        
        BMS_communication.setTimeout(500);
        digitalWrite(LED_BAL,LOW);
        return;
      }
      if(strcmp(command,"initial_loading") == 0){
        byte inital_loading_command = BMS_data["data"];
        if(inital_loading_command == 1){
          inital_loading = true;
        }else{
          inital_loading = false;
        }
        return;
      }

      if(strcmp(command,"resetDevice") == 0){
        _PROTECTED_WRITE(RSTCTRL.SWRR,1);
      }
      if(strcmp(command,"measure_res") == 0){
        for(int i = 0;i<100;i++){
          digitalWrite(LED_BAL,HIGH);
          measure();
          delay(1);
          digitalWrite(LED_BAL,LOW);
          }
          digitalWrite(LED_BAL,LOW);
        double vdd_noLoad = vdd_voltagedivider;
        setup_PWM();
        set_pwm1(pwm_periode);
        delay(1500);
        for(int i = 0;i<100;i++){
          measure();
          }
          BAL_R = (vdd_voltagedivider / current_bal_shunt)-SHUNT_BAL_RES;
          double internal_resistance = ((vdd_noLoad - vdd_voltagedivider)/current_bal_shunt);
          BMS_data.clear();
          BMS_data["ID"] = SLAVEID;
          BMS_data["VCC"] = vdd_voltagedivider;
          BMS_data["AMP"] = current_bal_shunt;
          BMS_data["RES"] = BAL_R;
          BMS_data["CELL RES"] = internal_resistance;
          serializeJson(BMS_data,BMS_communication);  
          //BMS_communication.print("VOLT: ");BMS_communication.println(vdd_voltagedivider);
          //BMS_communication.print("AMP : ");BMS_communication.println(current_bal_shunt);
          //BMS_communication.print("RES : ");BMS_communication.println(BAL_R);
          //BMS_communication.print("CELL Res: ");BMS_communication.println(internal_resistance,10);
          set_pwm1(0);
          turnOFF_PWM();
      }
      if(strcmp(command,"voltage_divider") == 0){
        for(int i = 0;i<100;i++){
          digitalWrite(LED_BAL,HIGH);
          measure();
          delay(1);
          digitalWrite(LED_BAL,LOW);
          }
        digitalWrite(LED_BAL,LOW);
        double    voltage_meter = BMS_data["meter"];
        vdd_divider_ratio = voltage_meter/((1024.00 / 0xFFFF) * vdd_voltagedivider_raw);
        //BMS_communication.print("ADC: ");BMS_communication.println();
        //BMS_communication.print("Meter: ");BMS_communication.println();
        //BMS_communication.print("divider: ");BMS_communication.println(vdd_divider_ratio,8);
        vdd_divider_ratio_set = true;
        BMS_data.clear();
        BMS_data["ID"] = SLAVEID;
        BMS_data["ADC"] = ((1024.00 / 0xFFFF) * vdd_voltagedivider_raw);
        BMS_data["METER"] = voltage_meter;
        BMS_data["DIV_RAT"] = vdd_divider_ratio;
        serializeJson(BMS_data,BMS_communication); 
        //write_eeprom();
        }
      if(strcmp(command,"saveEE") == 0){
        write_eeprom();
        read_eeprom();
        send_balancer_Setup();
      }      
      if(strcmp(command,"setData") == 0){
        byte ee_valid_calc = 0;
        bool balVoltage = BMS_data.containsKey("BAL_VOLTAGE");
        bool balhighVoltage = BMS_data.containsKey("BAL_HIGH_VOLTAGE");
        bool BAL_DELAY = BMS_data.containsKey("BAL_DELAY");
        bool BAL_CURR  = BMS_data.containsKey("BAL_MAX_CURR");
        bool num_resisitor = BMS_data.containsKey("NUM_RESISTORS");
        bool ext_shunt_on = BMS_data.containsKey("EXT_SHUNT_ON");
        bool ext_shunt_res_set = BMS_data.containsKey("EXT_SHUNT_RES");
        if(ext_shunt_res_set){
          ext_shunt_res = BMS_data["EXT_SHUNT_RES"];
        }
        if(ext_shunt_on){
          ext_shunt = BMS_data["EXT_SHUNT_ON"];
        }
        if(BAL_DELAY){
          bal_delay = BMS_data["BAL_DELAY"];
          if(bal_delay < 200 || bal_delay > 1000){
            bal_delay = 200;
          }
          CurrentTimer.setInterval(bal_delay, TimerHandler1);
          }
        if(balVoltage){
        bal_begin = BMS_data["BAL_VOLTAGE"][0];
        bal_stop  = BMS_data["BAL_VOLTAGE"][1];
          }
        if(balhighVoltage){
          inital_voltage_start = BMS_data["BAL_HIGH_VOLTAGE"][0];
          inital_voltage_stop = BMS_data["BAL_HIGH_VOLTAGE"][1];
          }
        if(BAL_CURR){
          MAX_current = BMS_data["BAL_MAX_CURR"];
          }
        if(num_resisitor){
          NUM_resistors = BMS_data["NUM_RESISTORS"];
          }
        if(NUM_resistors < 1 || NUM_resistors > 2){
          NUM_resistors = 1;
          }
        if(MAX_current < 50 || MAX_current > 6000){
          MAX_current = 100;
          }
        if(balhighVoltage){
          inital_voltage_valid = true;
          }else{
            inital_voltage_valid = false;
            }
        if(balVoltage){
          balancing_data_valid = true;
        }else{
          balancing_data_valid = false;
        }
        if(balVoltage && bal_begin > 3300 && bal_begin < 3600){
          ee_valid_calc ++;
        }
        if(balVoltage && bal_stop < bal_begin){
          ee_valid_calc ++;
        }
        if(bal_delay > 0 && bal_delay < 2000){
          ee_valid_calc ++;
        }
        if(ee_valid_calc == 3){
          //write_eeprom();
          //read_eeprom();
          check_EE_data();
          sendBMS_data();
        }else{
            BMS_data.clear();
            BMS_data["ID"] = SLAVEID;
            BMS_data["Error"] = "data not valid";
            serializeJson(BMS_data,BMS_communication);
        }
      }
    }
}