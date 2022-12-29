#include <EEPROM.h>

struct MyObject {
  byte accutype_ee;
  byte balancing_data_valid_ee;
  byte inital_voltage_valid_ee;
  double bal_begin_ee;
  double bal_stop_ee;
  double inital_voltage_start_ee;
  double inital_voltage_stop_ee;
  uint16_t bal_delay_ee;
  bool vdd_divider_ratio_set_ee;
  double vdd_divider_ratio_ee;
  double MAX_current_ee;
  byte NUM_resistors_ee;
  bool ext_shunt_ee;
  double ext_shunt_res_ee;
  long ADC_extShunt_raw_offset_ee;
};

void write_eeprom(){
  MyObject eedata = {
    accutype,
    balancing_data_valid,
    inital_voltage_valid,
    bal_begin,
    bal_stop,
    inital_voltage_start,
    inital_voltage_stop,
    bal_delay,
    vdd_divider_ratio_set,
    vdd_divider_ratio,
    MAX_current,
    NUM_resistors,
    ext_shunt,
    ext_shunt_res,
    ADC_extShunt_raw_offset,
  };
  EEPROM.put(1, eedata);
}

void write_id_ee(byte data){
  EEPROM.write(0,data);

}
void read_eeprom(){
  MyObject eedata;
  EEPROM.get(1, eedata);
  accutype              = eedata.accutype_ee;
  balancing_data_valid  = eedata.balancing_data_valid_ee;
  inital_voltage_valid  = eedata.inital_voltage_valid_ee;
  bal_begin             = eedata.bal_begin_ee;
  bal_stop              = eedata.bal_stop_ee;
  inital_voltage_start  = eedata.inital_voltage_start_ee;
  inital_voltage_stop   = eedata.inital_voltage_stop_ee;
  bal_delay             = eedata.bal_delay_ee;
  vdd_divider_ratio_set = eedata.vdd_divider_ratio_set_ee;
  MAX_current           = eedata.MAX_current_ee;
  NUM_resistors         = eedata.NUM_resistors_ee;
  ext_shunt             = eedata.ext_shunt_ee;
  ext_shunt_res         = eedata.ext_shunt_res_ee;
  ADC_extShunt_raw_offset = eedata.ADC_extShunt_raw_offset_ee;
  if(bal_delay<150 || bal_delay > 2500 || isnan(bal_delay)){
    bal_delay = 150;
  }
  if(!ext_shunt && isnan(ADC_extShunt_raw_offset)){
    ADC_extShunt_raw_offset = 0;
  }
  if(vdd_divider_ratio_set){
    vdd_divider_ratio     = eedata.vdd_divider_ratio_ee;
    }
  check_EE_data();

  }
void check_EE_data(){
  // Prüfen ob ein wert im EEPROM nicht gültig ist, wenn ja Balancing nicht erlauben.
  if(!balancing_data_valid || isnan(bal_begin) || isnan(bal_stop) || isnan(bal_delay)){
      eeprom_error = 1;
      }else{
          if((bal_begin > 3650 && bal_stop > bal_begin) || bal_begin < 3300){
            balancing_data_valid = false;
            eeprom_error = 1;
            }else{
              eeprom_error = 0;
            }
        }
  // Prüfen ob EEPROM Daten für Initial Balancing gültig sind.      
  if(inital_voltage_valid){
    if(inital_voltage_start > 3800 || inital_voltage_start < 4000 || inital_voltage_stop > inital_voltage_start){
      inital_voltage_valid = true;
      }else{
        inital_voltage_valid = false;
        eeprom_error = eeprom_error + 2;
        }
    }else{
      eeprom_error = eeprom_error + 2;
  }
}

void read_id_ee(){
  SLAVEID = EEPROM.read(0);
  if(SLAVEID == 0xFF){
    SLAVEID_set = false;
  }else{
    SLAVEID_set = true;
  }
}

void clear_EEPROM(){
  for(int i = 0;i < 256;i++){
    EEPROM.write(i,0xFF);
    }
  delay(1000);
  _PROTECTED_WRITE(RSTCTRL.SWRR,1);
}