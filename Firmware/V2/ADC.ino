#include "Filter.h"
ExponentialFilter<long> ADC_ChipVoltage(10,0);
ExponentialFilter<long> ADC_Voltage(10, 0);
ExponentialFilter<long> ADC_Current(10, 0);
ExponentialFilter<long> ADC_extShunt(10, 0);
ExponentialFilter<long> ADC_ChipTemp(10,0);

unsigned int long debug_millis;

void measure(){

  //analogReference(INTERNAL1V024);
  //delay(1);
  if(ext_shunt){
    int32_t ADC_extShunt_temp = analogReadDiff(Shunt_POS,Shunt_NEG,17,16);
    ADC_extShunt.Filter(ADC_extShunt_temp);
    ADC_extShunt_raw = ADC_extShunt.Current()-ADC_extShunt_raw_offset;
    ADC_extShunt_voltage = (((1024.0/16)/0XFFFF)*ADC_extShunt_raw)*2;
    current_ext_shunt = ADC_extShunt_voltage/ext_shunt_res;
    }
  uint32_t temp_chip_temp = analogReadEnh(ADC_TEMPERATURE, 10);
  ADC_ChipTemp.Filter(temp_chip_temp);
  temp_chip_raw = ADC_ChipTemp.Current();
  temp_chip = (temp_chip_raw - 273.15)/10;

  uint32_t vdd_chip_temp = analogReadEnh(ADC_VDDDIV10, 12);
  ADC_ChipVoltage.Filter(vdd_chip_temp);
  vdd_chip_raw = ADC_ChipVoltage.Current();
  vdd_chip_raw >>= 2;
  vdd_chip = vdd_chip_raw*10;

  //analogReference(INTERNAL2V048);
  //delay(1);
// Spannungsteiler Spannung messen
  uint32_t vdd = analogReadEnh(VDD_POS, 16);
  ADC_Voltage.Filter(vdd);
  vdd_voltagedivider_raw = ADC_Voltage.Current();
  vdd_voltagedivider = ((1024.00 / 0xFFFF) * vdd_voltagedivider_raw)*vdd_divider_ratio;

  int32_t shunt_bal = analogReadDiff(CUR_POS,CUR_NEG,17,2);
  ADC_Current.Filter(shunt_bal);
  voltage_shunt_bal_raw = ADC_Current.Current()-ADC_Current_offset;
  current_bal_shunt = (((1024.0/2)/0XFFFF)*voltage_shunt_bal_raw)/SHUNT_BAL_RES;

  BAL_CURRENT_calced = ((vdd_voltagedivider / BAL_R)/pwm_periode)*(pwm1+pwm2);
}

void calc_shunt(){

  for(int i=0;i <= 20;i++){
  int32_t shunt_bal = analogReadDiff(CUR_POS,CUR_NEG,17,2);
  ADC_Current.Filter(shunt_bal);
  }
  voltage_shunt_bal_raw = ADC_Current.Current() - ADC_Current_offset;
  current_bal_shunt = (((1024.0/2)/0XFFFF)*voltage_shunt_bal_raw)/SHUNT_BAL_RES;
}

void calc_offset_adc(){
int32_t shunt_bal;
for(int i=0; i<= 30;i++){
  shunt_bal = analogReadDiff(CUR_POS,CUR_NEG,17,2);
  ADC_Current.Filter(shunt_bal);
  }
  ADC_Current_offset = ADC_Current.Current();
  BMS_debug.println(ADC_Current_offset);
}