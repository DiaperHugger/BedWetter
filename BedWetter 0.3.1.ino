/*BedWETTER v0.3.1 Updated 7/1/21
This project is part of a collaborative effort to create a "Bed Wetting Simulator". This device is intended to simulate the experience of bed wetting with
the appropriate hardware. 
https://www.adisc.org/forum/threads/bed-wetting-simulator-updates.151608/ for discussion and help. 
PLEASE READ AND ABIDE BY THE FORUM RULES!! 18+ COMMUNITY!! 
Code at https://github.com/DiaperHugger/BedWetter.

THIS CODE COMES WITH NO IMPLIED WARRANTY!! USE AT YOUR OWN RISK!!
*/
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DS18B20.h>

//FLOW SENSOR
uint8_t FlowPin = 2;
volatile int FlowAmount;   

//TEMP SENSOR PIN
const byte ONEWIRE_PIN = 13;

//Profile variables
float FloodMargin = 0.15;
uint8_t FloodEventsMin = 1;
uint8_t FloodEventsMax = 3;
float HeavyMargin = 0.15;
uint8_t HeavyEventsMin = 3;
uint8_t HeavyEventsMax = 5;
float SpurtsMargin = 0.15;
uint8_t SpurtsEventsMin = 10;
uint8_t SpurtsEventsMax = 20;
float DampMargin = 0.0;
uint8_t DampEventsMin = 5;
uint8_t DampEventsMax = 8;
//FLOW CALIBRATION

uint16_t FlowCal = 1540;

//Shield button value defaults: right 60, up 200, down 400, left 600, select 800
const int right = 60;
const int up = 200;
const int down = 400;
const int left = 600;
const int select = 800;

//RELAY PINS
uint8_t RelayPin = 0; //Output pin for relay. Changes to stored value on power up.
uint8_t HeatRelayPin = 0; //Output pin for secondary relay. Changes to stored value on power up.
uint8_t LCDBackLight = 0; //Manual select the LCD backlight pin. USE AT OWN RISK!
  
uint16_t seconds;
uint32_t CycleTime;  
uint32_t CycleTime2; 
uint32_t CycleTime3; 

uint16_t DutyCycleTime;
uint16_t DutyPulseTime;
uint16_t DutyOffSet = 0;

uint8_t GO = 0;
uint8_t GOHEAT = 0;
uint8_t RandomRun = 0;
uint8_t MakeTimes = 0;
uint8_t WetChance = 1;
uint8_t PauseRun = 0;
uint8_t NewRunTag = 0;
uint8_t DisplayTemp = 1;
uint8_t OutputFeedback = 0;
uint8_t LastOutputFeedback = 0;

uint8_t CurrentVal;
uint8_t LastCurrentVal;
uint8_t CurrentStoredValue;
uint16_t Duration;
uint16_t Interval;
uint8_t ProfileDistribution = 100;

uint16_t NextTriggerEventStart;
uint16_t NextTriggerEventStop;

uint32_t LastTimeMillisA;
uint32_t LastTimeMillisB;

uint8_t CurrentInterval = 0;

uint8_t DiaperBlink = 0;

int ButtonRAW;
int MenuPosition = 0; 
int LastMenuPosition = 1;

int8_t Row = 0;
int8_t Column = 0;

int8_t Cursor = 0;
int8_t LastCursor = 0;
int8_t SPECIAL = 0;

uint8_t LastButtonPress = 0; //1 right, 2 up, 3 down, 4 left, 5 select 
uint8_t ButtonDebounce = 0;

String HomeMessage = "BWv0.3";
int version = 31;


const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

byte sensorAddress[8];
OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);
float temperature;

//Custom LCD characters
byte diaperleft[8] = {

0b01111,
0b11111,
0b00111,
0b11111,
0b01111,
0b01111,
0b00111,
0b00000
};

byte diaperright[8] = {

0b11110,
0b11111,
0b11100,
0b11111,
0b11110,
0b11110,
0b11100,
0b00000
};

byte diaperleft1[8] = {

0b01111,
0b10000,
0b00100,
0b10000,
0b01000,
0b01000,
0b00111,
0b00000
};

byte diaperright1[8] = {

0b11110,
0b00001,
0b00100,
0b00001,
0b00010,
0b00010,
0b11100,
0b00000
};

struct BedWetterDevice //Attributes
{ 
  uint8_t SolenoidState; //0 off, 1 on
  uint8_t HeatState;
  uint8_t Debug;
  uint16_t RunTime;
  uint16_t SolenoidOnTime;
};

BedWetterDevice BedWetter[1] =
{
  {0, 0, 0, 0}
};

struct TIMESVariables
{ 
  uint8_t TotalSleepTime; 
  uint8_t InitialDelay;
};

TIMESVariables TIMESVar[1] =
{
  {0, 0}
};

struct WETTINGVariables
{ 
  uint8_t WetChance; 
  uint8_t Random;
  uint8_t WetInterval;
  uint8_t WetDuration;
};

WETTINGVariables WETTINGVar[1] =
{
  {0, 0, 0, 0}
};

struct PROFILEVariables
{ 
  uint8_t Flood;
  uint8_t FloodDuration;
  uint8_t Heavy;
  uint8_t HeavyDuration;
  uint8_t Spurts;
  uint8_t SpurtsDuration;
  uint8_t Damp;
  uint8_t DampDuration;
  uint8_t Dry;

};

PROFILEVariables PROFILEVar[1] =
{
  {0, 0, 0, 0, 0, 0, 0, 0, 0}
};

struct CONTROLVariables
{ 
  uint8_t Prime; 
  uint8_t BackLight;
  uint8_t OutputPin;
  uint8_t OutputInvert;
  uint8_t DutyCycle;
  uint8_t DutyPulse;
};

CONTROLVariables CONTROLVar[1] =
{
  {0, 0, 0, 0, 0, 0}
};

struct SENSORSVariables
{ 
  uint8_t Temp; 
  uint8_t Flow;
};

SENSORSVariables SENSORSVar[1] =
{
  {0, 0}
};

struct HEATVariables
{ 
  uint8_t HeatRlyPin; 
  uint8_t IdleTemp;
};

HEATVariables HEATVar[1] =
{
  {0, 0}
};
uint16_t INTERVALTimes[50];//Array to store derived intervals.
uint16_t DURATIONTimes[50];//Array to store durations for profile wetting.

String MENUS[] = {"TIMES", "WETTING", "PROFILE", "CONTROL", "SENSORS", "HEAT", "RESET"};

String TIMESItems[] =   {"Total Sleep/h:", "Init Dly/m:"};
String WETTINGItems[] = {"Wet Chance%:", "Random:", "Wet Intval:", "Intval Dur/s:"};
String PROFILEItems[] = {"FLOOD%", "FLOOD DUR/s:", "HEAVY%", "HEAVY DUR/s:", "SPURTS%", "SPURTS DUR/s:", "DAMP%", "DAMP DUR/s:", "DRY%"};
String CONTROLItems[] = {"Prime:", "BackLight:", "PumpPin:", "OutPutInvert:", "Duty Cycle%:", "Duty Pulse:"};
String SENSORSItems[] = {"Temp:", "Flow:"};
String HEATItems []   = {"RlyPin:", "IdleTemp:"};
String RESETItems[] = {"CLEAR!!"};

String RANDOMItems[] = {"   NONE", "   PURE", "PROFILE"};

int8_t MENUSSize = (sizeof(MENUS) / sizeof(MENUS[0]));
int8_t TIMESItemsSize = (sizeof(TIMESItems) / sizeof(TIMESItems[0]));
int8_t WETTINGItemsSize = (sizeof(WETTINGItems) / sizeof(WETTINGItems[0]));
int8_t PROFILEItemsSize = (sizeof(PROFILEItems) / sizeof(PROFILEItems[0]));
int8_t CONTROLItemsSize = (sizeof(CONTROLItems) / sizeof(CONTROLItems[0]));
int8_t SENSORSItemsSize = (sizeof(SENSORSItems) / sizeof(SENSORSItems[0]));
int8_t HEATItemsSize = (sizeof(HEATItems) / sizeof(HEATItems[0]));

int MaxColumnsPerItem[] = {TIMESItemsSize, WETTINGItemsSize, PROFILEItemsSize, CONTROLItemsSize, SENSORSItemsSize, HEATItemsSize};


void EEPROM_RECALL() //EEPROM address layout tied to MenuPosition!  +10 per row.
{
  int a = EEPROM.read(99);
  {
    if(a != version)
    {
      ERASE();
      EEPROM.write(99, version);
      RESET();
    }
  }
  
  TIMESVar[0].TotalSleepTime = EEPROM.read(10);
  TIMESVar[0].InitialDelay = EEPROM.read(11);

  WETTINGVar[0].WetChance = EEPROM.read(20);
  WETTINGVar[0].Random = EEPROM.read(21);
  WETTINGVar[0].WetInterval = EEPROM.read(22);
  WETTINGVar[0].WetDuration = EEPROM.read(23);

  PROFILEVar[0].Flood = EEPROM.read(30);
  PROFILEVar[0].FloodDuration = EEPROM.read(31);
  PROFILEVar[0].Heavy = EEPROM.read(32);
  PROFILEVar[0].HeavyDuration = EEPROM.read(33);
  PROFILEVar[0].Spurts = EEPROM.read(34);
  PROFILEVar[0].SpurtsDuration = EEPROM.read(35);
  PROFILEVar[0].Damp = EEPROM.read(36);
  PROFILEVar[0].DampDuration = EEPROM.read(37);
  PROFILEVar[0].Dry = EEPROM.read(38);

  CONTROLVar[0].OutputPin = EEPROM.read(42);
  CONTROLVar[0].OutputInvert = EEPROM.read(43);
  CONTROLVar[0].DutyCycle = EEPROM.read(44);
  CONTROLVar[0].DutyPulse = EEPROM.read(45);

  SENSORSVar[0].Temp = EEPROM.read(50);
  SENSORSVar[0].Flow = EEPROM.read(51);

  HEATVar[0].HeatRlyPin = EEPROM.read(60);
  HEATVar[0].IdleTemp = EEPROM.read(61);

  RelayPin = CONTROLVar[0].OutputPin;
  HeatRelayPin = HEATVar[0].HeatRlyPin;
}

void setup()
{
  Serial.begin(9600);
  sensors.begin(); //TEMP
  randomSeed(analogRead(1));
  
  if (!onewire.search(sensorAddress)) DisplayTemp = 0;

  pinMode(FlowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(0, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow"  

  
  EEPROM_RECALL();
  pinMode(RelayPin, OUTPUT);
  pinMode(HeatRelayPin, OUTPUT);
  pinMode(LCDBackLight, OUTPUT);
  digitalWrite(LCDBackLight, HIGH); //LCD ON

  pinMode(LED_BUILTIN, OUTPUT); //For testing

  CycleTime = millis(); 
  CycleTime2 = millis();
  CycleTime3 = millis();
  lcd.begin(16, 2);
  lcd.createChar(0, diaperleft);
  lcd.createChar(1, diaperright);
  lcd.createChar(2, diaperleft1);
  lcd.createChar(3, diaperright1);

  if(CONTROLVar[0].OutputInvert == 1)
  {
    digitalWrite(RelayPin, HIGH);
    digitalWrite(HeatRelayPin, HIGH);
  }
  
}

void loop()
{
    sensors.request(sensorAddress);
    temperature = sensors.readTemperature(sensorAddress);
    if(SENSORSVar[0].Temp)
    {
      temperature = (temperature*1.8)+32;
    }
    
    seconds = millis() / 1000; //Get seconds from power on
    DeriveRunningTime();
    if(SENSORSVar[0].Flow)
    {
      interrupts();
    }
    // if ((CycleTime3 + 1000) < millis())
    // {
    //   CycleTime3 = millis(); //reset time mark
    // }

    if ((CycleTime + 100) < millis()) //0.1sec interval
      {
        LCDDisplayData();
        LCDDisplayLiveDate();
        CycleTime = millis(); //reset time mark
      }
        if((Cursor == 1) && (MenuPosition != 0))
        {
            lcd.setCursor(15, 1);
            lcd.blink();
        }
        else
        {
            lcd.setCursor(15, 1);
            lcd.noBlink();
        }
      if(GO == 1)
      {
        EventTrigger();
        OutputFeedbackFUNC();
        RlyInitialization();
      }
      if(GOHEAT)
      {
        TempCheck();
      }
      else
      {
        BedWetter[0].HeatState = 0;
      }
      HeatInitialization();
      ButtonPressCheck();
}
void TempCheck()
{
  if(HEATVar[0].IdleTemp != 0)
  {
    if(temperature < HEATVar[0].IdleTemp)
    {
      BedWetter[0].HeatState = 1;
    }
    else if(temperature > HEATVar[0].IdleTemp)
    {
      BedWetter[0].HeatState = 0;
    }
  }
  else
  {
    BedWetter[0].HeatState = 1;
  }
}
void DeriveRunningTime()
{
  if(GO == 0)
  {
    LastTimeMillisA = millis() - LastTimeMillisB;
  }
  else
  {
    LastTimeMillisB = millis() - LastTimeMillisA;
    BedWetter[0].RunTime = LastTimeMillisB / 1000;
  }
}

void DutyCycle()
{
    int Pulse = CONTROLVar[0].DutyPulse;
    if(Pulse > DURATIONTimes[CurrentInterval - 1])
    {
      Pulse = DURATIONTimes[CurrentInterval - 1];
    }
    DutyCycleTime = ((Pulse * 1000) * ((float)CONTROLVar[0].DutyCycle / 100));
    DutyPulseTime = ((Pulse * 1000) - DutyCycleTime);
    if ((CycleTime2 + DutyOffSet) < millis()) 
        {
          if(BedWetter[0].SolenoidState == 0)
          {
            BedWetter[0].SolenoidState = 1;
            DutyOffSet = DutyCycleTime;
          }
          else
          {
            BedWetter[0].SolenoidState = 0;
            DutyOffSet = DutyPulseTime;
          }
          CycleTime2 = millis();
        }
}


void EventTrigger()
{
  if(CurrentInterval != 0)
  {
    if ((BedWetter[0].RunTime > NextTriggerEventStart) && (BedWetter[0].RunTime < NextTriggerEventStop))
    {
      if((CONTROLVar[0].DutyCycle != 0) && (CONTROLVar[0].DutyPulse != 0))
      {
        DutyCycle();
      }
      else
      {
        BedWetter[0].SolenoidState = 1;
      }
    }
    else if (BedWetter[0].RunTime > NextTriggerEventStop)
    {
      CurrentInterval--;
      BedWetter[0].SolenoidState = 0;
      RlyInitialization();
      Events(); // Load next round
    }
  }
  else
  {
    BedWetter[0].SolenoidState = 0;
    RlyInitialization();
  }
  if((TIMESVar[0].TotalSleepTime * 3600) < BedWetter[0].RunTime)//Stop running after sleep time
  {
    RESET();
  }
  if(NextTriggerEventStart > NextTriggerEventStop)
  {
    RESET();
  }
}

uint16_t sort_desc(const void *cmp1, const void *cmp2)//Sort function
{
  uint16_t a = *((uint16_t *)cmp1);
  uint16_t b = *((uint16_t *)cmp2);
  return a > b ? -1 : (a < b ? 1 : 0);
}
void Events() //Triggered by select button and end of EventTrigger stop time
{
    
    Interval = WETTINGVar[0].WetInterval;
    Duration = WETTINGVar[0].WetDuration;
    
    
    if(MakeTimes == 1)//Derive interval times
    {
      if(WETTINGVar[0].Random == 0)//None random interval times
      {
        Serial.println(F("EVEN"));
        for (int i = (Interval); i >= 1; i--)
        {
           uint16_t T = ((TIMESVar[0].TotalSleepTime * 3600) / Interval);
            T = (T * i) - Duration;
            if(TIMESVar[0].InitialDelay != 0)
            {
              T = (T + (TIMESVar[0].InitialDelay * 60)); //Minuets
            }
            INTERVALTimes[Interval - i] = T;
        }
        for (int i = (Interval - 1); i >= 0; i--)
        {
          DURATIONTimes[i] = WETTINGVar[0].WetDuration;
        } 
        INTERVALTimes[Interval - 1] = TIMESVar[0].InitialDelay * 60;
      }
      else if(WETTINGVar[0].Random == 1)//Pure random interval times
      {
        Serial.println(F("PURE RANDOM"));
        for (int i = (Interval - 1); i >= 0; i--)
        {
          uint16_t TEMPTIME = TIMESVar[0].TotalSleepTime;
          TEMPTIME = TEMPTIME * 3600;
          uint16_t r = random((TIMESVar[0].InitialDelay * 60), TEMPTIME);
          //r = r - Duration;
          INTERVALTimes[i] = r;
          int d = random(257, WETTINGVar[0].WetDuration * 256) / 256;
          DURATIONTimes[i] = d;
        } 
      }
      else if(WETTINGVar[0].Random == 2)//Profile random
      {
        uint8_t ProfileRotation = random(1, 100);
        if(ProfileRotation <= PROFILEVar[0].Flood)
        {
          Serial.println(F("FLOOD"));
          FLOOD();
        }
        else if((ProfileRotation > PROFILEVar[0].Flood) && (ProfileRotation <= (PROFILEVar[0].Heavy + PROFILEVar[0].Flood)))
        {
          Serial.println(F("HEAVY"));
          HEAVY();
        }
        else if((ProfileRotation > (PROFILEVar[0].Heavy + PROFILEVar[0].Flood)) && (ProfileRotation <= ((PROFILEVar[0].Heavy + PROFILEVar[0].Flood) + PROFILEVar[0].Spurts)))
        {
          Serial.println(F("SPURTS"));
          SPURTS();
        }
        else if((ProfileRotation > ((PROFILEVar[0].Heavy + PROFILEVar[0].Flood) + PROFILEVar[0].Spurts)) && (ProfileRotation <= (((PROFILEVar[0].Heavy + PROFILEVar[0].Flood) + PROFILEVar[0].Spurts) + PROFILEVar[0].Damp)))
        {
          Serial.println(F("DAMP"));
          DAMP();
        }
        else if(ProfileRotation > (((PROFILEVar[0].Heavy + PROFILEVar[0].Flood) + PROFILEVar[0].Spurts) + PROFILEVar[0].Damp))
        {
          Serial.println(F("DRY"));
          Interval = 0;
          WetChance = 0;
        }
        
        //Add inital delay?
      }
      
      uint16_t INTERVALTimes_length = sizeof(INTERVALTimes) / sizeof(INTERVALTimes[0]);
      qsort(INTERVALTimes, INTERVALTimes_length, sizeof(INTERVALTimes[0]), sort_desc);
      uint16_t DURATIONTimes_length = sizeof(INTERVALTimes) / sizeof(DURATIONTimes[0]);
      qsort(INTERVALTimes, INTERVALTimes_length, sizeof(DURATIONTimes[0]), sort_desc);
      CurrentInterval = Interval;
      
      for (int i = 1; i <= Interval; i++)//Print intervals and duations
      {
        char buff[10];
        Serial.print("I:");
        sprintf(buff, "%2d, ", i);
        Serial.print(buff);
        Serial.print("T:");
        sprintf(buff, "%5u, ", INTERVALTimes[Interval - i]);
        Serial.print(buff);
        Serial.print("D:");
        sprintf(buff, "%2d", DURATIONTimes[Interval - i]);
        Serial.println(buff);
      }
      Serial.println("--------------------");

      if(WETTINGVar[0].WetChance != 0)
      {
        int PercentChance = random(0, 99);
        if(PercentChance <= WETTINGVar[0].WetChance)
        {
          WetChance = 1;
        }
        else
        {
          WetChance = 0;
        }
      }
      MakeTimes = 0;
    }
    
    if(WetChance == 1)
    {
      if(TIMESVar[0].TotalSleepTime != 0)
      {
        NextTriggerEventStart = (INTERVALTimes[CurrentInterval - 1]);
        NextTriggerEventStop = (NextTriggerEventStart + DURATIONTimes[CurrentInterval - 1]);
      }
    }
}

void RlyInitialization()
{
  if(BedWetter[0].SolenoidState == 1)
  {
    RlyON();
  }
  else if(BedWetter[0].SolenoidState == 0)
  {
    RlyOFF();
  }
}

void HeatInitialization()
{
  if(BedWetter[0].HeatState == 1)
  {
    HeatRlyON();
  }
  else if(BedWetter[0].HeatState == 0)
  {
    HeatRlyOFF();
  }
}

void RlyOFF()
{
  digitalWrite(LED_BUILTIN, LOW);//debug
  if(CONTROLVar[0].OutputInvert == 1)
  {
    digitalWrite(RelayPin, HIGH);
  }
  else
  {
    digitalWrite(RelayPin, LOW);
  }
  OutputFeedback = 1;
}
void RlyON()
{
  digitalWrite(LED_BUILTIN, HIGH);//debug
  if(CONTROLVar[0].OutputInvert == 1)
  {
    digitalWrite(RelayPin, LOW);
  }
  else
  {
    digitalWrite(RelayPin, HIGH);
  }
  OutputFeedback = 2;
}

void OutputFeedbackFUNC()
{
  if((OutputFeedback == 1) && (LastOutputFeedback != OutputFeedback))
  {
    char buff[10];
    sprintf(buff, "%5d, ", BedWetter[0].RunTime);
    Serial.print(F("TIME:"));
    Serial.print(buff);
    Serial.println(F("OFF"));
    LastOutputFeedback = 1;
  }
  else if((OutputFeedback == 2) && (LastOutputFeedback != OutputFeedback))
  {
    char buff[10];
    sprintf(buff, "%5d, ", BedWetter[0].RunTime);
    Serial.print(F("TIME:"));
    Serial.print(buff);
    Serial.println(F("ON"));
    LastOutputFeedback = 2;
  }
}

void HeatRlyOFF()
{
  if(CONTROLVar[0].OutputInvert == 1)
  {
    digitalWrite(HeatRelayPin, HIGH);
  }
  else
  {
    digitalWrite(HeatRelayPin, LOW);
  }
}
void HeatRlyON()
{
  if(CONTROLVar[0].OutputInvert == 1)
  {
    digitalWrite(HeatRelayPin, LOW);
  }
  else
  {
    digitalWrite(HeatRelayPin, HIGH);
  }
}

void LCDDisplayData() //MenuPosition = ROW,COLUMN
{
    if(MenuPosition == 0)
      {
        if(SENSORSVar[0].Flow)
        {
          lcd.setCursor(11, 0);
          float l = float(FlowAmount) / FlowCal;
          lcd.print(l, 2);
          lcd.setCursor(15, 0);
          lcd.print("L");
        }
        lcd.setCursor(0, 1);
        lcd.print("R:");
        lcd.setCursor(2, 1);
        lcd.print(BedWetter[0].RunTime);
        
        if(NewRunTag == 0)
        {
          lcd.setCursor(3, 1);
          lcd.write(126);
          lcd.setCursor(4, 1);
          lcd.print("NewRun");
        }
        else if(NewRunTag == 1)
        {
          lcd.setCursor(3, 1);
          lcd.print("       ");
          NewRunTag = 2;
        }

        if(DisplayTemp)
        {
          if(temperature > 99.9)
          {
            lcd.setCursor(10, 1);
            lcd.print(temperature, 1);
          }
          else
          {
            lcd.setCursor(10, 1);
            lcd.print(" ");
            lcd.setCursor(11, 1);
            lcd.print(temperature, 1);
          }
          lcd.setCursor(15, 1);
          if(SENSORSVar[0].Temp)
          {
            lcd.print("F");
          }
          else
          {
            lcd.print("C");
          }
        }
        if(GOHEAT)
        {
          lcd.setCursor(9, 0);
          lcd.print(F("h"));
          if(BedWetter[0].HeatState)
          {
            lcd.setCursor(9, 0);
            lcd.print(F("H"));
          }
          else
          {
            lcd.setCursor(9, 0);
            lcd.print(F("h"));
          }
        }
        else
        {
          lcd.setCursor(9, 0);
          lcd.print(F(" "));
        }
        
        
      }
    
    if((MenuPosition == 0) && (GO == 1))//Diaper animation
    {
      if(DiaperBlink == 1)
      {
        lcd.setCursor(0, 0);
        lcd.write(byte(0));
        lcd.setCursor(1, 0);
        lcd.write(byte(1));
        DiaperBlink = 0;
      }
      else
      {
        lcd.setCursor(0, 0);
        lcd.write(byte(2));
        lcd.setCursor(1, 0);
        lcd.write(byte(3));
        DiaperBlink = 1;
      }
    }
  
    if ((MenuPosition == 0) && (MenuPosition != LastMenuPosition)) //HOME
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      lcd.setCursor(1, 0);
      lcd.write(byte(1));
      lcd.setCursor(2, 0);
      lcd.print(HomeMessage);
      MenuPosition = 0;
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 10) && (MenuPosition != LastMenuPosition)) //TIMES Total Sleep
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(TIMESItems[Column]);
      lcd.setCursor(15, 1);
      if(TIMESVar[0].TotalSleepTime > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(TIMESVar[0].TotalSleepTime);
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 11) && (MenuPosition != LastMenuPosition)) //TIMES Initial Dly
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(TIMESItems[Column]);
      lcd.setCursor(15, 1);
      if(TIMESVar[0].InitialDelay > 99)
      {
        lcd.setCursor(13, 1);
      }
      else if(TIMESVar[0].InitialDelay > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(TIMESVar[0].InitialDelay);
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 20) && (MenuPosition != LastMenuPosition)) //WETTING Chance FULL Wet:
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(WETTINGItems[Column]);
      lcd.setCursor(15, 1);
      if(WETTINGVar[0].WetChance > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(WETTINGVar[0].WetChance);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 21) && (MenuPosition != LastMenuPosition)) //WETTING Random:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(WETTINGItems[Column]);
      if(WETTINGVar[0].Random == 0)
      {
        lcd.setCursor(9, 1);
        lcd.print(RANDOMItems[0]);
      }
      else if(WETTINGVar[0].Random == 1)
      {
        lcd.setCursor(9, 1);
        lcd.print(RANDOMItems[1]);
      }
      else if(WETTINGVar[0].Random == 2)
      {
        lcd.setCursor(9, 1);
        lcd.print(RANDOMItems[2]);
      }
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 22) && (MenuPosition != LastMenuPosition)) //WETTING Wet Intervals:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(WETTINGItems[Column]);
      lcd.setCursor(15, 1);
      if(WETTINGVar[0].WetInterval > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(WETTINGVar[0].WetInterval);
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 23) && (MenuPosition != LastMenuPosition)) //WETTING Intval Dur:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(WETTINGItems[Column]);
      lcd.setCursor(15, 1);
      if(WETTINGVar[0].WetDuration > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(WETTINGVar[0].WetDuration);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 30) && (MenuPosition != LastMenuPosition)) //Profile Flood
    {
      ProfileDistributionCheck();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].Flood > 9)
      {
        lcd.setCursor(14, 1);
      }
      if(PROFILEVar[0].Flood > 99)
      {
        lcd.setCursor(13, 1);
      }
      lcd.print(PROFILEVar[0].Flood);
      CurrentStoredValue = PROFILEVar[0].Flood;
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 31) && (MenuPosition != LastMenuPosition)) //Profile FloodDuration
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].FloodDuration > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(PROFILEVar[0].FloodDuration);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 32) && (MenuPosition != LastMenuPosition)) //Profile Heavy
    {
      ProfileDistributionCheck();
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].Heavy > 9)
      {
        lcd.setCursor(14, 1);
      }
      if(PROFILEVar[0].Heavy > 99)
      {
        lcd.setCursor(13, 1);
      }
      lcd.print(PROFILEVar[0].Heavy);
      CurrentStoredValue = PROFILEVar[0].Heavy;
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 33) && (MenuPosition != LastMenuPosition)) //Profile HeavyDuration
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].HeavyDuration > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(PROFILEVar[0].HeavyDuration);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 34) && (MenuPosition != LastMenuPosition)) //Profile Spurts
    {
      ProfileDistributionCheck();
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].Spurts > 9)
      {
        lcd.setCursor(14, 1);
      }
      if(PROFILEVar[0].Spurts > 99)
      {
        lcd.setCursor(13, 1);
      }
      lcd.print(PROFILEVar[0].Spurts);
      CurrentStoredValue = PROFILEVar[0].Spurts;
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 35) && (MenuPosition != LastMenuPosition)) //Profile SpurtsDuration
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].SpurtsDuration > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(PROFILEVar[0].SpurtsDuration);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 36) && (MenuPosition != LastMenuPosition)) //Profile Damp
    {
      ProfileDistributionCheck();
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].Damp > 9)
      {
        lcd.setCursor(14, 1);
      }
      if(PROFILEVar[0].Damp > 99)
      {
        lcd.setCursor(13, 1);
      }
      lcd.print(PROFILEVar[0].Damp);
      CurrentStoredValue = PROFILEVar[0].Damp;
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 37) && (MenuPosition != LastMenuPosition)) //Profile DampDuration
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].DampDuration > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(PROFILEVar[0].DampDuration);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 38) && (MenuPosition != LastMenuPosition)) //Profile Dry
    {
      ProfileDistributionCheck();
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(PROFILEItems[Column]);
      lcd.setCursor(15, 1);
      if(PROFILEVar[0].Dry > 9)
      {
        lcd.setCursor(14, 1);
      }
      if(PROFILEVar[0].Dry > 99)
      {
        lcd.setCursor(13, 1);
      }
      lcd.print(PROFILEVar[0].Dry);
      CurrentStoredValue = PROFILEVar[0].Dry;
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 40) && (MenuPosition != LastMenuPosition)) //CONTROL Prime:
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(13, 1);
      lcd.print("OFF");
      CONTROLVar[0].Prime = 0;
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 41) && (MenuPosition != LastMenuPosition)) //CONTROL BackLight:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(13, 1);
      lcd.print(" ON");
      CONTROLVar[0].BackLight = 1;
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 42) && (MenuPosition != LastMenuPosition)) //CONTROL OutPut Pin:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(15, 1);
      if(CONTROLVar[0].OutputPin > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(CONTROLVar[0].OutputPin);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 43) && (MenuPosition != LastMenuPosition)) //CONTROL OutputInvert:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      if(CONTROLVar[0].OutputInvert == 0)
      {
        lcd.setCursor(13, 1);
        lcd.print("OFF");
      }
      else
      {
        lcd.setCursor(13, 1);
        lcd.print(" ON");
      }
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 44) && (MenuPosition != LastMenuPosition)) //CONTROL Duty Cycle:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(15, 1);
      if(CONTROLVar[0].DutyCycle > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(CONTROLVar[0].DutyCycle);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 45) && (MenuPosition != LastMenuPosition)) //CONTROL Duty Pulse:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(15, 1);
      if(CONTROLVar[0].DutyPulse > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(CONTROLVar[0].DutyPulse);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 50) && (MenuPosition != LastMenuPosition))
    {
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(SENSORSItems[Column]);
      char buffer [10];
      sprintf (buffer, "..%02X%02X%02X", sensorAddress[5],sensorAddress[6],sensorAddress[7]);
      lcd.setCursor(8, 0);
      lcd.print(buffer);
      lcd.setCursor(11, 1);
      if(temperature >= 100)
      {
        lcd.setCursor(10, 1);
      }
      lcd.print(temperature, 1);
      lcd.setCursor(15, 1);
      if(SENSORSVar[0].Temp == 0)
      {
        lcd.print("C");
      }
      else
      {
        lcd.print("F");
      }
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 51) && (MenuPosition != LastMenuPosition))//SENSORS Flow
    {
      lcd.setCursor(0, 0);
      lcd.print(F("                "));
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(SENSORSItems[Column]);
      lcd.setCursor(11, 1);
      if(SENSORSVar[0].Flow == 2)
      {
        RlyOFF();
        SENSORSVar[0].Flow = 1;
      }
      if(SENSORSVar[0].Flow == 0)
      {
        lcd.print(F("  OFF"));
      }
      else
      {
        lcd.setCursor(11, 1);
        lcd.print(F(" SHOW"));
      }
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 60) && (MenuPosition != LastMenuPosition)) //Heat RlyPin:
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(HEATItems[Column]);
      lcd.setCursor(15, 1);
      if(HEATVar[0].HeatRlyPin > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(HEATVar[0].HeatRlyPin);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 61) && (MenuPosition != LastMenuPosition)) //Heat IdleTemp:
    {
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      lcd.print(HEATItems[Column]);
      float C = HEATVar[0].IdleTemp;
      float F = (C*1.8)+32;
      lcd.setCursor(15, 0);
      lcd.print("F");
      lcd.setCursor(14, 0);
      if(F > 99)
      {
      lcd.setCursor(12, 0);
      }
      else if(F > 9)
      {
      lcd.setCursor(13, 0);
      }
      lcd.print(F, 0);
      lcd.setCursor(15, 1);
      lcd.print("C");
      lcd.setCursor(14, 1);
      if(C > 99)
      {
      lcd.setCursor(12, 1);
      }
      else if(C > 9)
      {
      lcd.setCursor(13, 1);
      }
      lcd.print(C, 0);
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 70) && (MenuPosition != LastMenuPosition)) //RESET CLEAR!!:
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(RESETItems[Column]);
      Cursor = 0;
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
}

void LCDDisplayLiveDate()
{
  if(MenuPosition == 50)//SENSORS Temp
  {
    if(!DisplayTemp)
    {
      lcd.setCursor(12, 1);
      lcd.print(F("NONE"));
    }
    char buffer [8];
    sprintf (buffer, "..%02X%02X%02X", sensorAddress[5],sensorAddress[6],sensorAddress[7]);
    lcd.setCursor(8, 0);
    lcd.print(buffer);
    lcd.setCursor(11, 1);
    if(temperature >= 100)
    {
      lcd.setCursor(10, 1);
    }
    lcd.print(temperature, 1);
    lcd.setCursor(15, 1);
    if(SENSORSVar[0].Temp == 0)
    {
      lcd.print("C");
    }
    else
    {
      lcd.print("F");
    }
  }
  else if(MenuPosition == 51)//SENSORS Flow
  {
    lcd.setCursor(15, 0);
    if(FlowAmount > 9999)
    {
      lcd.setCursor(11, 0);
    }
    else if(FlowAmount > 999)
    {
      lcd.setCursor(12, 0);
    }
    else if(FlowAmount > 99)
    {
      lcd.setCursor(13, 0);
    }
    else if(FlowAmount > 9)
    {
      lcd.setCursor(14, 0);
    }
    lcd.print(FlowAmount);
  }
}

void ButtonPressCheck()
{
  ButtonRAW = analogRead (0);
  if(ButtonRAW == 1023)
  {
    ButtonDebounce = 0;
  }
  if(ButtonDebounce == 0)
  {
      if (ButtonRAW < right) //RIGHT
        {
          if(Cursor != 1)
          {
            Column = Column + 1;
            if(Column >= MaxColumnsPerItem[Row -1])
            {
                Column = (MaxColumnsPerItem[Row -1] -1);
            }
          }
          if(MenuPosition == 0)
          {
            FlowAmount = 0;
            RESET();
          }
          ButtonDebounce = 1;
          LastButtonPress = 1;
        }
        else if (ButtonRAW < up) //UP
        {
          if(Cursor == 1)
          {
            if(SPECIAL == 1)//Check for special condition
            {
              LastButtonPress = 2;
              ALTERNATE();
            }
            else
            {
              CurrentVal++;
              if(CurrentVal > 99)
              {
                CurrentVal = 0;
                lcd.setCursor(14, 1);
                lcd.print("  ");
              }
              lcd.setCursor(15, 1);
              if(CurrentVal > 9)
              {
                lcd.setCursor(14, 1);
              }
              lcd.print(CurrentVal);
            }
          }
          else
          {
            Row = Row + 1;
            if(Row > MENUSSize)
            {
              Row = MENUSSize;
            }
            Column = 0;
          }
          ButtonDebounce = 1;
          
        }
        else if (ButtonRAW < down) //DOWN
        {
          if(Cursor == 1)
          {
            if(SPECIAL == 1)//Check for special condition
            {
              LastButtonPress = 3;
              ALTERNATE();
            }
            else
            {
              if(CurrentVal == 0)
              {
                CurrentVal = 99;
              }
              else
              {
                CurrentVal--;
              }
              lcd.setCursor(14, 1);
              lcd.print(" ");
              lcd.setCursor(15, 1);
              if(CurrentVal > 9)
              {
                lcd.setCursor(14, 1);
              }
              lcd.print(CurrentVal);
            }
          }
          else
          {
            Row = Row - 1;
            if(Row < 0)
            {
              Row = 0;
            }
            Column = 0;
          }
          ButtonDebounce = 1;
        }
        else if (ButtonRAW < left) //LEFT
        {
          if(Cursor == 1)
          {
            
          }
          else
          {
            Column = Column - 1;
            if(Column < 0)
            {
              Column = 0;
            }
          }
          if(MenuPosition == 0)
          {
            if(GOHEAT == 0)
            {
               GOHEAT = 1;
            }
            else
            {
               GOHEAT = 0;
            }
          }
          ButtonDebounce = 1;
          LastButtonPress = 4;
        }
        else if (ButtonRAW < select) //SELECT
        {
          if(MenuPosition == 70)
          {
            ERASE();
            RESET();
          }
          
          if((Cursor == 0) && (MenuPosition != 0))
          {
            Cursor = 1;
            CurrentVal = EEPROM.read(MenuPosition);
          }
          else
          {
            CHECKLIMITS();
            EEPROM.write(MenuPosition, CurrentVal);
            EEPROM_RECALL();
            Cursor = 0;
          }
          
          if((MenuPosition == 0) && (GO != 1))
          {
            if(NextTriggerEventStart == 0)
            {
              MakeTimes = 1;
              NewRunTag = 1;
              Events();
            }
            GO = 1;
            RandomRun = 1;
            if(PauseRun == 1)
            {
              PauseRun = 0;
            }
          }
          else
          {
            if(PauseRun == 0)
            {
              BedWetter[0].SolenoidState = 0;
              RlyInitialization();
              PauseRun = 1;
            }
            GO = 0;
          }
          ButtonDebounce = 1;
          LastButtonPress = 5;
        }
  }
  MenuPosition = ((Row * 10) + Column); //Encode MenuPosition
}
void CHECKLIMITS()
{
  // if(MenuPosition == 45)//Restrict max duty pulse
  // {
  //   if(CurrentVal >= WETTINGVar[0].WetDuration)
  //   {
  //     CurrentVal = WETTINGVar[0].WetDuration;
  //   }
  // }
}
void ALTERNATE()
{
  if(MenuPosition == 10)//Total Sleep Time
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > 12)
        {
          CurrentVal = 0;
          lcd.setCursor(14, 1);
          lcd.print("  ");
        }
        lcd.setCursor(15, 1);
        if(CurrentVal > 9)
        {
          lcd.setCursor(14, 1);
        }
        lcd.print(CurrentVal);
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal == 0)
        {
          CurrentVal = 12;
        }
        else
        {
          CurrentVal--;
        }
        lcd.setCursor(14, 1);
        lcd.print(" ");
        lcd.setCursor(15, 1);
        if(CurrentVal > 9)
        {
         lcd.setCursor(14, 1);
        }
        lcd.print(CurrentVal);
      }
      TIMESVar[0].TotalSleepTime = CurrentVal;
  }
  if(MenuPosition == 11)//Initial delay
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal = CurrentVal + 15;
        if(CurrentVal > 240)
        {
          CurrentVal = 0;
          lcd.setCursor(13, 1);
          lcd.print("  ");
        }
        lcd.setCursor(15, 1);
        if(CurrentVal > 99)
        {
          lcd.setCursor(13, 1);
        }
        else if(CurrentVal > 9)
        {
          lcd.setCursor(14, 1);
        }
        lcd.print(CurrentVal);
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal == 0)
        {
          CurrentVal = 240;
        }
        else
        {
          CurrentVal = CurrentVal - 15;
        }
        lcd.setCursor(13, 1);
        lcd.print("  ");
        lcd.setCursor(15, 1);
        if(CurrentVal > 99)
        {
         lcd.setCursor(13, 1);
        }
        else if(CurrentVal > 9)
        {
         lcd.setCursor(14, 1);
        }
        lcd.print(CurrentVal);
      }
      TIMESVar[0].InitialDelay = CurrentVal;
  }
  if(MenuPosition == 21)//Random
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > 2)
        {
          CurrentVal = 2;
        }
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal != 0)
        {
          CurrentVal--;
        }
      }
      if(CurrentVal == 0)
      {
        lcd.setCursor(9, 1);
        lcd.print(RANDOMItems[0]);
      }
      else if(CurrentVal == 1)
      {
        lcd.setCursor(9, 1);
        lcd.print(RANDOMItems[1]);
      }
      else if(CurrentVal == 2)
      {
        lcd.setCursor(9, 1);
        lcd.print(RANDOMItems[2]);
      }
      WETTINGVar[0].Random = CurrentVal;
  }
  
  if(MenuPosition == 22)//Wet Intefvals
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > 49)
        {
          CurrentVal = 0;
          lcd.setCursor(14, 1);
          lcd.print("  ");
        }
        lcd.setCursor(15, 1);
        if(CurrentVal > 9)
        {
          lcd.setCursor(14, 1);
        }
        lcd.print(CurrentVal);
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal == 0)
        {
          CurrentVal = 49;
        }
        else
        {
          CurrentVal--;
        }
        lcd.setCursor(14, 1);
        lcd.print(" ");
        lcd.setCursor(15, 1);
        if(CurrentVal > 9)
        {
         lcd.setCursor(14, 1);
        }
        lcd.print(CurrentVal);
      }
      WETTINGVar[0].WetInterval = CurrentVal;
  }
  if(MenuPosition == 30)//Profile Flood
  {
      ProfileDistributionLimit();
      PROFILEVar[0].Flood = CurrentVal;
  }
  if(MenuPosition == 32)//Profile Heavy
  {
      ProfileDistributionLimit();
      PROFILEVar[0].Heavy = CurrentVal;
  }
  if(MenuPosition == 34)//Profile Spurts
  {
      ProfileDistributionLimit();
      PROFILEVar[0].Spurts = CurrentVal;
  }
  if(MenuPosition == 36)//Profile Damp
  {
      ProfileDistributionLimit();
      PROFILEVar[0].Damp = CurrentVal;
  }
  if(MenuPosition == 38)//Profile Dry
  {
      ProfileDistributionLimit();
      PROFILEVar[0].Dry = CurrentVal;
  }
  if(MenuPosition == 40)//PRIME
  {
    if(CONTROLVar[0].Prime == 0)
    {
      lcd.setCursor(13, 1);
      lcd.print(" ON");
      RlyON();
      CONTROLVar[0].Prime = 1;
    }
    else
    {
      lcd.setCursor(13, 1);
      lcd.print("OFF");
      RlyOFF();
      CONTROLVar[0].Prime = 0;
    }
    ButtonDebounce = 1;
  }
  if(MenuPosition == 41)//BackLight
  {
    if(CONTROLVar[0].BackLight == 0)
    {
      lcd.setCursor(13, 1);
      lcd.print(" ON");
      digitalWrite(10, HIGH);
      CONTROLVar[0].BackLight = 1;
    }
    else
    {
      lcd.setCursor(13, 1);
      lcd.print("OFF");
      digitalWrite(10, LOW);
      CONTROLVar[0].BackLight = 0;
    }
    ButtonDebounce = 1;
  }
  if(MenuPosition == 43)//OutputInvert
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > 1)
        {
          CurrentVal = 1;
        }
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal != 0)
        {
          CurrentVal--;
        }
      }
      if(CurrentVal == 0)
      {
        lcd.setCursor(13, 1);
        lcd.print("OFF");
        CONTROLVar[0].OutputInvert = 0;
      }
      else if(CurrentVal == 1)
      {
        lcd.setCursor(13, 1);
        lcd.print(" ON");
        CONTROLVar[0].OutputInvert = 1;
      }
   }
   if(MenuPosition == 50)//SENSORS Temp
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > 1)
        {
          CurrentVal = 1;
        }
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal != 0)
        {
          CurrentVal--;
        }
      }
      if(CurrentVal == 0)
      {
        lcd.setCursor(15, 1);
        lcd.print("C");
        SENSORSVar[0].Temp = 0;
      }
      else if(CurrentVal == 1)
      {
        lcd.setCursor(15, 1);
        lcd.print("F");
        SENSORSVar[0].Temp = 1;
      }
   }
  if(MenuPosition == 51)//SENSORS Flow
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > 2)
        {
          CurrentVal = 2;
        }
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal != 0)
        {
          CurrentVal--;
        }
      }
      if(CurrentVal == 0)
      {
        lcd.setCursor(11, 1);
        lcd.print(F("  OFF"));
        SENSORSVar[0].Flow = 0;
        RlyOFF();

      }
      else if(CurrentVal == 1)
      {
        lcd.setCursor(11, 1);
        lcd.print(F(" SHOW"));
        SENSORSVar[0].Flow = 1;
        RlyOFF();
      }
      else if(CurrentVal == 2)
      {
        lcd.setCursor(11, 1);
        lcd.print(F("PRIME"));
        SENSORSVar[0].Flow = 2;
        RlyON();
      }
   }
   if(MenuPosition == 61)//HEAT IdleTemp
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > 120)
        {
          CurrentVal = 0;
          lcd.setCursor(12, 1);
          lcd.print("  ");
          lcd.setCursor(12, 0);
          lcd.print("  ");
        }
        lcd.setCursor(14, 1);
        if(CurrentVal > 99)
        {
          lcd.setCursor(12, 1);
        }
        else if(CurrentVal > 9)
        {
          lcd.setCursor(13, 1);
        }
        int F = (CurrentVal*1.8)+32;
        lcd.print(CurrentVal);
        lcd.setCursor(14, 0);
        if(F > 99)
        {
          lcd.setCursor(12, 0);
        }
        else if(F > 9)
        {
          lcd.setCursor(13, 0);
        }
        lcd.print(F);
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal == 0)
        {
          CurrentVal = 120;
        }
        else
        {
          CurrentVal--;
        }
        lcd.setCursor(12, 1);
        lcd.print(F("  "));
        lcd.setCursor(12, 0);
        lcd.print(F("  "));
        lcd.setCursor(14, 1);
        if(CurrentVal > 99)
        {
         lcd.setCursor(12, 1);
        }
        else if(CurrentVal > 9)
        {
         lcd.setCursor(13, 1);
        }
        int F = (CurrentVal*1.8)+32;
        lcd.print(CurrentVal);
        lcd.setCursor(14, 0);
        if(F > 99)
        {
          lcd.setCursor(12, 0);
        }
        else if(F > 9)
        {
          lcd.setCursor(13, 0);
        }
        lcd.print(F);
      }
      HEATVar[0].IdleTemp = CurrentVal;
  }
}

void ProfileDistributionCheck()
{
  ProfileDistribution = (100 - ((((PROFILEVar[0].Flood + PROFILEVar[0].Heavy) + PROFILEVar[0].Spurts) + PROFILEVar[0].Damp) + PROFILEVar[0].Dry));
  if(ProfileDistribution > 100)
  {
    ProfileDistribution = 100;
  }
  else if(ProfileDistribution <= 0)
  {
    ProfileDistribution = 0;
  }
}
void ProfileDistributionLimit()
{
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > (ProfileDistribution + CurrentStoredValue))
        {
          CurrentVal = 0;
          lcd.setCursor(13, 1);
          lcd.print("  ");
        }
        lcd.setCursor(15, 1);
        if(CurrentVal > 9)
        {
          lcd.setCursor(14, 1);
        }
        if(CurrentVal > 99)
        {
          lcd.setCursor(13, 1);
        }
        lcd.print(CurrentVal);
      }
    else if(LastButtonPress == 3)//DOWN
      {
        if(CurrentVal == 0)
        {
          CurrentVal = (ProfileDistribution + CurrentStoredValue);
        }
        else
        {
          CurrentVal--;
        }
        lcd.setCursor(13, 1);
        lcd.print("  ");
        lcd.setCursor(15, 1);
        if(CurrentVal > 9)
        {
         lcd.setCursor(14, 1);
        }
        if(CurrentVal > 99)
        {
         lcd.setCursor(13, 1);
        }
        lcd.print(CurrentVal);
      }
      // Serial.print(CurrentVal);
      // Serial.println(" , ");
      // Serial.print(ProfileDistribution);
      // Serial.println(" , ");
      // Serial.print(CurrentStoredValue);
      // Serial.println(" , ");

  }


void FLOOD()
{
  uint16_t ProfileIntervals = (random((FloodEventsMin * 1000), ((FloodEventsMax * 1000) + 999)) / 1000);//Find number of weetings
  Interval = ProfileIntervals;
  for (int i = (ProfileIntervals - 1); i >= 0; i--)
  {
    float FloodLow = (PROFILEVar[0].FloodDuration - (PROFILEVar[0].FloodDuration * FloodMargin));
    float FloodHigh = (PROFILEVar[0].FloodDuration + (PROFILEVar[0].FloodDuration * FloodMargin));
    uint16_t r = random(1, (FloodMargin * 200));
    uint16_t d = map(r, 1, (FloodMargin * 200), FloodLow, FloodHigh);
    DURATIONTimes[i] = d;
    uint16_t t = random((TIMESVar[0].InitialDelay * 60), TIMESVar[0].TotalSleepTime * 3600);
    INTERVALTimes[i] = t;
  } 
}
void HEAVY()
{
  uint16_t ProfileIntervals = (random((HeavyEventsMin * 1000), ((HeavyEventsMax * 1000) + 999)) / 1000);//Find number of weetings
  Interval = ProfileIntervals;
  for (int i = (ProfileIntervals - 1); i >= 0; i--)
  {
    float HeavyLow = (PROFILEVar[0].HeavyDuration - (PROFILEVar[0].HeavyDuration * HeavyMargin));
    float HeavyHigh = (PROFILEVar[0].HeavyDuration + (PROFILEVar[0].HeavyDuration * HeavyMargin));
    uint16_t r = random(1, (HeavyMargin * 200));
    uint16_t d = map(r, 1, (HeavyMargin * 200), HeavyLow, HeavyHigh);
    DURATIONTimes[i] = d;
    uint16_t t = random((TIMESVar[0].InitialDelay * 60), TIMESVar[0].TotalSleepTime * 3600);
    INTERVALTimes[i] = t;
  } 
}
void SPURTS()
{
  uint16_t ProfileIntervals = (random((SpurtsEventsMin * 1000), ((SpurtsEventsMax * 1000) + 999)) / 1000);//Find number of weetings
  Interval = ProfileIntervals;
  for (int i = (ProfileIntervals - 1); i >= 0; i--)
  {
    float SpurtsLow = (PROFILEVar[0].SpurtsDuration - (PROFILEVar[0].SpurtsDuration * SpurtsMargin));
    float SpurtsHigh = (PROFILEVar[0].SpurtsDuration + (PROFILEVar[0].SpurtsDuration * SpurtsMargin));
    uint16_t r = random(1, (SpurtsMargin * 200));
    uint16_t d = map(r, 1, (SpurtsMargin * 200), SpurtsLow, SpurtsHigh);
    DURATIONTimes[i] = d;
    uint16_t t = random((TIMESVar[0].InitialDelay * 60), TIMESVar[0].TotalSleepTime * 3600);
    INTERVALTimes[i] = t;
  } 
}
void DAMP()
{
  uint16_t ProfileIntervals = (random((DampEventsMin * 1000), ((DampEventsMax * 1000) + 999)) / 1000);//Find number of weetings
  Interval = ProfileIntervals;
  for (int i = (ProfileIntervals - 1); i >= 0; i--)
  {
    float DampLow = (PROFILEVar[0].DampDuration - (PROFILEVar[0].DampDuration * DampMargin));
    float DampHigh = (PROFILEVar[0].DampDuration + (PROFILEVar[0].DampDuration * DampMargin));
    uint16_t r = random(1, (DampMargin * 200));
    uint16_t d = map(r, 1, (DampMargin * 200), DampLow, DampHigh);
    DURATIONTimes[i] = d;
    uint16_t t = random((TIMESVar[0].InitialDelay * 60), TIMESVar[0].TotalSleepTime * 3600);
    INTERVALTimes[i] = t;
  } 
}

void Flow()
{
   FlowAmount++; //Every time this function is called, increment "count" by 1
}

void RESET()
{
  GO = 0;
  GOHEAT = 0;
  RandomRun = 0;
  MakeTimes = 0;
  WetChance = 1;
  PauseRun = 0;
  CurrentVal = 0;
  LastCurrentVal = 0;
  Duration = 0;
  NextTriggerEventStart = 0;
  NextTriggerEventStop = 0;
  LastTimeMillisB = 0;
  CurrentInterval = 0;
  DiaperBlink = 0;
  MenuPosition = 0; 
  LastMenuPosition = 1;
  Row = 0;
  Column = 0;
  Cursor = 0;
  LastCursor = 0;
  SPECIAL = 0;
  LastButtonPress = 0; 
  ButtonDebounce = 0;
  NewRunTag = 0;
  OutputFeedback = 0;
  LastOutputFeedback = 0;
  BedWetter[0].RunTime = 0;
  BedWetter[0].SolenoidState = 0;
  BedWetter[0].HeatState = 0;
  RlyInitialization();
  for (int i = 0; i <= 49; i++)
  {
    INTERVALTimes[i] = 0;
    DURATIONTimes[i] = 0;
  }
}

void ERASE()
{
  for (int i = 0; i <= 99; i++)
  {
    EEPROM.write(i, 0);
  }
}
