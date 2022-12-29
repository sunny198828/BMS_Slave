void setup_PWM(){
  pinMode(PWM1,OUTPUT);
  pinMode(PWM2,OUTPUT);
  PORTMUX.TCAROUTEA   = PORTMUX_TCA01_ALT1_gc | PORTMUX_TCA02_ALT1_gc;
  takeOverTCA0();                             // This replaces disabling and resettng the timer, required previously.
  TCA0.SINGLE.CTRLB   = TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
  TCA0.SINGLE.PER     = pwm_periode;               // Count all the way up to 0xFFFF
  TCA0.SINGLE.CMP2    = 0;
  TCA0.SINGLE.CMP1    = 0;
  TCA0.SINGLE.CTRLA   = TCA_SINGLE_ENABLE_bm; // Enable the timer with no prescaler
  pwm_active          = true;
}

void turnOFF_PWM(){
  takeOverTCA0();
  digitalWrite(PWM1, LOW);
  digitalWrite(PWM2, LOW);
  pwm_active          = false;
}

void set_pwm1(uint16_t data){
  if(!pwm_active){
    setup_PWM();
  }
  TCA0.SINGLE.CMP2    = data;
}

void set_pwm2(uint16_t data){
  if(!pwm_active){
    setup_PWM();
  }
  TCA0.SINGLE.CMP1    = data;
}