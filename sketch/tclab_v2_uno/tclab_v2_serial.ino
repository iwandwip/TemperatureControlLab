void readCommand() {
  while (Serial && (Serial.available() > 0) && (newData == false)) {
    int byte = Serial.read();
    if ((byte != '\r') && (byte != nl) && (buffer_index < 64)) {
      Buffer[buffer_index] = byte;
      buffer_index++;
    } else {
      newData = true;
    }
  }
}

// for debugging with the serial monitor in Arduino IDE
void echoCommand() {
  if (newData) {
    Serial.write("Received Command: ");
    Serial.write(Buffer, buffer_index);
    Serial.write(nl);
    Serial.flush();
  }
}

void parseCommand(void) {
  if (newData) {
    String read_ = String(Buffer);

    // separate command from associated data
    int idx = read_.indexOf(sp);
    cmd = read_.substring(0, idx);
    cmd.trim();
    cmd.toUpperCase();

    // extract data. toFloat() returns 0 on error
    String data = read_.substring(idx + 1);
    data.trim();
    val = data.toFloat();

    // reset parameter for next command
    memset(Buffer, 0, sizeof(Buffer));
    buffer_index = 0;
    newData = false;
  }
}

void dispatchCommand(void) {
  if (cmd == "A") {
    setHeater1(0);
    setHeater2(0);
    sendResponse("Start");
  } else if (cmd == "LED") {
    ledTimeout = millis() + 10000;
    LED = max(0, min(100, val));
    sendResponse(String(LED));
  } else if (cmd == "P1") {
    P1 = max(0, min(255, val));
    sendResponse(String(P1));
  } else if (cmd == "P2") {
    P2 = max(0, min(255, val));
    sendResponse(String(P2));
  } else if (cmd == "Q1") {
    setHeater1(val);
    sendFloatResponse(Q1);
  } else if (cmd == "Q1B") {
    setHeater1(val);
    sendBinaryResponse(Q1);
  } else if (cmd == "Q2") {
    setHeater2(val);
    sendFloatResponse(Q2);
  } else if (cmd == "Q2B") {
    setHeater1(val);
    sendBinaryResponse(Q2);
  } else if (cmd == "R1") {
    sendFloatResponse(Q1);
  } else if (cmd == "R2") {
    sendFloatResponse(Q2);
  } else if (cmd == "SCAN") {
    sendFloatResponse(readTemperature(pinT1));
    sendFloatResponse(readTemperature(pinT2));
    sendFloatResponse(Q1);
    sendFloatResponse(Q2);
  } else if (cmd == "T1") {
    sendFloatResponse(readTemperature(pinT1));
  } else if (cmd == "T1B") {
    sendBinaryResponse(readTemperature(pinT1));
  } else if (cmd == "T2") {
    sendFloatResponse(readTemperature(pinT2));
  } else if (cmd == "T2B") {
    sendBinaryResponse(readTemperature(pinT2));
  } else if (cmd == "VER") {
    sendResponse("TCLab Firmware " + vers + " " + boardType);
  } else if (cmd == "X") {
    setHeater1(0);
    setHeater2(0);
    sendResponse("Stop");
  } else if (cmd.length() > 0) {
    setHeater1(0);
    setHeater2(0);
    sendResponse(cmd);
  }
  Serial.flush();
  cmd = "";
}

void sendResponse(String msg) {
  Serial.println(msg);
}

void sendFloatResponse(float val) {
  Serial.println(String(val, 3));
}

void sendBinaryResponse(float val) {
  byte *b = (byte *)&val;
  Serial.write(b, 4);
}