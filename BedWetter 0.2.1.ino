#include <LiquidCrystal.h>
#include <EEPROM.h>

uint16_t seconds;
uint32_t CycleTime;
uint32_t CycleTime2;
uint32_t CycleTime3; //For serial debug

uint16_t DutyCycleTime;
uint16_t DutyPulseTime;
uint16_t DutyOffSet = 0;

uint8_t GO = 0;
uint8_t DiaperBlink = 0;

uint16_t NextTriggerEventStart;
uint16_t NextTriggerEventStop;
uint8_t RUNLoop = 0;
uint8_t CurrentInterval = 0;
uint8_t InitialDelay = 1;


int RelayPin = 0; //Output pin for relay
int ButtonRAW;
int MenuPosition = 0; 
int LastMenuPosition = 1;

int8_t Row = 0;
int8_t Column = 0;

int8_t Cursor = 0;
int8_t LastCursor = 0;

uint16_t LastButtonPress = 0;

String HomeMessage = "BedWetter V0.2";
//String version = "V0.2";

int8_t CurrentVal;



uint16_t WetEvent1;
uint16_t WetEvent2;
uint16_t WetEvent3;

const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);


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
  uint8_t WetIntervalTime; //Total time to open valve
  //uint8_t Probability;
  uint8_t SolenoidState; //0 off, 1 on
  uint8_t Debug;
};

BedWetterDevice BedWetter[1] =
{
  {0, 0, 0}
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
  uint8_t WetInterval;
  uint8_t WetDuration;
};

WETTINGVariables WETTINGVar[1] =
{
  {0, 0, 0}
};

struct CONTROLVariables
{ 
  uint8_t Prime; 
  uint8_t OutputPin;
  uint8_t DutyCycle;
  uint8_t DutyPulse;
};

CONTROLVariables CONTROLVar[1] =
{
  {0, 0, 0, 0}
};




//TIMES   (10)Total Sleep:, (11)Initial Dly:,
//WETTING (20)Wet Chance:,  (21)Wet Intval:,  (22)Intval Dur:
//CONTROL (30)Prime:,  (31)OutPut Pin:,  (32)Duty Cycle:, (33)Duty Pulse:

String MENUS[] = {"TIMES", "WETTING", "CONTROL"};

//PAD ITEMS TO 16 TOTAL CHARACTERS FOR FULL LINE ON LCD
String TIMESItems[] =   {"Total Sleep/h:  ", "Initial Dly/m:  "};
//int8_t TIMESVAR[] = {0, 0};
String WETTINGItems[] = {"Wet Chance:     ", "Wet Intval:     ", "Intval Dur/s:   "};
//int8_t WETTINGVAR[] = {0, 0, 0};
String CONTROLItems[] = {"Prime:     ", "OutPut Pin:     ", "Duty Cycle%:     ", "Duty Pulse:     "};
//int8_t CONTROLVAR[] = {0, 0, 0, 0};

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
  WETTINGVar[0].WetInterval = EEPROM.read(21);
  WETTINGVar[0].WetDuration = EEPROM.read(22);

  CONTROLVar[0].Prime = EEPROM.read(30);
  CONTROLVar[0].OutputPin = EEPROM.read(31);
  CONTROLVar[0].DutyCycle = EEPROM.read(32);
  CONTROLVar[0].DutyPulse = EEPROM.read(33);

  RelayPin = CONTROLVar[0].OutputPin;
}

void setup()
{
  Serial.begin(9600);
  
  EEPROM_RECALL();
  pinMode(RelayPin, OUTPUT);
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

  EEPROM_RECALL();
  
}

void loop()
{
  
    seconds = millis() / 1000; //Get seconds from power on
  
  
  
  
    if ((CycleTime3 + 1000) < millis())
    {
      //Serial.println(RelayPin);
      Serial.println(NextTriggerEventStart);
      Serial.println(NextTriggerEventStop);
      Serial.println(seconds);
      Serial.println(CurrentInterval);

      CycleTime3 = millis(); //reset time mark
    }

    if ((CycleTime + 200) < millis()) //0.1sec interval
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
        //Serial.print("GO");
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

//if triggered run RLYInit for the interval time and reset NextTrigger at the end>

void EventTrigger()
{
  if ((seconds > NextTriggerEventStart) && (seconds < NextTriggerEventStop))
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
  else if (seconds > NextTriggerEventStop)
  {
    BedWetter[0].SolenoidState = 0;
    RlyInitialization();
    RUNLoop = 0;
    Events();
    if((TIMESVar[0].TotalSleepTime * 60) < seconds)//Stop running after sleep time
    {
      GO = 0;
      RUNLoop = 0;
      NextTriggerEventStart = 0;
      NextTriggerEventStop = 0;
      InitialDelay = 1;
    }
  }
}

void Events() //Triggered by select button and end of EventTrigger stop time
{
    
    if((TIMESVar[0].TotalSleepTime != 0) && (WETTINGVar[0].WetInterval != 0))
    {
      int T = TIMESVar[0].TotalSleepTime * 3600; //Total sleep time in seconds
      T = ((T / WETTINGVar[0].WetInterval) - WETTINGVar[0].WetDuration); //Fixed interval time division
      NextTriggerEventStart = ((T + seconds));
      if(InitialDelay == 1 && TIMESVar[0].InitialDelay != 0)
      {
        NextTriggerEventStart = (seconds + (TIMESVar[0].InitialDelay * 60)); //Minuets
        InitialDelay = 0;
      }
      NextTriggerEventStop = (NextTriggerEventStart + WETTINGVar[0].WetDuration);
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
  digitalWrite(RelayPin, LOW);
  digitalWrite(LED_BUILTIN, LOW);//debug
  //Serial.print("OFF");
}
void RlyON()
{
  digitalWrite(RelayPin, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);//debug
  //Serial.print("ON");
}

void LCDDisplayData() //MenuPosition = ROW,COLUMN
{
    if(MenuPosition == 0)
    {
      lcd.setCursor(0, 1);
      lcd.print(seconds);
    }
    if(MenuPosition != 0)
    {
      GO == 0;
      RUNLoop == 0;
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
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 21) && (MenuPosition != LastMenuPosition)) //WETTING Wet Intervals:
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
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 22) && (MenuPosition != LastMenuPosition)) //WETTING Intval Dur:
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
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 30) && (MenuPosition != LastMenuPosition)) //CONTROL Prime:
    {
      
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(15, 1);
      if(CONTROLVar[0].Prime > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(CONTROLVar[0].Prime);
      Cursor = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 31) && (MenuPosition != LastMenuPosition)) //CONTROL OutPut Pin:
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
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 32) && (MenuPosition != LastMenuPosition)) //CONTROL Duty Cycle:
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
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 33) && (MenuPosition != LastMenuPosition)) //CONTROL Duty Pulse:
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
      LastMenuPosition = MenuPosition;
    }

    

}


void ButtonPressCheck()
{
  ButtonRAW = analogRead (0);
  
  if(ButtonRAW == 1023)
  {
    LastButtonPress = 0;
  }
  if(LastButtonPress == 0)
  {
      if (ButtonRAW < 60) //RIGHT
        {
          
          Column = Column + 1;
          if(Column >= MaxColumnsPerItem[Row -1])
          {
              Column = (MaxColumnsPerItem[Row -1] -1);
          }
          LastButtonPress = 1;
        }
        else if (ButtonRAW < 220) //UP
        {
          if(Cursor == 1)
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
            LastButtonPress = 1;
          }
          else
          {
            Row = Row + 1;
            if(Row > MENUSSize)
            {
              Row = MENUSSize;
            }
            Column = 0;
            LastButtonPress = 1;
          }
          
        }
        else if (ButtonRAW < 430) //DOWN
        {
          if(Cursor == 1)
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
            LastButtonPress = 1;
          }
          else
          {
            Row = Row - 1;
            if(Row < 0)
            {
              Row = 0;
            }
            Column = 0;
            LastButtonPress = 1;
          }
          
        }
        else if (ButtonRAW < 640) //LEFT
        {
          Column = Column - 1;
          if(Column < 0)
          {
            Column = 0;
          }
          LastButtonPress = 1;
        }
        else if (ButtonRAW < 850) //SELECT
        {
          if((Cursor == 0) && (MenuPosition != 0))
          {
            Cursor = 1;
            CurrentVal = EEPROM.read(MenuPosition);
            Serial.println(CurrentVal);
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
              Events();
            }
            GO = 1;
          }
          else
          {
            GO = 0;
          }
          


          LastButtonPress = 1;
        }
  }
  MenuPosition = ((Row * 10) + Column); //Encode MenuPosition
}
