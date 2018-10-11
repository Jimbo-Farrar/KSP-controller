void apsideCalc(float apside, char* unitApside, char* str_tempApside) {
  float tempApside;
  if (abs(apside) >= 1000000.0 && abs(apside) < 1000000000.0) {
    tempApside = apside / 1000.0;
    unitApside[0] = 'k';
    unitApside[1] = 'm';
  }
  else if (abs(apside) >= 1000000000.0 && abs(apside) < 1000000000000.0) {
    tempApside = apside / 1000000.0;
    unitApside[0] = 'M';
    unitApside[1] = 'm';
  }
  else if (abs(apside) >= 1000000000000.0) {
    tempApside = apside / 1000000000.0;
    unitApside[0] = 'G';
    unitApside[1] = 'm';
  }
  else {
    tempApside = apside;
    unitApside[0] = 'm';
    unitApside[1] = ' ';
  }
  dtostrf(tempApside, 8, 0, str_tempApside);
}

void blinkUpdate() {
  long now = millis();
  if (now > lastSlowBlink + slowBlinkRate) {
    lastSlowBlink = now;
    slowBlinkState = !slowBlinkState;
  }
  if (now > lastFastBlink + fastBlinkRate) {
    lastFastBlink = now;
    fastBlinkState = !fastBlinkState;
  }
}


void initLEDS() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}


void demo() {
  write7Segment(1, 0, "88888888", 10);
  write7Segment(1, 1, "88888888", 10);
  write7Segment(1, 2, "88888888", 10);
  write7Segment(1, 3, "88888888", 10);
  write7Segment(2, 0, "88888888", 10);
  write7Segment(2, 1, "88888888", 10);
  write7Segment(2, 2, "88888888", 10);
  write7Segment(2, 3, "88888888", 10);
  write7Segment(2, 4, "88888888", 10);
  write7Segment(2, 5, "88888888", 10);

  alpha4.writeDigitRaw(0, 0x3FFF);
  alpha4.writeDigitRaw(1, 0x3FFF);
  alpha4.writeDigitRaw(2, 0x3FFF);
  alpha4.writeDigitRaw(3, 0x3FFF);
  alpha4.writeDisplay();

  for (int i = 0; i < ANNUNCIATOR0NUM; i++) {
    leds[i] = CHSV(96, 255, 128);
  }
  for (int i = 0; i < ANNUNCIATOR1NUM; i++) {
    ledsSAS[i] = CHSV(0, 0, 64);
  }
  FastLED.show();
}

void initAlpha4() {
  alpha4.begin(0x70);
  alpha4.setBrightness(0);
  alpha4.clear();
  alpha4.writeDisplay();
}

void initANNUN() {
  for (int i = 0; i < ANNUNCIATOR0NUM; i++) {
    leds[i] = CHSV(0, 255, 0);
  }
  for (int i = 0; i < ANNUNCIATOR1NUM; i++) {
    ledsSAS[i] = CHSV(0, 0, 0);
  }
  FastLED.show();
}


void initSEVENsegs() {
  for (int i = 0; i < 4; i++) {
    lc1.shutdown(i, false);
    lc1.setIntensity(i, 1);
    write7Segment(1, i, "        ", 10);
  }
  for (int i = 0; i < 6; i++) {
    lc2.shutdown(i, false);
    lc2.setIntensity(i, 0);
    write7Segment(2, i, "        ", 10);
  }
}

void InitTxPackets() {
  HPacket.id = 0;
  CPacket.id = 101;
} //End InitTxPackets

void notConnected() {
  digitalWrite(13, LOW);

  for (int i = 0; i < 4; i++) {
    lc1.clearDisplay(i);
  }
  for (int i = 0; i < 6; i++) {
    lc2.clearDisplay(i);
  }

  alpha4.clear();
  alpha4.writeDisplay();

  for (int i = 0; i < ANNUNCIATOR0NUM; i++) {
    leds[i] = CRGB::Black;
  }
  leds[6] = CHSV(0, 255, 128);
  for (int i = 0; i < ANNUNCIATOR1NUM; i++) {
    ledsSAS[i] = CRGB::Black;
  }
  FastLED.show();
}


void timeCalc(long counter, char* str_hour, int &dispsec) {
  int years, days, hours, minutes, seconds;
  years = counter / 9203400;
  counter -= (years * 9203400L);
  days = counter / 21600;
  counter -= (days * 21600L);
  hours = counter / 3600;
  counter -= (hours * 3600L);
  minutes = counter / 60;
  counter -= (minutes * 60L);
  seconds = counter;
  if (years >= 1) {
    if (years < 10) {
      str_hour[0] = ' ';
      str_hour[1] = years + '0';
    }
    else {
      str_hour[0] = (years / 10) + '0';
      str_hour[1] = (years % 10) + '0';
    }
    str_hour[2] = 'A';
    str_hour[3] = ' ';

    if (days < 10) {
      str_hour[4] = '0';
      str_hour[5] = '0';
      str_hour[6] = days + '0';
    }
    else if (days < 100) {
      str_hour[4] = '0';
      str_hour[5] = (days / 10) + '0';
      str_hour[6] = (days % 10) + '0';
    }
    else if (days >= 100) {
      str_hour[4] = (days / 100) + '0';
      str_hour[5] = ((days - ((days / 100) * 100)) / 10) + '0';
      str_hour[6] = (days % 10) + '0';
    }
    str_hour[7] = 'd';
  }
  else if (years < 1 && days >= 1) {
    if (days < 100) {
      if (days < 10) {
        str_hour[0] = ' ';
        str_hour[1] = days + '0';
      }
      else {
        str_hour[0] = (days / 10) + '0';
        str_hour[1] = (days % 10) + '0';
      }
      str_hour[2] = 'd';
      if (hours < 10) {
        str_hour[3] = '0';
        str_hour[4] = hours + '0';
      }
      else {
        str_hour[3] = abs((hours / 10)) + '0';
        str_hour[4] = (hours % 10) + '0';
      }

      str_hour[5] = 'h';
      if (minutes < 10) {
        str_hour[6] = '0';
        str_hour[7] = minutes + '0';
      }
      else {
        str_hour[6] = abs((minutes / 10)) + '0';
        str_hour[7] = (minutes % 10) + '0';
      }
    }
    else {
      char buf[4];
      sprintf(buf, "%03i", days);
      str_hour[0] = ' ';
      str_hour[1] = buf[0];
      str_hour[2] = buf[1];
      str_hour[3] = buf[2];
      str_hour[4] = 'd';
      if (hours < 10) {
        str_hour[5] = '0';
        str_hour[6] = hours + '0';
      }
      else {
        str_hour[5] = abs((hours / 10)) + '0';
        str_hour[6] = (hours % 10) + '0';
      }
      str_hour[7] = 'h';
    }
  }
  else if (years < 1 && days < 1) {
    if (seconds != dispsec) {
      if (hours < 10) {
        str_hour[0] = '0';
        str_hour[1] = hours + '0';
      }
      else {
        str_hour[0] = abs((hours / 10)) + '0';
        str_hour[1] = (hours % 10) + '0';
      }
      str_hour[2] = 'h';
      if (minutes < 10) {
        str_hour[3] = '0';
        str_hour[4] = minutes + '0';
      }
      else {
        str_hour[3] = abs((minutes / 10)) + '0';
        str_hour[4] = (minutes % 10) + '0';
      }
      str_hour[5] = '-';
      if (seconds < 10) {
        str_hour[6] = '0';
        str_hour[7] = seconds + '0';
      }
      else {
        str_hour[6] = abs((seconds / 10)) + '0';
        str_hour[7] = (seconds % 10) + '0';
      }
      dispsec = seconds;
    }
  }
} //End timeCalc

void nodeTimeCalc(long counter, char* str_hour, int &dispsec, bool neg) {
  bool negTest;
  int years, days, hours, minutes, seconds;
  negTest = neg;
  years = counter / 9203400;
  counter -= (years * 9203400L);
  days = counter / 21600;
  counter -= (days * 21600L);
  hours = counter / 3600;
  counter -= (hours * 3600L);
  minutes = counter / 60;
  counter -= (minutes * 60L);
  seconds = counter;
  if (years >= 1) {
    if (years < 10) {
      str_hour[0] = ' ';
      str_hour[1] = years + '0';
    }
    else {
      str_hour[0] = (years / 10) + '0';
      str_hour[1] = (years % 10) + '0';
    }
    str_hour[2] = 'A';
    str_hour[3] = ' ';

    if (days < 10) {
      str_hour[4] = '0';
      str_hour[5] = '0';
      str_hour[6] = days + '0';
    }
    else if (days < 100) {
      str_hour[4] = '0';
      str_hour[5] = (days / 10) + '0';
      str_hour[6] = (days % 10) + '0';
    }
    else if (days >= 100) {
      str_hour[4] = (days / 100) + '0';
      str_hour[5] = ((days - ((days / 100) * 100)) / 10) + '0';
      str_hour[6] = (days % 10) + '0';
    }
    str_hour[7] = 'd';
  }
  else if (years < 1 && days >= 1) {
    if (days < 100) {
      if (days < 10) {
        str_hour[0] = ' ';
        str_hour[1] = days + '0';
      }
      else {
        str_hour[0] = (days / 10) + '0';
        str_hour[1] = (days % 10) + '0';
      }
      str_hour[2] = 'd';
      if (hours < 10) {
        str_hour[3] = '0';
        str_hour[4] = hours + '0';
      }
      else {
        str_hour[3] = abs((hours / 10)) + '0';
        str_hour[4] = (hours % 10) + '0';
      }

      str_hour[5] = 'h';
      if (minutes < 10) {
        str_hour[6] = '0';
        str_hour[7] = minutes + '0';
      }
      else {
        str_hour[6] = abs((minutes / 10)) + '0';
        str_hour[7] = (minutes % 10) + '0';
      }
    }
    else {
      char buf[4];
      sprintf(buf, "%03i", days);
      str_hour[0] = ' ';
      str_hour[1] = buf[0];
      str_hour[2] = buf[1];
      str_hour[3] = buf[2];
      str_hour[4] = 'd';
      if (hours < 10) {
        str_hour[5] = '0';
        str_hour[6] = hours + '0';
      }
      else {
        str_hour[5] = abs((hours / 10)) + '0';
        str_hour[6] = (hours % 10) + '0';
      }
      str_hour[7] = 'h';
    }
  }
  else if (years < 1 && days < 1) {
    if (seconds != dispsec) {
      if (negTest == false) {
        str_hour[0] = '-';
      }
      else {
        str_hour[0] = ' ';
      }
      if (hours < 10) {
        str_hour[1] = '0';
        str_hour[2] = hours + '0';
      }
      else {
        str_hour[1] = abs((hours / 10)) + '0';
        str_hour[2] = (hours % 10) + '0';
      }
      str_hour[3] = 'h';
      if (minutes < 10) {
        str_hour[4] = '0';
        str_hour[5] = minutes + '0';
      }
      else {
        str_hour[4] = abs((minutes / 10)) + '0';
        str_hour[5] = (minutes % 10) + '0';
      }
      // str_hour[5] = '-';
      nodeTimesec = true;
      if (seconds < 10) {
        str_hour[6] = '0';
        str_hour[7] = seconds + '0';
      }
      else {
        str_hour[6] = abs((seconds / 10)) + '0';
        str_hour[7] = (seconds % 10) + '0';
      }
      dispsec = seconds;
    }
  }
} //End nodeTimeCalc

void write7Segment(byte controller, byte display, char value[9], byte decimal)
{
  switch (controller) {
    case 1:
      for (byte j = 0; j < 8; j++) {
        lc1.setChar(display, j, value[7 - j], decimal == j);
      }
      break;
    case 2:
      for (byte j = 0; j < 8; j++) {
        lc2.setChar(display, j, value[7 - j], decimal == j);
      }
      break;
  }
}

