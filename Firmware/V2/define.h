HardwareSerial & BMS_communication = Serial1;
HardwareSerial & BMS_debug = Serial;
// TIMER TCB1 Setup nicht verändern
#if !( defined(MEGATINYCORE) )
  #error This is designed only for MEGATINYCORE megaAVR board! Please check your Tools->Board setting
#endif

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USING_FULL_CLOCK      true
#define USING_HALF_CLOCK      false
#define USING_250KHZ          false         // Not supported now

// Try to use RTC, TCA0 or TCD0 for millis()
#define USE_TIMER_0           true          // Check if used by millis(), Servo or tone()
#define USE_TIMER_1           false         // Check if used by millis(), Servo or tone()

#if USE_TIMER_0
  #define CurrentTimer   ITimer0
#elif USE_TIMER_1
  #define CurrentTimer   ITimer1
#else
  #error You must select one Timer  
#endif
bool TimerSetup_OK = false;
bool Timer2Setup_OK = false;
// Ende Setup TIMER TCB0

char BMS_communication_data[250] = "";
bool BMS_communication_data_available = false;
byte eeprom_error = 0;
// bool
bool DEBUG = false;
bool bal_thermal_shutdown = false;
bool vdd_divider_ratio_set = false;
bool Serial_OFF = false;
bool swap_pwm = false;
bool SLAVEID_set = false;
bool balance_activ = false;
// ADC RAW werte
long temp_chip_raw;
long vdd_chip_raw;
long vdd_voltagedivider_raw;
long voltage_shunt_bal_raw;
long ADC_extShunt_raw;
long ADC_extShunt_raw_offset;
int32_t ADC_Current_offset = 0;
double vdd_divider_ratio = 4.27;
// Berechnete Werte
double SHUNT_BAL_RES = 0.1;
double BAL_R = 1.0;
//double ext_shunt_R = 0.00075;
double ext_shunt_res = 0.0;
double temp_chip;
double vdd_chip;
double vdd_voltagedivider;
double current_bal_shunt;
double current_ext_shunt;
double BAL_CURRENT_calced;
double ADC_extShunt_voltage;
//Temperturen
double TEMP_SHUNT1  = -127.0;
double TEMP_SHUNT2  = -127.0;

// PWM werte
uint16_t pwm1 = 0;
uint16_t pwm2 = 0;
uint16_t pwm_periode = 2048;
uint16_t bal_delay = 250;
bool     pwm_active = true;

// BMS werte
byte accutype = 0;
bool balancing_data_valid = false;
bool inital_loading = false;
bool inital_balancing_active = false;
bool inital_voltage_valid = false;
bool ext_shunt = false;
byte NUM_resistors = 1;
double bal_begin = 0;
double bal_stop  = 0;
double inital_voltage_start = 0;
double inital_voltage_stop  = 0;
double MAX_current = 100;
//double ext_shunt_res = 0.0;
byte   SLAVEID   = 0;


// Defeniere Analoge Kanäle
#define Shunt_NEG     PIN_PA3
#define Shunt_POS     PIN_PC0
#define CUR_NEG       PIN_PA6
#define CUR_POS       PIN_PA7
#define VDD_POS       PIN_PC3

// Defeniere PWM Kanäla
#define PWM1          PIN_PB5
#define PWM2          PIN_PB4

// Defeiniere Eingänge
#define ALERT_IN      PIN_PA4
#define LM75_INT      PIN_PC4
#define LED_BAL       PIN_PB6

// Definiere Ausgänge
#define ALERT_OUT     PIN_PC5

// Defeniere Schnittstellen
#define EXT_INPUT     PIN_PB7
#define I2C_SDA       PIN_PB1
#define I2C_SCL       PIN_PB0
#define UART0_TX      PIN_PA1
#define UART0_RX      PIN_PA2