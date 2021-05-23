![HOME2](https://user-images.githubusercontent.com/83486730/119241300-495f9500-bb0a-11eb-8698-b69c5e57e39d.jpg)


Home Screen:

   1: The diaper icon will flash when the program is running.

   2: The "H" indicates that the secondary relay is active. A "h" indicates that an IdleTemp is controlling the secondary relay with optional temp sensor.

   3: Total liquid flow can be displayed with optional flow meter.

   4: RunTime counter keeps track of total program run time. 

   5: "NewRun" tag indicates new times will be generated with selected options.

   6: Temperature will display if sensor is present. Can be changed from C to F in sensor menu.



![Times Sleep](https://user-images.githubusercontent.com/83486730/119258661-80bc5900-bb7f-11eb-9de2-0435f479f17b.jpg)

TIMES: TOTAL SLEEP/h:

   The total sleep time in hours.

![Times initial](https://user-images.githubusercontent.com/83486730/119258828-3687a780-bb80-11eb-927a-79d42fdeac72.jpg)

TIMES: INITIAL DLY/m:

   The initial delay to start the program. This value is added to all three random types.
    
![Wetting chance](https://user-images.githubusercontent.com/83486730/119258892-88303200-bb80-11eb-97c7-c8c3053761f6.jpg)

WETTING: WET CHANCE%:

   This is a percentage chance that any relay activation will happen. If the TOTAL SLEEP is entered the RUNTIME counter will still count.
    
![Wetting none](https://user-images.githubusercontent.com/83486730/119258976-dc3b1680-bb80-11eb-9231-c16c7d6c9ff7.jpg)

WETTING: RANDOM: NONE

   This will divide the INTERVAL and DURATION into the TOTAL SLEEP time evenly.
    
![Wetting pure](https://user-images.githubusercontent.com/83486730/119259031-291eed00-bb81-11eb-9d02-79d085678ba2.jpg)

WETTING: RANDOM: PURE
    
   This will create random interval and duration times using the entered values as limits. Note that the total number of intervals will not change only the interval times! 
   
![Wetting Profile](https://user-images.githubusercontent.com/83486730/119259257-37213d80-bb82-11eb-9dd2-9e701fad24d9.jpg)

WETTING: RANDOM: PROFILE

   This options will ignore entered INTERVAL and DURATION. You can choose a percentage distribution (100% total) across the 5 different profiles (FLOOD, HEAVY, SPURTS, DAMP, DRY). You can also choose the durations for each profile type. The intervals for each profile are hard coded. 
   
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
   
   These defaults can be changed to fit taste and consist of a MIN/MAX interval and percentage +/- for the duration to add a bit of randomness.
 
![Wetting wet interval](https://user-images.githubusercontent.com/83486730/119259319-8d8e7c00-bb82-11eb-92c5-c7a72f9172a6.jpg)

WETTING: WET INTVAL:

This is the number of intervals to divide the TOTAL SLEEP time.

![Wetting wet duration](https://user-images.githubusercontent.com/83486730/119259347-b4e54900-bb82-11eb-941f-059c9d01eaf3.jpg)

WETTING: INTVAL DUR/s:

This is the duration in seconds for the wetting event. 

![Profile Flood](https://user-images.githubusercontent.com/83486730/119259492-6f754b80-bb83-11eb-93e6-6089e5007c7d.jpg)

PROFILE: (FLOOD%, HEAVY%, SPURTS%, DAMP%, DRY%)

These are the different profiles to choose a total percentage distribution across (total 100%). You can change the hard coded values to fit your taste. DRY% is a profile where no activation happens. The outcome of a chosen profile is weighted by the percentage and is chosen once per new run.

![Profile flood dur](https://user-images.githubusercontent.com/83486730/119259623-ed395700-bb83-11eb-9385-d0c5c9a41f0f.jpg)

PROFILE: (FLOOD DUR/s, HEAVY DUR/s, SPURTS DUR/s, DAMP DUR/s)

These are the different durations for each profile to set a target (15% +/- by default). You can change this hard coded percentage to fit the desired randomness.

![Control prime](https://user-images.githubusercontent.com/83486730/119259672-2671c700-bb84-11eb-9c5c-0a323429638a.jpg)

CONTROL: PRIME:

This is intended to be used to prime the line prior to use.

![Control backlight](https://user-images.githubusercontent.com/83486730/119259730-6cc72600-bb84-11eb-8494-f2e831763b6c.jpg)

CONTROL: BACKLIGHT:

This can control the LCD backlight if you manually enter the LCD backlight pin value in the code prior to uploading. Possible option is pin 10 but USE AT YOUR OWN RISK!

![Control pump](https://user-images.githubusercontent.com/83486730/119259798-d47d7100-bb84-11eb-8a98-02fb85263ffa.jpg)

CONTROL: PUMPPIN:

Select which pin to use as the pump/solenoid relay pin. While using the LCD shield your pins are limited to (2, 3, 11, 12, 13). Pin (0, 1) are used for the USB serial connection on the Arduino. You can use them but will lose serial feedback. Remember to hard reset after changing hardware options!!

![Control outputinvert](https://user-images.githubusercontent.com/83486730/119259852-068ed300-bb85-11eb-8dcb-2ebd629206c1.jpg)

CONTROL: OUTPUTINVERT:

This will invert the output behaviour for both relay pins.

![Control duty cycle](https://user-images.githubusercontent.com/83486730/119259938-62595c00-bb85-11eb-99e8-7a3976b32e23.jpg)

CONTROL: DUTY CYCLE%:

If needed the output can be chopped with a duty cycle. This changes the percentage of ON time within the DUTY PULSE time. Default of 0 will have no effect.

![Control duty pulse](https://user-images.githubusercontent.com/83486730/119259970-8ddc4680-bb85-11eb-8c75-02aa3f3cb9d7.jpg)

CONTROL: DUTY PULSE:

This sets the pulse time in seconds to chop the output. Default of 0 will have no effect.

![sensors temp valid](https://user-images.githubusercontent.com/83486730/119260087-0f33d900-bb86-11eb-9b46-bbf62f05e41c.jpg)

SENSORS: TEMP:

To add a temp sensor you must connect a compatible sensor (DS18B20) to the Arduino and also change the hard coded pin accordingly!

      const byte ONEWIRE_PIN = 13;
      
If a sensor is found the display will show the last 8 hex digits of the sensor ID and the current temp. By default the temps are in celsus but you can change to fahrenheit here if desired. If no sensor is found then the temps indicator on the home screen is left blank.

![sensors flow prime](https://user-images.githubusercontent.com/83486730/119260503-ead8fc00-bb87-11eb-8389-1385d99381d8.jpg)

SENSORS: FLOW:

If a flow sensor is added you can track the total liquid moved during the program run. The sensor input pin must be entered manually and must use an interrupt pin on the Arduino UNO (2, 3);

      uint8_t FlowPin = 2;
This menu has three options (OFF, SHOW, PRIME). OFF/SHOW will change the behaviour on the home screen. If the sensor is working you can choose PRIME to activate the pump/solenoid and see the flow counter change.

In order to display proper liquid amounts you must calibrate the sensor.

Make sure the sensor is working then reset the Arduino. Using a measuring cup or scale use this menu to dispense a full 1 liter of liquid. Perform this task a few times to ensure a proper reading. Write down the displayed count and enter this into the code here.

      uint16_t FlowCal = 1540;
      
The home screen should now show the total liquid moved in liters.

![heat rlypin](https://user-images.githubusercontent.com/83486730/119260552-270c5c80-bb88-11eb-961a-b5ce1c5ca6ac.jpg)

HEAT: RLYPIN:

This selects the secondary relay pin. 

![heat idletemp](https://user-images.githubusercontent.com/83486730/119260619-794d7d80-bb88-11eb-960b-7a593d026c60.jpg)

HEAT: IDLETEMP:

You can set a temperature that will control the secondary relay. Enter hole degrees celsius. Consideration must be taken for proper usage! This should only be used as a backup to an existing temperature controlled heating device for the liquid! USE AT YOUR OWN RISK!

![RESET](https://user-images.githubusercontent.com/83486730/119260721-e19c5f00-bb88-11eb-90bd-f1a5f01c54cd.jpg)

RESET: CLEAR!!:

This option will wipe all stored values. The device will check for a different version and wipe the settings although this option should be used when uploading a different version to avoid unpredictable behaviours.



