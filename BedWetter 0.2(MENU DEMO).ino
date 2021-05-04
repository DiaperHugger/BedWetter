#include <LiquidCrystal.h>
#include <EEPROM.h>

uint16_t seconds;
uint32_t CycleTime;
uint32_t CycleTime2;

const int RelayPin = 2; //Output pin for relay
int ButtonRAW;
int MenuPosition = 0; 
int LastMenuPosition = 1;

int8_t Row = 0;
int8_t Column = 0;

int8_t Cursor = 0;
int8_t LastCursor = 0;

uint16_t LastButtonPress = 0;

String HomeMessage = "BedWetter";
String version = "V0.2";

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

//TIMES   (10)Total Sleep:, (11)Initial Dly:,
//WETTING (20)Wet Chance:,  (21)Wet Intval:,  (22)Intval Dur:
//CONTROL (30)OutPut Pin:,  (31)Duty Cycle:,  (32)Duty Pulse:

String MENUS[] = {"TIMES", "WETTING", "CONTROL"};

//PAD ITEMS TO 16 TOTAL CHARACTERS FOR FULL LINE ON LCD
String TIMESItems[] =   {"Total Sleep:    ", "Initial Dly:    "};
int8_t TIMESVAR[] = {0, 0};
String WETTINGItems[] = {"Wet Chance:     ", "Wet Intval:     ", "Intval Dur:     "};
int8_t WETTINGVAR[] = {0, 0, 0};
String CONTROLItems[] = {"OutPut Pin:     ", "Duty Cycle:     ", "Duty Pulse:     "};
int8_t CONTROLVAR[] = {0, 0, 0};

int8_t MENUSSize = (sizeof(MENUS) / sizeof(MENUS[0]));
int8_t TIMESItemsSize = (sizeof(TIMESItems) / sizeof(TIMESItems[0]));
int8_t WETTINGItemsSize = (sizeof(WETTINGItems) / sizeof(WETTINGItems[0]));
int8_t CONTROLItemsSize = (sizeof(CONTROLItems) / sizeof(CONTROLItems[0]));

int MaxColumnsPerItem[] = {TIMESItemsSize, WETTINGItemsSize, CONTROLItemsSize};


void EEPROM_RECALL() //EEPROM address layout +10 per row.
{
  for (int i = 0; i <= (TIMESItemsSize -1); i++) 
  {
    TIMESVAR[i] = EEPROM.read(i + 10);
  }
  for (int i = 0; i <= (WETTINGItemsSize -1); i++) 
  {
    WETTINGVAR[i] = EEPROM.read(i + 20);
  }
  for (int i = 0; i <= (CONTROLItemsSize -1); i++) 
  {
    CONTROLVAR[i] = EEPROM.read(i + 30);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(RelayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); //For testing

  CycleTime = millis(); //time mark for cycled events
  CycleTime2 = millis(); //time mark for cycled events
  lcd.begin(16, 2);
  lcd.createChar(0, diaperleft);
  lcd.createChar(1, diaperright);


  BedWetter[0].WetIntervalTime = 10; //10 seconds
  
  //hard coded times//////////////////////////////////////////////////
  WetEvent1 = 3600; //hour 1
  WetEvent2 = 7200; //hour 2 
  WetEvent3 = 10800; //hour 3
  
  // EEPROM.write(0, 1);
  // EEPROM.write(1, 2);
  //randomnumber();
  EEPROM_RECALL();
  
}

void loop()
{
  seconds = millis() / 1000; //Get seconds from power on
  
  

    if ((CycleTime + 100) < millis()) //0.1sec interval
      {
        //Serial.println(seconds); //Debug
        eventcheck(); //Check for actions
        // lcd.setCursor(13, 0);
        // lcd.print(MenuPosition);
        // lcd.setCursor(12, 1);
        // lcd.print(ButtonRAW);
        // lcd.setCursor(0, 1);
        // lcd.print(LastButtonPress);
        //Serial.print(Row); //Debug
        //Serial.println(Column); //Debug
        //Serial.print(".");
        //Serial.print(MaxColumnsPerItem[0]);
        //Serial.print(MaxColumnsPerItem[1]);
        //Serial.println(MaxColumnsPerItem[2]);
        //Serial.println(".");
        //Serial.println(MenuPosition);
        
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
        
      
      
      RlyInitialization(); //Relay initialization loop
      ButtonPressCheck();
  
}

void LCDDisplayData() //MenuPosition = ROW,COLUMN
{
    if ((MenuPosition == 0) && (MenuPosition != LastMenuPosition)) //HOME
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      lcd.setCursor(1, 0);
      lcd.write(byte(1));
      lcd.setCursor(4, 0);
      lcd.print(HomeMessage);
      lcd.setCursor(8, 1);
      lcd.print(version);
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
      if(TIMESVAR[Column] > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(TIMESVAR[Column]);
      Cursor = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 11) && (MenuPosition != LastMenuPosition)) //TIMES Initial Dly
    {
      lcd.setCursor(0, 1);
      lcd.print(TIMESItems[Column]);
      lcd.setCursor(15, 1);
      if(TIMESVAR[Column] > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(TIMESVAR[Column]);
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
      if(WETTINGVAR[Column] > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(WETTINGVAR[Column]);
      Cursor = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 21) && (MenuPosition != LastMenuPosition)) //WETTING Wet Intervals:
    {
      
      
      lcd.setCursor(0, 1);
      lcd.print(WETTINGItems[Column]);
      lcd.setCursor(15, 1);
      if(WETTINGVAR[Column] > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(WETTINGVAR[Column]);
      Cursor = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 22) && (MenuPosition != LastMenuPosition)) //WETTING Intval Dur:
    {
      
      
      lcd.setCursor(0, 1);
      lcd.print(WETTINGItems[Column]);
      lcd.setCursor(15, 1);
      if(WETTINGVAR[Column] > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(WETTINGVAR[Column]);
      Cursor = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 30) && (MenuPosition != LastMenuPosition)) //CONTROL OutPut Pin:
    {
      
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENUS[Row -1]);
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(15, 1);
      if(CONTROLVAR[Column] > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(CONTROLVAR[Column]);
      Cursor = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 31) && (MenuPosition != LastMenuPosition)) //CONTROL Duty Cycle:
    {
      
      
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(15, 1);
      if(CONTROLVAR[Column] > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(CONTROLVAR[Column]);
      Cursor = 0;
      LastMenuPosition = MenuPosition;
    }
    else if ((MenuPosition == 32) && (MenuPosition != LastMenuPosition)) //CONTROL Duty Pulse:
    {
      
      
      lcd.setCursor(0, 1);
      lcd.print(CONTROLItems[Column]);
      lcd.setCursor(15, 1);
      if(CONTROLVAR[Column] > 9)
      {
        lcd.setCursor(14, 1);
      }
      lcd.print(CONTROLVAR[Column]);
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
          


          LastButtonPress = 1;
        }
  }
  MenuPosition = ((Row * 10) + Column); //Encode MenuPosition
}





void eventcheck()
{
    if(seconds == WetEvent1)
      {
        BedWetter[0].SolenoidState = 1; //on
        Serial.println("on");
      }
    else if(seconds == (WetEvent1 + BedWetter[0].WetIntervalTime))
      {
        BedWetter[0].SolenoidState = 0; //off
        Serial.println("off");
      }

    if(seconds == WetEvent2)
      {
        BedWetter[0].SolenoidState = 1; //on
        Serial.println("on");
      }
    else if(seconds == (WetEvent2 + BedWetter[0].WetIntervalTime))
      {
        BedWetter[0].SolenoidState = 0; //off
        Serial.println("off");
      }

    if(seconds == WetEvent3)
      {
        BedWetter[0].SolenoidState = 1; //on
        Serial.println("on");
      }
    else if(seconds == (WetEvent3 + BedWetter[0].WetIntervalTime))
      {
        BedWetter[0].SolenoidState = 0; //off
        Serial.println("off");
      }
}

void RlyInitialization()
{
  if (BedWetter[0].SolenoidState == 1)
  {
    digitalWrite(RelayPin, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);//debug
    
  }
  else if (BedWetter[0].SolenoidState == 0)
  {
    digitalWrite(RelayPin, LOW);
    digitalWrite(LED_BUILTIN, LOW);//debug
    
  }
}
