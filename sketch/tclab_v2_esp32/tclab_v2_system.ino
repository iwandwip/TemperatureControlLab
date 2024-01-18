// return average  of n reads of thermister temperature in °C
inline float readTemperature(int pin) {
  float degC = 0.0;
  for (int i = 0; i < n; i++) {
    uint16_t adcRead = map(analogRead(pin), 0, 4095, 0, 1023);
    adcRead = (adcRead < 0) ? 0 : adcRead;

    degC += adcRead * 0.322265625 - 50.0;  // use for 3.3v AREF
    //degC += analogRead(pin) * 0.170898438 - 50.0;  // use for 1.75v AREF
  }
  return degC / float(n);
  // return analogRead(pin);  // for debugging only
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
    analogWritePWM(pinLED1, LED);
  } else {
    switch (ledStatus) {
      case 1:  // normal operation, heaters off
        analogWritePWM(pinLED1, loLED);
        break;
      case 2:  // normal operation, heater on
        analogWritePWM(pinLED1, hiLED);
        break;
      case 3:  // high temperature alarm, heater off
        if ((millis() % 2000) > 1000) {
          analogWritePWM(pinLED1, loLED);
        } else {
          analogWritePWM(pinLED1, loLED / 4);
        }
        break;
      case 4:  // high temperature alarm, heater on
        if ((millis() % 2000) > 1000) {
          analogWritePWM(pinLED1, hiLED);
        } else {
          analogWritePWM(pinLED1, loLED);
        }
        break;
    }
  }
}

void systemInit() {
  pinMode(pinT1, INPUT);
  pinMode(pinT2, INPUT);

  ledcSetup(pinQ1, freq, resolution);
  ledcAttachPin(pinQ1, pinQ1);  // pin ch

  ledcSetup(pinQ2, freq, resolution);
  ledcAttachPin(pinQ2, pinQ2);  // pin ch

  ledcSetup(pinLED1, freq, resolution);
  ledcAttachPin(pinLED1, pinLED1);  // pin ch
}

void analogWritePWM(uint8_t ch, int dutyCycle) {
  ledcWrite(ch, dutyCycle);
}

// set Heater 1
void setHeater1(float qval) {
  Q1 = max(0.0f, min(qval, 100.0f));
  analogWritePWM(pinQ1, (Q1 * P1) / 100);
}

// set Heater 2
void setHeater2(float qval) {
  Q2 = max(0.0f, min(qval, 100.0f));
  analogWritePWM(pinQ2, (Q2 * P2) / 100);
}