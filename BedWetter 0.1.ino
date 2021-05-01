/*BedWetter 0.1 by LainIsLain
Automated device that simulates bed wetting. This version has three coded wet times under WetEvent 1 thru 3.
Serial print shows current time in seconds after reset and will show on/off message at trigger times. The duration
is set by changing "BedWetter[0].WetIntervalTime" in setup(). The "BedWetterDevice" struct is setup for future additions of control
parameters. Relay pin is currenlty 2. If the relay behavoir is inverted, swap the HIGH and LOW for the digitalWrite events 
under RlyInitialization() function.
*/ 

//hard coded times in seconds//////////////////////////////////////////////////
uint16_t WetEvent1 = 3600; //hour 1
uint16_t WetEvent2 = 7200; //hour 2 
uint16_t WetEvent3 = 10800; //hour 3

uint16_t seconds;
uint32_t CycleTime;
const int RelayPin = 2; //Output pin for relay

struct BedWetterDevice //Attributes
{ 
  uint8_t WetIntervalTime; //Total time to open valve
  uint8_t SolenoidState; //0 off, 1 on
  uint8_t Debug;
  //uint8_t Probability;
};

BedWetterDevice BedWetter[1] =
{
  {0, 0, 0}
};

void setup()
{
  Serial.begin(9600);
  pinMode(RelayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); //For debug
  digitalWrite(LED_BUILTIN, LOW);//debug
  CycleTime = millis(); //Time mark for cycled events
  
  BedWetter[0].WetIntervalTime = 10; //10 seconds
}

void loop()
{
  seconds = millis() / 1000; //Get seconds from power on

    if ((CycleTime + 500) < millis()) //0.5sec interval
      {
        Serial.println(seconds); //Debug
        eventcheck(); //Check for actions
        
        CycleTime = millis(); //reset time mark
      }

  RlyInitialization(); //Relay initialization loop
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
    digitalWrite(RelayPin, HIGH); //Change if relay action is inverted
    digitalWrite(LED_BUILTIN, HIGH); //debug
    
  }
  else if (BedWetter[0].SolenoidState == 0)
  {
    digitalWrite(RelayPin, LOW); //Change if relay action is inverted
    digitalWrite(LED_BUILTIN, LOW); //debug
    
  }
}
