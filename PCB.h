/**
 * PCB Description
 * Components connections and orientation
 */

/** BatteryMonitor */
const uint8_t pinBatteryMonitor = A0;

/** KeyPad  */
const uint8_t pinKeyPad1 = A1;
const uint8_t pinKeyPad2 = A2;

/** Display old */
const uint8_t pinCS = 10;
const uint8_t pinDC = 9;
const uint8_t pinBacklight = 23;
/** Display new */
//const uint8_t pinCS = 22;
//const uint8_t pinDC = 20;
//const uint8_t pinBacklight = 23;
/** Display uses */
// pinMOSI=11, pinMISO=12, pinSCLK=13

const uint8_t uDisplayOrientation = 3;

/** Stepper connections */
const uint8_t pinSteppersEnable = 0;
const uint8_t pinPanStep = 3;
const uint8_t pinPanDirection = 6;
const uint8_t pinTiltStep = 4;
const uint8_t pinTiltDirection = 7;
const uint8_t pinSlideStep = 5;
const uint8_t pinSlideDirection = 8;

/** Bescor head connections */
// const uint8_t pinPanPWM = 3
// const uint8_t pinPanIn1 = 6
// const uint8_t pinPanIn2 = 7
// const uint8_t pinTiltPWM = 4
// const uint8_t pinTiltIn1 = 9
// const uint8_t pinTiltIn2 = 10

/** FAN PWM */
const uint8_t pinFanPWM = 22;

/** I2C */
const uint8_t pinI2C_SDA = 18;
const uint8_t pinI2C_SCL = 19;
const uint8_t pinI2C_Int = 20;
//const uint8_t pinI2C_Int = 17;


