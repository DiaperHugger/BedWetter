This project is part of a collaborative effort to create a "Bed Wetting Simulator". https://www.adisc.org/forum/threads/bed-wetting-simulator-updates.151608/ for discussion and help. PLEASE READ AND ABIDE BY THE FORUM RULES!! 18+ COMMUNITY!! Code at https://github.com/DiaperHugger/BedWetter. V0.3.1 Updated 5/23/21


This device is intended to simulate the experience of bed wetting with the appropriate hardware. This version is intended for Arduino UNO + LCD Shield with buttons. This version supports a pump relay as well as a secondary relay that can be temperature controls. 

Required hardware for basic operation:

    Arduino UNO
    Arduino compatible LCD Shield with buttons
    Relay or relay board module
    Solenoid or pump
    
Optional hardware:

    Temperature sensor (DS18B20) for secondary relay control
    Flow meter (Hall Sensor Flowmeter)

This version can support two different configurations. 

Option ONE is a gravity fed drip using the BedWetter as a solenoid control using a relay. The water source should be placed above the intended resting level. The solenoid activation will allow liquid to flow to the resting level with a volume determined by the height of the water source, diameter of the line used, and the duration of the solenoid activation. User consideration should be made for all external variables including but not limited to water temperature, total amount of liquid used, etc.....

Option Two is a pump activated by the relay. The pump and water source should be placed below the resting level so as to not suffer from a syphon effect that could fully drain the liquid source unintentionally. The total volume is determined by the pump flow rate and duration of the activation. All external variables should be considered including but not limited to water temperature, container type, pump style, flow regulation, etc..... This picture shows a possible configuration of option TWO. Ancillary hardware such as sensors and flow monitors will be integrated in future updates. 

![20210507_235701](https://user-images.githubusercontent.com/83486730/117649057-ea7e4100-b143-11eb-8497-7afaa3d84afc.jpg)


The BedWetter will only function if the Total Sleep Time, Wet Interval, and Wet Duration values are NOT zero. The other variables are optional to the overall function. The SELECT button will initiate the program running and can also pause/resume the current program. Once the device has started the interval and duration times are created. These times will persist if the operation is paused or variables are changed. The current program can be reset by pressing the RIGHT button while on the home screen. If a Wet Chance% is entered the device will determine the percentage chance that any activation will happen for the duration of the Total Sleep Time regardless of the other variables. The RunTime counter on the home screen will count regardless of the outcome of the chance. 

The Profile system will disregare the entered INTEVAL and DURATIONS in favor of he Profile generated system!

Here is a simple flowchart outlining the internal operation.![Flowchart](https://user-images.githubusercontent.com/83486730/119256225-87919e80-bb74-11eb-8c93-9af4e81c8558.jpg)

Below is a description of the menu structure.
![HOME2](https://user-images.githubusercontent.com/83486730/119256295-df300a00-bb74-11eb-997e-3662c2adf5fe.jpg)

1: The diaper icon will flash when the program is running.
2: The "H" indicates that the secondary relay is active. A "h" indicates that an IdleTemp is controlling the secondary relay with optional temp sensor.
3: Total liquid flow can be displayed with optional flow meter.
4: RunTime counter keeps track of total program run time. 
5: "NewRun" tag indicates new times will be generated with selected options.
6: Temperature will display is sensor is present. Can be changed from C to F in sensor menu.

Recommended first time operation.

On first powerup all the internal variables are 0's. First determine which relay pin/s you are using. IMPORTANT NOTE! While using the LCD sheild some pins are not available. Relay pins can be (2, 3, 11, 12 ,13) however if the optional flow sensor is used it must be connected to either of the interrupt pins (2, 3). Also you will need to consider which pin to use for the optional temp sensor. A sample configuration may look like this.
![Sample setup](https://user-images.githubusercontent.com/83486730/119257420-13f29000-bb7a-11eb-9127-8d80300957a5.jpg)






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
    
    OutPut Invert: This will invert the output behavior of the relay pin.

    (These variables can introduce a duty cycle while the output is active.)

      -Duty Cycle%: This is a percentage of "ON" time within the Duty Pulse time. 

      -Duty Pulse: This is the pulse time in seconds.

   RESET Menu

    This function will erase the stored settings.



THIS CODE COMES WITH NO IMPLIED WARRANTY!! USE AT YOUR OWN RISK!!
