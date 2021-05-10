/*BedWETTER v0.2.5  Updated 5/10/21
This project is part of a collaborative effort to create a "Bed Wetting Simulator". This device is intended to simulate the experience of bed wetting with
the appropriate hardware. 
https://www.adisc.org/forum/threads/bed-wetting-simulator-updates.151608/ for discussion and help. 
PLEASE READ AND ABIDE BY THE FORUM RULES!! 18+ COMMUNITY!! 
Code at https://github.com/DiaperHugger/BedWetter.

THIS CODE COMES WITH NO IMPLIED WARRANTY!! USE AT YOUR OWN RISK!!
*/
#include <LiquidCrystal.h>
#include <EEPROM.h>

//Shield button value defaults: right 60, up 200, down 400, left 600, select 800
const int right = 60;
const int up = 200;
const int down = 400;
const int left = 600;
const int select = 800;

int RelayPin = 0; //Output pin for relay. Changes to stored value on power up.
int LCDBackLight = 0; //Manual select the LCD backlight pin. USE AT OWN RISK!
  
uint16_t seconds;
uint32_t CycleTime;  //LCD Diasply loop
uint32_t CycleTime2; //Duty cycle loop
uint32_t CycleTime3; //Run time loop

uint16_t DutyCycleTime;
uint16_t DutyPulseTime;
uint16_t DutyOffSet = 0;

uint8_t GO = 0;
uint8_t RUNLoop = 0;
uint8_t RandomRun = 0;
uint8_t MakeTimes = 0;
uint8_t WetChance = 1;

int8_t CurrentVal;
int8_t LastCurrentVal;
uint16_t Duration;

uint16_t NextTriggerEventStart;
uint16_t NextTriggerEventStop;

uint8_t CurrentInterval = 0;
uint8_t InitialDelay = 1;

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

String HomeMessage = "BedWetter V0.2";


const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

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

struct CONTROLVariables
{ 
  uint8_t Prime; 
  uint8_t BackLight;
  uint8_t OutputPin;
  uint8_t DutyCycle;
  uint8_t DutyPulse;
};

CONTROLVariables CONTROLVar[1] =
{
  {0, 0, 0, 0, 0}
};

uint16_t INTERVALTimes[50];//Array to store derived intervals.

String MENUS[] = {"TIMES", "WETTING", "CONTROL", "RESET"};

//PAD ITEMS TO 16 TOTAL CHARACTERS FOR FULL LINE ON LCD
String TIMESItems[] =   {"Total Sleep/h:  ", "Initial Dly/m:  "};
String WETTINGItems[] = {"Wet Chance%:    ", "Random:         ", "Wet Intval:     ", "Intval Dur/s:   "};
String CONTROLItems[] = {"Prime:     ", "BackLight       ", "OutPut Pin:     ", "Duty Cycle%:     ", "Duty Pulse:     "};
String RESETItems[] = {"RESET SETTINGS"};
String RANDOMItems[] = {"NONE", "PURE"};

int8_t MENUSSize = (sizeof(MENUS) / sizeof(MENUS[0]));
int8_t TIMESItemsSize = (sizeof(TIMESItems) / sizeof(TIMESItems[0]));
int8_t WETTINGItemsSize = (sizeof(WETTINGItems) / sizeof(WETTINGItems[0]));
int8_t CONTROLItemsSize = (sizeof(CONTROLItems) / sizeof(CONTROLItems[0]));

int MaxColumnsPerItem[] = {TIMESItemsSize, WETTINGItemsSize, CONTROLItemsSize};


void EEPROM_RECALL() //EEPROM address layout tied to MenuPosition!  +10 per row.
{
  TIMESVar[0].TotalSleepTime = EEPROM.read(10);
  TIMESVar[0].InitialDelay = EEPROM.read(11);

  WETTINGVar[0].WetChance = EEPROM.read(20);
  WETTINGVar[0].Random = EEPROM.read(21);
  WETTINGVar[0].WetInterval = EEPROM.read(22);
  WETTINGVar[0].WetDuration = EEPROM.read(23);

  CONTROLVar[0].OutputPin = EEPROM.read(32);
  CONTROLVar[0].DutyCycle = EEPROM.read(33);
  CONTROLVar[0].DutyPulse = EEPROM.read(34);

  RelayPin = CONTROLVar[0].OutputPin;
}

void setup()
{
  Serial.begin(9600);
  
  EEPROM_RECALL();
  pinMode(RelayPin, OUTPUT);
  pinMode(LCDBackLight, OUTPUT);
  digitalWrite(LCDBackLight, HIGH); //LCD ON

  pinMode(LED_BUILTIN, OUTPUT); //For testing

  CycleTime = millis(); //time mark for cycled events
  CycleTime2 = millis();
  CycleTime3 = millis(); //time mark for cycled events
  lcd.begin(16, 2);
  lcd.createChar(0, diaperleft);
  lcd.createChar(1, diaperright);
  lcd.createChar(2, diaperleft1);
  lcd.createChar(3, diaperright1);
  
  RelayPin = CONTROLVar[0].OutputPin; //Pull relay pin from stored value
  digitalWrite(RelayPin, HIGH);
}

void loop()
{
    seconds = millis() / 1000; //Get seconds from power on
    if ((CycleTime3 + 1000) < millis())
    {
      if(GO == 1)
      {
        BedWetter[0].RunTime++; //Total running time
      }
      CycleTime3 = millis(); //reset time mark
    }
    if ((CycleTime + 100) < millis()) //0.1sec interval
      {
        LCDDisplayData();
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
      }
      if(RUNLoop == 1)
      {
        RlyInitialization(); //Relay initialization loop
      }
      ButtonPressCheck();
}

void DutyCycle()
{
    DutyCycleTime = ((CONTROLVar[0].DutyPulse * 1000) * ((float)CONTROLVar[0].DutyCycle / 100));
    DutyPulseTime = ((CONTROLVar[0].DutyPulse * 1000) - DutyCycleTime);
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
          CycleTime2 = millis(); //reset time mark
          //Serial.print("MARK");
        }
}

void EventTrigger()
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
    RUNLoop = 1;
  }
  else if (BedWetter[0].RunTime > NextTriggerEventStop)
  {
    if(CurrentInterval == 0)//If interval is zero then stop the loop
    {
      BedWetter[0].SolenoidState = 0;
      RUNLoop = 0;
    }
    else
    {
      CurrentInterval--;
    }
    BedWetter[0].SolenoidState = 0;
    RlyInitialization();
    RUNLoop = 0;
    Events();
    if((TIMESVar[0].TotalSleepTime * 3600) < BedWetter[0].RunTime)//Stop running after sleep time
    {
      RESET();
    }
  }
}
int sort_desc(const void *cmp1, const void *cmp2)//Sort function
{
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  return b - a;
}
void Events() //Triggered by select button and end of EventTrigger stop time
{
    int Interval = WETTINGVar[0].WetInterval;
    Duration = WETTINGVar[0].WetDuration;
    
    if(MakeTimes == 1)//Derive interval times
    {
      if(WETTINGVar[0].Random == 0)//None random interval times
      {
        for (int i = Interval; i >= 1; i--)//Load array
        {
          int T = TIMESVar[0].TotalSleepTime * 3600;
          T = ((T / i) - Duration);
          INTERVALTimes[Interval - i] = T;
        }
      }
      else if(WETTINGVar[0].Random == 1)//Pure random interval times
      {
        randomSeed(analogRead(1));
          for (int i = 0; i <= (Interval - 1); i++)//Load array
          {
            int r = random(1, TIMESVar[0].TotalSleepTime * 3600);
            r = r - Duration;
            INTERVALTimes[i] = r;
          }
      }
      //Sort array descending
      int INTERVALTimes_length = sizeof(INTERVALTimes) / sizeof(INTERVALTimes[0]);
      qsort(INTERVALTimes, INTERVALTimes_length, sizeof(INTERVALTimes[0]), sort_desc);
      CurrentInterval = Interval;

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
      
      if(WETTINGVar[0].Random == 1)//Random durations
      {
        Duration = random(1, WETTINGVar[0].WetDuration);
      }
      MakeTimes = 0;
    }
      
    if(WetChance == 1)
    {
      if((TIMESVar[0].TotalSleepTime != 0) && (WETTINGVar[0].WetInterval != 0) && (WETTINGVar[0].WetDuration != 0))//Main function for start/stop times calculated each interval.
      {
        
        NextTriggerEventStart = (INTERVALTimes[CurrentInterval - 1]);
        if(InitialDelay == 1 && TIMESVar[0].InitialDelay != 0)
        {
          NextTriggerEventStart = (BedWetter[0].RunTime + (TIMESVar[0].InitialDelay * 60)); //Minuets
          InitialDelay = 0;
        }
        NextTriggerEventStop = (NextTriggerEventStart + Duration);
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

void RlyOFF()
{
  digitalWrite(RelayPin, HIGH);
  digitalWrite(LED_BUILTIN, LOW);//debug
}
void RlyON()
{
  digitalWrite(RelayPin, LOW);
  digitalWrite(LED_BUILTIN, HIGH);//debug
}

void LCDDisplayData() //MenuPosition = ROW,COLUMN
{
    if(MenuPosition == 0)
    {
      if(MenuPosition == 0)
        {
          lcd.setCursor(0, 1);
          lcd.print("RunTime:");
          lcd.setCursor(8, 1);
          lcd.print(BedWetter[0].RunTime);
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
      SPECIAL = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 11) && (MenuPosition != LastMenuPosition)) //TIMES Initial Dly
    {
      lcd.setCursor(0, 1);
      lcd.print(TIMESItems[Column]);
      lcd.setCursor(15, 1);
      if(TIMESVar[0].InitialDelay > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(TIMESVar[0].InitialDelay);
      Cursor = 0;
      SPECIAL = 0;
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
      lcd.print(WETTINGItems[Column]);
      if(WETTINGVar[0].Random == 0)
      {
        lcd.setCursor(12, 1);
        lcd.print(RANDOMItems[0]);
      }
      else if(WETTINGVar[0].Random == 1)
      {
        lcd.setCursor(12, 1);
        lcd.print(RANDOMItems[1]);
      }
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 22) && (MenuPosition != LastMenuPosition)) //WETTING Wet Intervals:
    {
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
    else if ((MenuPosition == 30) && (MenuPosition != LastMenuPosition)) //CONTROL Prime:
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
    else if ((MenuPosition == 31) && (MenuPosition != LastMenuPosition)) //CONTROL BackLight:
    {
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(13, 1);
      lcd.print("ON");
      CONTROLVar[0].BackLight = 1;
      Cursor = 0;
      SPECIAL = 1;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 32) && (MenuPosition != LastMenuPosition)) //CONTROL OutPut Pin:
    {
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
    else if ((MenuPosition == 33) && (MenuPosition != LastMenuPosition)) //CONTROL Duty Cycle:
    {
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
    else if ((MenuPosition == 34) && (MenuPosition != LastMenuPosition)) //CONTROL Duty Pulse:
    {
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
    else if ((MenuPosition == 40) && (MenuPosition != LastMenuPosition)) //RESET RESET SETTINGS:
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
          ButtonDebounce = 1;
          LastButtonPress = 4;
        }
        else if (ButtonRAW < select) //SELECT
        {
          if(MenuPosition == 40)
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
            EEPROM.write(MenuPosition, CurrentVal);
            EEPROM_RECALL();
            Cursor = 0;
          }
          
          if((MenuPosition == 0) && (GO != 1))
          {
            if(NextTriggerEventStart == 0)
            {
              MakeTimes = 1;
              Events();
            }
            GO = 1;
            RandomRun = 1;
          }
          else
          {
            GO = 0;
          }
          ButtonDebounce = 1;
          LastButtonPress = 5;
        }
  }
  MenuPosition = ((Row * 10) + Column); //Encode MenuPosition
}

void ALTERNATE()
{
  if(MenuPosition == 21)//Random
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
        lcd.setCursor(12, 1);
        lcd.print(RANDOMItems[0]);
      }
      else if(CurrentVal == 1)
      {
        lcd.setCursor(12, 1);
        lcd.print(RANDOMItems[1]);
      }
      WETTINGVar[0].Random = CurrentVal;
  }
  
  if(MenuPosition == 22)//Wet Intefvals
  {
    if(LastButtonPress == 2)//UP
      {
        CurrentVal++;
        if(CurrentVal > 50)
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
          CurrentVal = 50;
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
  
  if(MenuPosition == 30)//PRIME
  {
    if(CONTROLVar[0].Prime == 0)
    {
      lcd.setCursor(13, 1);
      lcd.print("ON ");
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
  if(MenuPosition == 31)//BackLight
  {
    if(CONTROLVar[0].BackLight == 0)
    {
      lcd.setCursor(13, 1);
      lcd.print("ON ");
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
}

void RESET()
{
  GO = 0;
  RUNLoop = 0;
  WetChance = 1;
  NextTriggerEventStart = 0;
  NextTriggerEventStop = 0;
  BedWetter[0].RunTime = 0;
  CurrentInterval = 0;
  CurrentVal = 0;
  InitialDelay = 1;
  MenuPosition = 0;
  Row = 0;
  Column = 0;
  lcd.setCursor(0, 1);
  lcd.clear();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.setCursor(1, 0);
  lcd.write(byte(1));
  lcd.setCursor(2, 0);
  lcd.print(HomeMessage);
}

void ERASE()
{
  for (int i = 0; i <= 99; i++)
  {
    EEPROM.write(i, 0);
  }
}
