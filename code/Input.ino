int input() {
  int returnValue = -1;
  now = millis();

  if (KSPBoardReceiveData()) {
    deadtimeOld = now;
    returnValue = id;
    switch (id) {
      case 0: //Handshake packet
        Handshake();
        break;
      case 1:
        displays();
        break;
    }

    Connected = true;
  }
  else
  { //if no message received for a while, go idle
    deadtime = now - deadtimeOld;
    if (deadtime > IDLETIMER)
    {
      deadtimeOld = now;
      Connected = false;
      notConnected();
    }
  }
  return returnValue;
}

void displays() {
  if (Connected) {
    leds[6] = CRGB::Green;

    if (VData.e < 1) {
      apsideCalc(VData.AP, unitAP, str_tempAP);
      timeCalc(VData.TAp, str_hourAP, dispsecAP);
      apsideCalc(VData.PE, unitPE, str_tempPE);
      timeCalc(VData.TPe, str_hourPE, dispsecPE);
    }
    else {
      unitAP[0] = ' ';
      unitAP[1] = ' ';
      tempAP_string = " ESCAPE ";
      tempAP_string.toCharArray(str_tempAP, 9);
      tempAP_string.toCharArray(str_hourAP, 9);
      unitPE[0] = ' ';
      unitPE[1] = ' ';
      tempPE_string = "        ";
      tempPE_string.toCharArray(str_tempPE, 9);
      tempPE_string.toCharArray(str_hourPE, 9);
    }

    if (!countdownEnable) {
      timeCalc(VData.MissionTime, str_hourMI, dispsecMI);
      write7Segment(2, 0, str_hourMI, 10);
    }

    write7Segment(1, 3, str_tempAP, 10);
    write7Segment(1, 2, str_hourAP, 10);
    write7Segment(1, 1, str_tempPE, 10);
    write7Segment(1, 0, str_hourPE, 10);
    alpha4.writeDigitAscii(2, unitPE[0]);
    alpha4.writeDigitAscii(3, unitPE[1]);
    alpha4.writeDigitAscii(0, unitAP[0]);
    alpha4.writeDigitAscii(1, unitAP[1]);
    alpha4.writeDisplay();


    if (VData.Alt < 10000000.0) {
      apsideCalc(VData.Alt, unitAlt, str_alt);
      write7Segment(2, 1, str_alt, 10);
    }
    else {
      write7Segment(2, 1, "--------", 10);
    }

    navMode = getNavballMode();
    switch (navMode) {
      case 1:
        dtostrf(VData.VOrbit, 8, 0, str_VOrbit);
        break;
      case 2:
        dtostrf(VData.Vsurf, 8, 0, str_VOrbit);
        break;
      case 3:
        dtostrf(VData.TargetV, 8, 0, str_VOrbit);
        break;
      default:
        break;
    }
    write7Segment(2, 2, str_VOrbit, 10);

    dtostrf(VData.e, 8, 3, str_e);
    str_e[4] = str_e[3];
    str_e[3] = str_e[2];
    str_e[2] = str_e[1];
    str_e[1] = str_e[0];
    str_e[0] = ' ';
    write7Segment(2, 3, str_e, 3);

    tempMNTime = (long)VData.MNTime;
    if ((tempMNTime == 0) && (VData.MNDeltaV == 0)) {
      write7Segment(2, 4, "        ", 10);
      write7Segment(2, 5, "        ", 10);
    }
    else {
      if (tempMNTime < 0) {
        tempMNTime = -tempMNTime;
        negMNTime = true;
      }
      else {
        lc1.setRow(4, 0, B00000000);
        negMNTime = false;
      }
      nodeTimeCalc(tempMNTime, str_MNTime, dispsecMNTime, negMNTime);
      write7Segment(2, 4, str_MNTime, 2 * nodeTimesec);

      if (VData.MNDeltaV <= 9999999.9) {
        dtostrf(VData.MNDeltaV, 8, 1, str_MNDeltaV);
        str_MNDeltaV[6] = str_MNDeltaV[5];
        str_MNDeltaV[5] = str_MNDeltaV[4];
        str_MNDeltaV[4] = str_MNDeltaV[3];
        str_MNDeltaV[3] = str_MNDeltaV[2];
        str_MNDeltaV[2] = str_MNDeltaV[1];
        str_MNDeltaV[1] = str_MNDeltaV[0];
        str_MNDeltaV[0] = ' ';
        write7Segment(2, 5, str_MNDeltaV, 1);
      }
      else {
        write7Segment(2, 5, "--------", 10);
      }
    }

    blinkUpdate();
    SASLeds();
    gLightSet();
    stageLedSet();
    rcsLightSet();
    sasLightSet();
    solidLightSet();
    oxygenLightSet();
    liquidLightSet();
    gearLedSet();
    brakeLedSet();
    lightLedSet();
    xenonLightSet();
    powerLightSet();
    heatLightSet();
    chuteLightSet();
    monopropLightSet();

    FastLED.show();
  }
  else {
    notConnected();
  }
}

void SASLeds() {
  byte tempSASLed;
  tempSASLed = getSASMode();
  FastLED.clear();
  ledsSAS[convSASLeds[tempSASLed - 1]] = CHSV(0, 0, 64);
}

void stageLedSet() {
  if (ControlStatus(AGAbort)) {
    if (fastBlinkState) {
      leds[5] = CRGB::Red;
    }
    else {
      leds[5] = CRGB::Black;
    }
  }
}


void gearLedSet() {
  if (ControlStatus(AGGear)) {
    leds[12] = CRGB::Green;
  }
  else {
    leds[12] = CRGB::Black;
  }
}

void brakeLedSet() {
  if (ControlStatus(AGBrakes)) {
    leds[11] = CRGB::Green;
  }
  else {
    leds[11] = CRGB::Black;
  }
}

void lightLedSet() {
  if (ControlStatus(AGLight)) {
    leds[10] = CRGB::Green;
  }
  else {
    leds[10] = CRGB::Black;
  }
}

void powerLightSet() {
  byte remainingPowerPercent = byte(VData.ECharge / VData.EChargeTot * 100.0);
  if (remainingPowerPercent == 0) {
    leds[0] = CHSV(0, 255, colour_value);
  }
  else if (remainingPowerPercent < 5) {
    if (fastBlinkState) {
      leds[0] = CHSV(0, 255, colour_value);
    }
    else {
      leds[0] = CHSV(45, 255, colour_value);
    }
  }
  else if (remainingPowerPercent < 10) {
    if (slowBlinkState) {
      leds[0] = CHSV(0, 255, colour_value);
    }
    else {
      leds[0] = CHSV(45, 255, colour_value);
    }
  }
  else if (remainingPowerPercent < 20) {
    leds[0] = CHSV(45, 255, colour_value);
  }
  else {
    leds[0] = CRGB::Green;
  }
}

void heatLightSet() {
  if (VData.MaxOverHeat > 95) {
    if (fastBlinkState) {
      leds[16] = CHSV(10, 255, colour_value);
    }
    else {
      leds[16] = CRGB::Black;
    }
  }
  else if (VData.MaxOverHeat > 90) {
    leds[16] = CHSV(10, 255, colour_value);
  }
  else if (VData.MaxOverHeat > 80) {
    leds[16] = CHSV(64, 255, colour_value);
  }
}

void gLightSet() {
  if (VData.G > 9.0) {
    leds[7] = CHSV(0, 255, colour_value);
  }
  else if (VData.G > 5.0) {
    leds[7] = CHSV(32, 255, colour_value);
  }
  else if (VData.G > 2.0) {
    leds[7] = CHSV(64, 255, colour_value);
  }
  else if (VData.G <= 2.0) {
    leds[7] = CHSV(96, 255, colour_value);
  }
}

void chuteLightSet() {
  if (VData.VVI < 0 && VData.Alt < 30000.0) {
    if (VData.MachNumber > 1.2) {
      // "unsafe"
      if (slowBlinkState) {
        leds[18] = CHSV(0, 255, colour_value);
      }
      else {
        leds[18] = CRGB::Black;
      }
    }
    else if (VData.MachNumber > 0.98) {
      // "risky"
      leds[18] = CHSV(32, 255, colour_value);
    }
    else if (VData.MachNumber > 0.1) {
      leds[18] = CHSV(96, 255, colour_value);
    }
    else {
      leds[18] = CRGB::Black;
    }
  }
}

void solidLightSet() {
  if (VData.SolidFuelTot > 0.001) {
    byte hue = byte(VData.SolidFuel / VData.SolidFuelTot * 96.0);
    leds[2] = CHSV(hue, 255, colour_value);
  }
  else {
    leds[2] = CRGB::Black;
  }
}

void xenonLightSet() {
  if (VData.XenonGasTot > 0.001) {
    byte hue = byte(VData.XenonGas / VData.XenonGasTot * 96.0);
    leds[2] = CHSV(hue, 255, colour_value);
  }
  else {
    leds[2] = CRGB::Black;
  }
}

void oxygenLightSet() {
  if (VData.OxidizerTotS > 0.001) {
    byte hue = byte(VData.OxidizerS / VData.OxidizerTotS * 96.0);
    leds[3] = CHSV(hue, 255, colour_value);
  }
  else {
    leds[3] = CRGB::Black;
  }
}

void liquidLightSet() {
  if (VData.LiquidFuelTotS > 0.001) {
    byte hue = byte(VData.LiquidFuelS / VData.LiquidFuelTotS * 96.0);
    leds[4] = CHSV(hue, 255, colour_value);
  }
  else {
    leds[4] = CRGB::Black;
  }
}

void sasLightSet() {
  if (ControlStatus(AGSAS)) {
    leds[13] = CRGB::Green;
  }
  else {
    leds[13] = CRGB::Black;
  }
}

void rcsLightSet() {
  if (ControlStatus(AGRCS)) {
    leds[14] = CRGB::Green;
  }
  else {
    leds[14] = CRGB::Black;
  }
}

void monopropLightSet() {
  if (VData.MonoPropTot > 0.001) {
    // Wipe from green to red depending on how much mono is available
    byte hue = byte(VData.MonoProp / VData.MonoPropTot * 96);
    leds[1] = CHSV(hue, 255, colour_value);
  }
  else {
    leds[1] = CRGB::Black;
  }
}

byte ControlStatus(byte n)
{
  return ((VData.ActionGroups >> n) & 1) == 1;
}

