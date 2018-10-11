void output() {
  now = millis();
  controlTime = now - controlTimeOld;
  if (controlTime > CONTROLREFRESH) {
    controlTimeOld = now;
    controls();
  }
}

bool countdownStatus() {
  if (digitalRead(COUNTDOWNPIN)) {
    return false;
  } else {
    return true;
  }
}

bool stageStatus() {
  if (digitalRead(STAGEPIN)) {
    return false;
  } else {
    return true;
  }
}

void controls() {
  char temp_key;
  if (Connected) {
    if (stageStatus()) {
      MainControls(STAGE, HIGH);
    }
    else {
      MainControls(STAGE, LOW);
    }

    if (countdownStatus()) {
      countdownEnable = true;
      countdownTemp = slowBlinkState;
    }

    if (countdownEnable) {
      if (countdownCounter < 11) {
        dtostrf(countdownCounter, 8, 0, str_countdown);
        write7Segment(2, 0, str_countdown, 10);
      }
      if ((countdownTemp && !slowBlinkState) || (!countdownTemp && slowBlinkState)) {
        countdownTemp = slowBlinkState;
        countdownCounter--;
      }
      if (countdownCounter == 0) {
        MainControls(STAGE, HIGH);
        countdownTime = lastSlowBlink;
        long now = millis();
        if (now > countdownTime + 100) {
          MainControls(STAGE, LOW);
          countdownEnable = false;
          countdownCounter = 10;
        }
      }
    }

    temp_key = keypad_ag_sas.getKey();
  }

  //This is an example of reading analog inputs to an axis, with deadband and limits
  //CPacket.Throttle = constrain(map(analogRead(THROTTLEIN),THROTTLEDB,1024-THROTTLEDB,0,1000),0, 1000);

  KSPBoardSendData(details(CPacket));
} // End of controls()

void controlsInit() {
  pinMode(STAGEPIN, INPUT_PULLUP);
  pinMode(ABORTPIN, INPUT_PULLUP);
  pinMode(TESTPIN, INPUT_PULLUP);
  pinMode(RCSPIN, INPUT_PULLUP);
  pinMode(GEARPIN, INPUT_PULLUP);
  pinMode(BRAKESPIN, INPUT_PULLUP);
  pinMode(LIGHTSPIN, INPUT_PULLUP);
  pinMode(ORBSPEEDPIN, INPUT_PULLUP);
  pinMode(SURFSPEEDPIN, INPUT_PULLUP);
  pinMode(ASLSPEEDPIN, INPUT_PULLUP);

  pinMode(NAVORBITPIN, INPUT_PULLUP);
  pinMode(NAVSURFPIN, INPUT_PULLUP);
  pinMode(NAVTARGETPIN, INPUT_PULLUP);

  pinMode(COUNTDOWNPIN, INPUT_PULLUP);

  pinMode(LEDALTRADAR, OUTPUT);
  pinMode(LEDALTASL, OUTPUT);
  pinMode(LEDSPEEDORB, OUTPUT);
  pinMode(LEDSPEEDSURF, OUTPUT);
  pinMode(LEDSPEEDASL, OUTPUT);
  pinMode(LEDTIMETONODEFROM, OUTPUT);
  pinMode(LEDTIMETONODETO, OUTPUT);
  pinMode(LEDINC, OUTPUT);
  pinMode(LEDEXC, OUTPUT);
}

void keypadEvent(KeypadEvent key) {
  byte numKey = 0;
  tempSASMode = getSASMode();
  switch (keypad_ag_sas.getState()) {
    case PRESSED:
      if (isdigit(key)) {
        bitSet(CPacket.ControlGroup, (key - '0'));
      }
      else if (isalpha(key)) {
        numKey = key - 'A';
        if (numKey == tempSASMode) {
          setSASMode(0);
        }
        else {
          setSASMode(numKey);
        }
      }
      break;

    case HOLD:
      break;

    case RELEASED:
      if (isdigit(key)) {
        bitClear(CPacket.ControlGroup, (key - '0'));
      }
      break;
  }
}

byte getSASMode() {
  return VData.NavballSASMode & B00001111; // leaves alone the lower 4 bits of; all higher bits set to 0.
}

byte getNavballMode() {
  return VData.NavballSASMode >> 4; // leaves alone the higher 4 bits of; all lower bits set to 0.
}

void setSASMode(byte m) {
  CPacket.NavballSASMode &= B11110000;
  CPacket.NavballSASMode += m;
}

void setNavballMode(byte m) {
  CPacket.NavballSASMode &= B00001111;
  CPacket.NavballSASMode += m << 4;
}

void MainControls(byte n, boolean s) {
  if (s)
    CPacket.MainControls |= (1 << n);       // forces nth bit of x to be 1.  all other bits left alone.
  else
    CPacket.MainControls &= ~(1 << n);      // forces nth bit of x to be 0.  all other bits left alone.
}

void ControlGroups(byte n, boolean s) {
  if (s)
    CPacket.ControlGroup |= (1 << n);       // forces nth bit of x to be 1.  all other bits left alone.
  else
    CPacket.ControlGroup &= ~(1 << n);      // forces nth bit of x to be 0.  all other bits left alone.
}




