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

   This will divied the INTERVAL and DURATION into the TOTAL SLEEP time evenly.
    
![Wetting pure](https://user-images.githubusercontent.com/83486730/119259031-291eed00-bb81-11eb-9d02-79d085678ba2.jpg)

WETTING: RANDOM: PURE
    
   This will create random interval and duration times using the entered values as limts. Note that the total number of intervals will not change only the interval times! 
   
![Wetting Profile](https://user-images.githubusercontent.com/83486730/119259257-37213d80-bb82-11eb-9dd2-9e701fad24d9.jpg)

WETTING: RANDOM: PROFILE

   This options will ignore entered INTERVAL and DURATION. You can choose a percentage distrubution (100% total) across the 5 different profiles (FLOOD, HEAVY, SPURTS, DAMP, DRY). You can also choose the durations for each profile type. The intervals for each profile are hard coded. 
   
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
