#include "Arduino.h"
#include "tclab_v2_utils.h"

// global variables
char Buffer[64];          // buffer for parsing serial input
int buffer_index = 0;     // index for Buffer
String cmd;               // command
float val;                // command value
int ledStatus;            // 1: loLED, 2: hiLED, 3: loLED blink, 4: hiLED blink
long ledTimeout = 0;      // when to return LED to normal operation
float LED = 100;          // LED override brightness
float P1 = 200;           // heater 1 power limit in units of pwm. Range 0 to 255
float P2 = 100;           // heater 2 power limit in units in pwm, range 0 to 255
float Q1 = 0;             // last value written to heater 1 in units of percent
float Q2 = 0;             // last value written to heater 2 in units of percent
int alarmStatus;          // hi temperature alarm status
boolean newData = false;  // boolean flag indicating new command
int n = 10;               // number of samples for each temperature measurement

// esp32 startup
void setup() {
  // analogReference(EXTERNAL); // set analog reference to aref pin (0 - 5V only)
  // while (!Serial) {
  //   ;  // wait for serial port to connect.
  // }
  Serial.begin(baud);
  if (DEBUG) {
    Serial.println("begin(baud)");
  }
  Serial.flush();
  if (DEBUG) {
    Serial.println("systemInit");
  }
  systemInit();
  setHeater1(0);
  setHeater2(0);
  ledTimeout = millis() + 1000;
}

// esp32 main event loop
void loop() {
  readCommand();
  if (DEBUG) echoCommand();
  parseCommand();
  dispatchCommand();
  checkAlarm();
  updateStatus();

  // for (uint8_t i = 0; i < 255; i++) {
  //   analogWritePWM(pinLED1, i);
  //   delay(30);
  //   Serial.println(i);
  // }
}