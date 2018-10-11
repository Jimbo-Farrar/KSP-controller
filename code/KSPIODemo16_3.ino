#include <Keypad.h>
#include <LedControl.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <FastLED.h>
#include <L293.h>

#define ANNUNCIATOR0NUM 20
#define ANNUNCIATOR1NUM 10
//pins for annunciators
#define ANNUNCIATOR0PIN 6
#define ANNUNCIATOR1PIN 7

//pins for input
#define COUNTDOWNPIN 5

#define STAGEPIN 8
#define ABORTPIN 9
#define TESTPIN 10
#define RCSPIN 11
#define GEARPIN 12
#define BRAKESPIN 45
#define LIGHTSPIN 14
#define THROTTLESENSE1 15
#define THROTTLESENSE2 16
#define ORBSPEEDPIN 17
#define SURFSPEEDPIN 18
#define ASLSPEEDPIN 19

#define AG_SAS_ROW1PIN 22
#define AG_SAS_ROW2PIN 23
#define AG_SAS_ROW3PIN 24
#define AG_SAS_ROW4PIN 25

#define AG_SAS_COL1PIN 26
#define AG_SAS_COL2PIN 27
#define AG_SAS_COL3PIN 28
#define AG_SAS_COL4PIN 29
#define AG_SAS_COL5PIN 30

#define LEDALTRADAR 31
#define LEDALTASL 32
#define LEDSPEEDORB 33
#define LEDSPEEDSURF 34
#define LEDSPEEDASL 35
// #define LEDMISSIONTIME 36
#define LEDINC 37
#define LEDEXC 38
#define LEDTIMETONODEFROM 39
#define LEDTIMETONODETO 40

#define JOYSTICK0SWITCH 41

#define NAVORBITPIN 42
#define NAVSURFPIN 43
#define NAVTARGETPIN 44

#define JOYSTICK0X A0
#define JOYSTICK0Y A1
#define JOYSTICK0Z A2
#define JOYSTICK1X A3
#define JOYSTICK1Y A4
#define JOYSTICK1Z A5
#define THROTTLEIN A6
#define THROTTLESERVO A7

#define THROTTLEDB 4 //Throttle axis deadband

//Input enums
#define SAS 7
#define RCS 6
#define LIGHTS 5
#define GEAR 4
#define BRAKES 3
#define PRECISION 2
#define ABORT 1
#define STAGE 0

//Action group statuses
#define AGSAS      0
#define AGRCS      1
#define AGLight    2
#define AGGear     3
#define AGBrakes   4
#define AGAbort    5
#define AGCustom01 6
#define AGCustom02 7
#define AGCustom03 8
#define AGCustom04 9
#define AGCustom05 10
#define AGCustom06 11
#define AGCustom07 12
#define AGCustom08 13
#define AGCustom09 14
#define AGCustom10 15

//SAS Modes
#define SMOFF           0
#define SMSAS           1
#define SMPrograde      2
#define SMRetroGrade    3
#define SMNormal        4
#define SMAntinormal    5
#define SMRadialIn      6
#define SMRadialOut     7
#define SMTarget        8
#define SMAntiTarget    9
#define SMManeuverNode  10

//Navball Target Modes
#define NAVBallIGNORE   0
#define NAVBallORBIT    1
#define NAVBallSURFACE  2
#define NAVBallTARGET   3

//macro
#define details(name) (uint8_t*)&name,sizeof(name)

//if no message received from KSP for more than 2s, go idle
#define IDLETIMER 2000
#define CONTROLREFRESH 25

#define MAX7219DIN 51
#define MAX7219CLK 52
#define MAX7219LOAD 53
#define MAX7219NUM 4

#define MAX7219_2DIN 47
#define MAX7219_2CLK 48
#define MAX7219_2LOAD 49
#define MAX7219_2NUM 6

bool Connected = false;
bool negMNTime = false;

byte tempSASMode;
byte caution = 0, warning = 0, id;
byte navMode = 0;
byte countdownCounter = 11;
bool countdownEnable = false;
bool countdownTemp = false;
bool nodeTimesec = false;

char str_alt[9];
char str_e[9];
char str_hourAP[9];
char str_hourPE[9];
char str_tempAP[9];
char str_hourMI[9];
char str_MNTime[9];
char str_MNDeltaV[9];
char str_tempPE[9];
char str_VOrbit[9];
char unitAP[2];
char unitPE[2];
char unitAlt[2];
char str_countdown[9];

float tempAP, tempPE;
long tempMNTime = 0;

int dispsecAP = 61;
int dispsecMI = 61;
int dispsecMNTime = 61;
int dispsecPE = 61;

String tempAP_string;
String tempPE_string;

unsigned long deadtime, deadtimeOld, controlTime, controlTimeOld, countdownTime;
unsigned long now;

const byte colour_value = 128;
const int slowBlinkRate = 1000; // How frequently to blink slow blinkers
const int fastBlinkRate = 500; // How frequently to blink fast blinkers
bool slowBlinkState = false; // Current state of slow blinking lights
bool fastBlinkState = false; // Current state of fast blinking lights
long lastSlowBlink = 0; // Last time slow blink changed
long lastFastBlink = 0; // Last time fast blnk changed

struct VesselData
{
  byte id;                //1
  float AP;               //2
  float PE;               //3
  float SemiMajorAxis;    //4
  float SemiMinorAxis;    //5
  float VVI;              //6
  float e;                //7
  float inc;              //8
  float G;                //9
  long TAp;               //10
  long TPe;               //11
  float TrueAnomaly;      //12
  float Density;          //13
  long period;            //14
  float RAlt;             //15
  float Alt;              //16
  float Vsurf;            //17
  float Lat;              //18
  float Lon;              //19
  float LiquidFuelTot;    //20
  float LiquidFuel;       //21
  float OxidizerTot;      //22
  float Oxidizer;         //23
  float EChargeTot;       //24
  float ECharge;          //25
  float MonoPropTot;      //26
  float MonoProp;         //27
  float IntakeAirTot;     //28
  float IntakeAir;        //29
  float SolidFuelTot;     //30
  float SolidFuel;        //31
  float XenonGasTot;      //32
  float XenonGas;         //33
  float LiquidFuelTotS;   //34
  float LiquidFuelS;      //35
  float OxidizerTotS;     //36
  float OxidizerS;        //37
  uint32_t MissionTime;   //38
  float deltaTime;        //39
  float VOrbit;           //40
  uint32_t MNTime;        //41
  float MNDeltaV;         //42
  float Pitch;            //43
  float Roll;             //44
  float Heading;          //45
  uint16_t ActionGroups;  //46  status bit order:SAS, RCS, Light, Gear, Brakes, Abort, Custom01 - 10
  byte SOINumber;         //47  SOI Number (decimal format: sun-planet-moon e.g. 130 = kerbin, 131 = mun)
  byte MaxOverHeat;       //48  Max part overheat (% percent)
  float MachNumber;       //49
  float IAS;              //50  Indicated Air Speed
  byte CurrentStage;      //51  Current stage number
  byte TotalStage;        //52  TotalNumber of stages
  float TargetDist;       //53  Distance to targeted vessel (m)
  float TargetV;          //54  Target vessel relative velocity
  byte NavballSASMode;    //55  Combined byte for navball target mode and SAS mode
  // First four bits indicate AutoPilot mode:
  // 0 SAS is off  //1 = Regular Stability Assist //2 = Prograde
  // 3 = RetroGrade //4 = Normal //5 = Antinormal //6 = Radial In
  // 7 = Radial Out //8 = Target //9 = Anti-Target //10 = Maneuver node
  // Last 4 bits set navball mode. (0=ignore,1=ORBIT,2=SURFACE,3=TARGET)
};

struct HandShakePacket
{
  byte id;
  byte M1;
  byte M2;
  byte M3;
};

struct ControlPacket {
  byte id;
  byte MainControls;                  //SAS RCS Lights Gear Brakes Precision Abort Stage
  byte Mode;                          //0 = stage, 1 = docking, 2 = map
  unsigned int ControlGroup;          //control groups 1-10 in 2 bytes
  byte NavballSASMode;                //AutoPilot mode
  byte AdditionalControlByte1;
  int Pitch;                          //-1000 -> 1000
  int Roll;                           //-1000 -> 1000
  int Yaw;                            //-1000 -> 1000
  int TX;                             //-1000 -> 1000
  int TY;                             //-1000 -> 1000
  int TZ;                             //-1000 -> 1000
  int WheelSteer;                     //-1000 -> 1000
  int Throttle;                       //    0 -> 1000
  int WheelThrottle;                  //    0 -> 1000
};

HandShakePacket HPacket;
VesselData VData;
ControlPacket CPacket;

CRGB leds[ANNUNCIATOR0NUM];
CRGB ledsSAS[ANNUNCIATOR1NUM];

LedControl lc1 = LedControl(MAX7219DIN, MAX7219CLK, MAX7219LOAD, MAX7219NUM);
LedControl lc2 = LedControl(MAX7219_2DIN, MAX7219_2CLK, MAX7219_2LOAD, MAX7219_2NUM);
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

static byte kpadState;
const byte rows_ag_sas = 4;
const byte cols_ag_sas = 5;
char keys_ag_sas [rows_ag_sas][cols_ag_sas] = {
  {'0', '1', '2', '3', '4'},
  {'5', '6', '7', '8', '9'},
  {'B', 'C', 'E', 'G', 'I'},
  {'K', 'D', 'F', 'H', 'J'}
};

byte convSASLeds[10] = {9, 8, 3, 7, 2, 6, 1, 5, 0, 4};

#define AG_SAS_ROW1PIN 22
#define AG_SAS_ROW2PIN 23
#define AG_SAS_ROW3PIN 24
#define AG_SAS_ROW4PIN 25

#define AG_SAS_COL1PIN 26
#define AG_SAS_COL2PIN 27
#define AG_SAS_COL3PIN 28
#define AG_SAS_COL4PIN 29
#define AG_SAS_COL5PIN 30

byte rowPins_ag_sas[rows_ag_sas] = {AG_SAS_ROW1PIN, AG_SAS_ROW2PIN, AG_SAS_ROW3PIN, AG_SAS_ROW4PIN} ;
byte colPins_ag_sas[cols_ag_sas] = {AG_SAS_COL1PIN, AG_SAS_COL2PIN, AG_SAS_COL3PIN, AG_SAS_COL4PIN, AG_SAS_COL5PIN};
Keypad keypad_ag_sas = Keypad(makeKeymap(keys_ag_sas), rowPins_ag_sas, colPins_ag_sas, sizeof(rowPins_ag_sas), sizeof(colPins_ag_sas));

void setup() {
  Serial.begin(38400);
  keypad_ag_sas.addEventListener(keypadEvent);
  FastLED.addLeds<NEOPIXEL, ANNUNCIATOR0PIN>(leds, ANNUNCIATOR0NUM);
  FastLED.addLeds<NEOPIXEL, ANNUNCIATOR1PIN>(ledsSAS, ANNUNCIATOR1NUM);
  initLEDS();
  initSEVENsegs();
  initAlpha4();
  initANNUN();
  InitTxPackets();
  controlsInit();
}

void loop() {
  input();
  output();
}

