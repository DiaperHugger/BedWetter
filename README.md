This project is part of a collaborative effort to create a "Bed Wetting Simulator". https://www.adisc.org/forum/threads/bed-wetting-simulator-updates.151608/ for discussion and help. PLEASE READ AND ABIDE BY THE FORUM RULES!! 18+ COMMUNITY!! Code at https://github.com/DiaperHugger/BedWetter. V0.2.5 Updated 5/10/21


THIS CODE COMES WITH NO IMPLIED WARRANTY!! USE AT YOUR OWN RISK!!


This device is intended to simulate the experience of bed wetting with the appropriate hardware. This version is intended for Arduino UNO + LCD Shield with buttons. This device controls a selectable output pin intended to control a pump or solenoid. The output behavior is derived by the input variables.

Required hardware for basic operation:

    Arduino UNO
    Arduino compatible LCD Shield with buttons
    Relay or relay board module
    Solenoid or pump

This version can support two different configurations. 

Option ONE is a gravity fed drip using the BedWetter as a solenoid control using a relay. The water source should be placed above the intended resting level. The solenoid activation will allow liquid to flow to the resting level with a volume determined by the height of the water source, diameter of the line used, and the duration of the solenoid activation. User consideration should be made for all external variables including but not limited to water temperature, total amount of liquid used, etc.....

Option Two is a pump activated by the relay. The pump and water source should be placed below the resting level so as to not suffer from a syphon effect that could fully drain the liquid source unintentionally. The total volume is determined by the pump flow rate and duration of the activation. All external variables should be considered including by not limited to water temperature, container type, pump style, flow regulation, etc..... This picture shows a possible configuration of option TWO. Ancillary hardware such as sensors and flow monitors will be integrated in future updates. 

![20210507_235701](https://user-images.githubusercontent.com/83486730/117649057-ea7e4100-b143-11eb-8497-7afaa3d84afc.jpg)


The BedWetter will only function if the Total Sleep Time, Wet Interval, and Wet Duration values are NOT zero. The other variables are optional to the overall function. The SELECT button will initiate the program running and can also pause/resume the current program. Once the device has started the interval and duration times are created. These times will persist if the operation is paused or variables are changed. These times will only be updated if the Total Sleep Time has ended or the Arduino is reset. If a Wet Chance% is entered the device will determine the percentage chance that any activation will happen for the duration of the Total Sleep Time regardless of the other variables. The RunTime counter on the home screen will count regardless of the outcome of the chance. Here is a simple flowchart outlining the internal operation. ![BedWETTER flowchart](https://user-images.githubusercontent.com/83486730/117658232-5c0fbc80-b14f-11eb-8bdc-1c7beced6b0e.jpg)




List of input variables: 

  TIMES Menu:

    Total Sleep Time: Total running time in hours.

    Initial Delay: Delay first interval with a delay in seconds.

  WETTING Menu:

    Wet Chance%: This value determines whether the output will activate during the Total Sleep Time. if left 0 the chance is 100%.

    Random: Option "Pure" will randomize the time intervals and durations using the INTERVAL and DURATION values as limits.

    Wet Interval: This is the number of intervals to activate the output during the Total Sleep time.

    Interval Duration: This is the duration to activate the output in seconds.

  CONTROL Menu:

    Prime: This is a bypass that will activate the output in order to purge or prime the line.

    BackLight: This can control the backlight for the LCD shield and in most cases is pin 10. The pin must be entered manually. USE AT YOUR OWN RISK!

    OutPut Pin: This is the output pin for the relay or pump.

    (These variables can introduce a duty cycle while the output is active.)

      -Duty Cycle%: This is a percentage of "ON" time within the Duty Pulse time. 

      -Duty Pulse: This is the pulse time in seconds.

   RESET Menu

    This function will erase the stored settings.



