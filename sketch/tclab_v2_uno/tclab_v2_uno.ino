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

// arduino startup
void setup() {
  analogReference(EXTERNAL);
  while (!Serial) {
    ;  // wait for serial port to connect.
  }
  Serial.begin(baud);
  Serial.flush();
  systemInit();
  setHeater1(0);
  setHeater2(0);
  ledTimeout = millis() + 1000;
}

// arduino main event loop
void loop() {
  systemTest();

  // readCommand();
  // if (DEBUG) echoCommand();
  // parseCommand();
  // dispatchCommand();
  // checkAlarm();
  // updateStatus();
}

void systemTest() {
  Serial.print("| pinT1: ");
  Serial.print(readTemperature(pinT1));
  Serial.print("| pinT2: ");
  Serial.print(readTemperature(pinT2));
  // Serial.print("| pinT3(A1): ");
  // Serial.print(readTemperature(A1));
  // Serial.print("| pinT4(A3): ");
  // Serial.print(readTemperature(A3));
  // Serial.print("| pinT5(A4): ");
  // Serial.print(readTemperature(A4));
  Serial.print("| pinLED1: ");
  Serial.print(digitalRead(pinLED1));

  analogWrite(pinQ1, 255);
  analogWrite(pinQ2, 255);
  analogWrite(pinLED1, 255);

  Serial.println();

  // for (int i = 0; i < 256; i++) {
  //   analogWrite(pinLED1, i);  // pwm
  //   delay(20);
  //   Serial.print("| i: ");
  //   Serial.print(i);
  //   Serial.println();
  // }

  // digitalWrite(pinLED1, !digitalRead(pinLED1));  // blink
  // digitalWrite(pinLED1, HIGH);
  // delay(250);
}