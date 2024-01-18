/* https://apmonitor.com/pdc/index.php/Main/ArduinoTemperatureControl
  TCLab Temperature Control Lab Firmware
  Jeffrey Kantor, Bill Tubbs, John Hedengren, Shawn Summey
  February 2021

  This firmware provides a high level interface to the Temperature Control Lab. The
  firmware scans the serial port for commands. Commands are case-insensitive. Any
  unrecognized command results in sleep model. Each command returns a result string.

  A         software restart. Returns "Start".
  LED float set LED to float for 10 sec. range 0 to 100. Returns actual float
  P1 float  set pwm limit on heater 1, range 0 to 255. Default 200. Returns P1.
  P2 float  set pwm limit on heater 2, range 0 to 255. Default 100. Returns P2.
  Q1 float  set Heater 1, range 0 to 100. Returns value of Q1.
  Q2 float  set Heater 2, range 0 to 100. Returns value of Q2.
  Q1B float set Heater 1, range 0 to 100. Returns value of Q1 as a 32-bit float.
  Q2B float set Heater 2, range 0 to 100. Returns value of Q2 as a 32-bit float.
  R1        get value of Heater 1, range 0 to 100
  R2        get value of Heater 2, range 0 to 100
  SCAN      get values T1 T2 Q1 Q1 in line delimited values
  T1        get Temperature T1. Returns value of T1 in °C.
  T2        get Temperature T2. Returns value of T2 in °C.
  T1B       get Temperature T1. Returns value of T1 in °C as a 32-bit float.
  T2B       get Temperature T2. Returns value of T2 in °C as a 32-bit float.
  VER       get firmware version string
  X         stop, enter sleep mode. Returns "Stop"

  Limits on the heater power can be configured with the constants below.

  Status is indicated by LED1 on the Temperature Control Lab. Status conditions are:

      LED1        LED1
      Brightness  State
      ----------  -----
      dim         steady     Normal operation, heaters off
      bright      steady     Normal operation, heaters on
      dim         blinking   High temperature alarm on, heaters off
      bright      blinking   High temperature alarm on, heaters on

  The Temperature Control Lab shuts down the heaters if it receives no host commands
  during a timeout period (configure below), receives an "X" command, or receives
  an unrecognized command from the host.

  Constants are used to configure the firmware.

  Changelog ordered by Semantic Version
  
      1.0.1 first version included in the tclab package
      1.1.0 added R1 and R2 commands to read current heater values
            changed heater values to units of percent of full power
            added P1 and P2 commands to set heater power limits
            changed readCommand to avoid busy states
            changed simplified LED status model
      1.2.0 added LED command
      1.2.1 fixed reset heater values on close
            added version history
      1.2.2 changed version string for better display by TCLab
      1.2.3 changed baudrate to from 9600 to 115200
      1.3.0 added SCAN function 
            added board type in version string
      1.4.0 changed Q1 and Q2 to float from int
      1.4.1 fixed missing Serial.flush() at end of command loop
      1.4.2 fixed bug with X command
      1.4.3 deprecated use of Arduino IDE Version < 1.0.0
      1.5.0 removed webusb
      1.6.0 changed temperature to average 10 measurements to reduce noise
      2.0.0 added binary communications.
            added T1B and T2B commands return 32-bit float
            added Q1B and Q2B commands return 32-bit float confirmation of heater setting
            added calculation to use 1.75 AREF to match TMP36 voltage range 
      2.0.1 added updates to Notre Dame and BYU versions of this firmware
            changed version history to standard change log practices

*/

// determine board type
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
String boardType = "Arduino Uno";
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
String boardType = "Arduino Leonardo/Micro";
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
String boardType = "Arduino Mega";
#elif defined(ESP32)
String boardType = "ESP32";
#else
String boardType = "Unknown board";
#endif

// Enable debugging output
const bool DEBUG = false;

// constants
const String vers = "2.0.1";  // version of this firmware
const long baud = 115200;     // serial baud rate
const char sp = ' ';          // command separator
const char nl = '\n';         // command terminator

// pin numbers corresponding to signals on the TC Lab Shield
const int pinT1 = 14;   // T1
const int pinT2 = 27;   // T2
const int pinQ1 = 13;   // Q1
const int pinQ2 = 12;   // Q2
const int pinLED1 = 2;  // LED1

// temperature alarm limits
const int limT1 = 50;  // T1 high alarm (°C)
const int limT2 = 50;  // T2 high alarm (°C)

// LED1 levels
const int hiLED = 60;          // hi LED
const int loLED = hiLED / 16;  // lo LED

// setting PWM properties
const int freq = 1000;  // default arduino 490hz
const int ledChannel = 0;
const int resolution = 8;
