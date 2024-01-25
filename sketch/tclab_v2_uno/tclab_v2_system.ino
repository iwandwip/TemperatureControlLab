/*
 * The pin parameter is the pin number which must be capable of generating PWM signal.
 * For Arduino Nano or Arduino UNO the PWM pins are 3,5,6,9,10 and 11.
 * The pins 3, 9, 10 and 11 generates PWM frequency of 490Hz and pins 5 and 6 generates PWM frequency of 980Hz. "
 * reference: https://www.electronicwings.com/users/sanketmallawat91/projects/215/frequency-changing-of-pwm-pins-of-arduino-uno
 *          : https://code.google.com/archive/p/arduino-pwm-frequency-library/downloads
 */

#define ADC_VREF_mV 5000.0     // in millivolt
#define ADC_RESOLUTION 1024.0  // adc resolution
#define PWM_FREQUENCY 5        // pwm freq

// return average  of n reads of thermister temperature in °C
inline float readTemperature(int pin) {
  float degC = 0.0;
  for (int i = 0; i < n; i++) {
    // degC += analogRead(pin) * 0.322265625 - 50.0;  // use for 3.3v AREF
    //degC += analogRead(pin) * 0.170898438 - 50.0;  // use for 1.75v AREF

    int adcVal = analogRead(pin);
    float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);  // convert the ADC value to voltage in millivolt
    float tempC = milliVolt / 10;                               // convert the voltage to the temperature in °C
    degC += tempC;
  }
  return degC / float(n);
}

void checkAlarm(void) {
  if ((readTemperature(pinT1) > limT1) or (readTemperature(pinT2) > limT2)) {
    alarmStatus = 1;
  } else {
    alarmStatus = 0;
  }
}

void updateStatus(void) {
  // determine led status
  ledStatus = 1;
  if ((Q1 > 0) or (Q2 > 0)) {
    ledStatus = 2;
  }
  if (alarmStatus > 0) {
    ledStatus += 2;
  }
  // update led depending on ledStatus
  if (millis() < ledTimeout) {  // override led operation
    analogWrite(pinLED1, LED);
  } else {
    switch (ledStatus) {
      case 1:  // normal operation, heaters off
        analogWrite(pinLED1, loLED);
        break;
      case 2:  // normal operation, heater on
        analogWrite(pinLED1, hiLED);
        break;
      case 3:  // high temperature alarm, heater off
        if ((millis() % 2000) > 1000) {
          analogWrite(pinLED1, loLED);
        } else {
          analogWrite(pinLED1, loLED / 4);
        }
        break;
      case 4:  // high temperature alarm, heater on
        if ((millis() % 2000) > 1000) {
          analogWrite(pinLED1, hiLED);
        } else {
          analogWrite(pinLED1, loLED);
        }
        break;
    }
  }
}

void systemInit() {
  pinMode(pinT1, INPUT);
  pinMode(pinT2, INPUT);

  pinMode(pinQ1, OUTPUT);
  pinMode(pinQ2, OUTPUT);
  pinMode(pinLED1, OUTPUT);

  // InitTimersSafe();
  // bool successQ1 = SetPinFrequencySafe(pinQ1, 5);
  // if (!successQ1) return false;
  // bool successQ2 = SetPinFrequencySafe(pinQ2, 5);
  // if (!successQ2) return false;
  // bool successLED1 = SetPinFrequencySafe(pinLED1, 5);
  // if (!successLED1) return false;
  // return true;
}

void analogWritePWM(uint8_t ch, int dutyCycle) {
  // for esp32 only
}

// set Heater 1
void setHeater1(float qval) {
  Q1 = max(0., min(qval, 100.));
  analogWrite(pinQ1, (Q1 * P1) / 100);
}

// set Heater 2
void setHeater2(float qval) {
  Q2 = max(0., min(qval, 100.));
  analogWrite(pinQ2, (Q2 * P2) / 100);
}

// set LED
void setLED(float qval) {
  Q2 = max(0., min(qval, 100.));
  analogWrite(pinLED1, (Q2 * P2) / 100);
}
