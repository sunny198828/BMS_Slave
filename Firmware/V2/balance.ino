long last_balance_count;
long last_balance_count_inital;

// Prüfen ob obere Spannung von der Zelle erreicht ist.


void balance(){
  // Prüfen ob die EEPROM daten gültig sind.
  // Prüfen ob der Balancing Widerstand zu heiß ist.
  // Prüfen ob eine Initialladung angefordert wurde.  
  if(!bal_thermal_shutdown && balancing_data_valid == 1 && !inital_loading){
      // Prüfen ob obere Spannung von der Zelle erreicht ist.
      if(vdd_voltagedivider > bal_begin && millis() - last_balance_count > bal_delay){
          last_balance_count = millis();
          digitalWrite(LED_BAL, !digitalRead(LED_BAL));
          balance_activ = true;
              if(NUM_resistors == 2){
                //swap_pwm
                if(swap_pwm){
                  if(pwm1 >= 0 && pwm1 <= pwm_periode && current_bal_shunt < MAX_current){
                    pwm1++;
                    set_pwm1(pwm1);
                    swap_pwm = false;
                    }
                  }else{
                    if(pwm2 >= 0 && pwm2 <= pwm_periode && current_bal_shunt < MAX_current){
                      pwm2++;
                      set_pwm2(pwm2);
                      swap_pwm = true;
                      }                    
                  }
              }else{
                if(pwm1 >= 0 && pwm1 <= pwm_periode && current_bal_shunt < MAX_current){
                  pwm1++;
                  set_pwm1(pwm1);
                  }
              }
      }
      // Prüfen ob die Spannung unter der Balancing Spannung liegt aber noch über der abschaltung.
      if(vdd_voltagedivider < bal_begin && vdd_voltagedivider > bal_stop && millis() - last_balance_count > bal_delay && !bal_thermal_shutdown && pwm_active && balancing_data_valid == 1 && !inital_loading){
          last_balance_count = millis();
          digitalWrite(LED_BAL,!digitalRead(LED_BAL));
            if(NUM_resistors == 2){
              if(swap_pwm){              
                if(pwm1 >= 0 && pwm1 <= pwm_periode){
                  pwm1--;
                  set_pwm1(pwm1);
                  swap_pwm = false;
                  }
                }else{
                  if(pwm2 >= 0 && pwm2 <= pwm_periode){
                    pwm2--;
                    set_pwm2(pwm2);
                    swap_pwm = true;
                    }                  
                  }
                }else{
                if(pwm1 >= 0 && pwm1 <= pwm_periode){
                  pwm1--;
                  set_pwm1(pwm1);
                  swap_pwm = false;
                  }
                }
      }
      // Prüfen ob die Spannung unterhalb der abschaltung liegt
      if((vdd_voltagedivider < bal_stop || bal_thermal_shutdown) && pwm_active){
          turnOFF_PWM();
          pwm1 = 0;
          pwm2 = 0;
          digitalWrite(LED_BAL, LOW);
          balance_activ = false;
      }          
  }

      //Prüfen ob einen Initalladung angefordert wurde und ob die EEPROM Daten gültig sind
      if(inital_loading && inital_voltage_valid){
          //Prüfen ob obere Spannung von Initialladung erreicht ist und PWM um eins erhöhen
          if(vdd_voltagedivider > inital_voltage_start && millis() - last_balance_count > bal_delay ){
              inital_balancing_active = true;
              last_balance_count = millis();
              if(millis()-last_balance_count_inital >= 1000){
                  last_balance_count_inital = millis();
                  digitalWrite(LED_BAL, !digitalRead(LED_BAL));
                  }
              if(pwm1 >= 0 && pwm1 <= pwm_periode){
                  pwm1++;
                  set_pwm1(pwm1);
                  }
            }
          //Prüfen ob obere Spannung unter dem Wert von Initalladung liegt aber oberehalb von der abschaltspannung  
          if(vdd_voltagedivider < inital_voltage_start && vdd_voltagedivider > inital_voltage_stop && millis() - last_balance_count > bal_delay && inital_balancing_active){
              if(millis()-last_balance_count_inital >= 1000){
                  last_balance_count_inital = millis();
                  digitalWrite(LED_BAL, !digitalRead(LED_BAL));
                  }
              if(pwm1 >= 0 && pwm1 <= pwm_periode){
                  pwm1--;
                  set_pwm1(pwm1);
                  }
            }
          // Prüfen ob Spannung unter Abschaltwert liegt
          if(vdd_voltagedivider < inital_voltage_stop){
              turnOFF_PWM();
              pwm1 = 0;
              pwm2 = 0;
              digitalWrite(LED_BAL,LOW);
              inital_balancing_active = false;
              }
        }

}