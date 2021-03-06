This project is part of a collaborative effort to create a "Bed Wetting Simulator". https://www.adisc.org/forum/threads/bed-wetting-simulator-updates.151608/ for discussion and help. PLEASE READ AND ABIDE BY THE FORUM RULES!! 18+ COMMUNITY!!
THIS CODE COMES WITH NO IMPLIED WARRANTY!! USE AT YOUR OWN RISK!!


This device is intended to simulate the experience of bed wetting with the appropriate hardware. This version is intended for Arduino UNO + LCD Shield with buttons. This version supports a pump relay as well as a secondary relay that can be temperature controlled. 

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


The BedWetter will only function if the Total Sleep Time is NOT zero. The other variables are optional to the overall function. The SELECT button will initiate the program running and can also pause/resume the current program. Once the device has started the interval and duration times are created. These times will persist if the operation is paused or variables are changed. The current program can be reset by pressing the RIGHT button while on the home screen. If a Wet Chance% is entered the device will determine the percentage chance that any activation will happen for the duration of the Total Sleep Time regardless of the other variables. The RunTime counter on the home screen will count regardless of the outcome of the chance. 

The Profile system will disregard the entered INTERVAL and DURATIONS in favor of the Profile generated system!

Here is a flowchart outlining the internal operation.![BW](https://user-images.githubusercontent.com/83486730/120636846-01eada00-c423-11eb-972b-5124345ec962.jpeg)


![HOME2](https://user-images.githubusercontent.com/83486730/119256295-df300a00-bb74-11eb-997e-3662c2adf5fe.jpg)

1: The diaper icon will flash when the program is running.

2: The "H" indicates that the secondary relay is active. A "h" indicates that an IdleTemp is controlling the secondary relay with optional temp sensor.

3: Total liquid flow can be displayed with optional flow meter.

4: RunTime counter keeps track of total program run time. 

5: "NewRun" tag indicates new times will be generated with selected options.

6: Temperature will display if sensor is present. Can be changed from C to F in sensor menu.

Recommended first time operation. PLEASE SEE THE MENU DESCRIPTION FILE AS NEEDED!

On first powerup all the internal variables are 0's. First determine which relay pin/s you are using. IMPORTANT NOTE! While using the LCD shield some pins are not available. Relay pins can be (2, 3, 11, 12 ,13) however if the optional flow sensor is used it must be connected to either of the interrupt pins (2, 3). Also you will need to consider which pin to use for the optional temp sensor. Be sure to change the option "OutPutInvert'' to change the relay behaviour if needed! This option will affect both relay channels! If your LCD shield buttons aren't working you may need to change the default read values here.
        
        const int right = 60;
        const int up = 200;
        const int down = 400;
        const int left = 600;
        const int select = 800;

A sample configuration may look like this.  A HARD RESET SHOULD BE PERFORMED AFTER CHANGING ANY HARDWARE OPTIONS!!

![sample](https://user-images.githubusercontent.com/83486730/119261478-73599b80-bb8c-11eb-8edd-7658a51a13ab.jpg)


You can check both relay's for operation. For the pump/solenoid relay go to the CONTROL menu and choose PRIME. This is intended for priming the lines prior to use. The secondary relay can be activated with or without a temp sensor. While on the home screen push the LEFT button. This will activate the relay immediately if the IdleTemp is set to 0 degrees celsius. 

Next choose interval type (NONE, PURE, PROFILE). Enter the desired INTERVAL count and DURATION you want in the WETTING menu. Note that if PROFILE is chosen the INTERVAL and DURATION are ignored in favor of the Profile System.

While on the home screen press the SELECT button to start. If NONE was chosen as the interval type the pump/solenoid relay should activate immediately if there was no INITIAL DLY was entered. If PURE was chosen then random events are generated using INTERVAL and DURATION as limits. Note that the interval times are random however the number of intervals is not.

A chance outcome can be used with the WET CHANCE% under the WETTING menu. This is a percentage chance that any activation happens at all. This option wraps all options chosen for output!

Optional: A temperature sensor can be added to control the secondary relay. It can control the relay with a desired holding temp by changing the Idle Temp option in the HEAT menu. If left 0 or temp sensor is not present you can still use the secondary relay by pressing the LEFT button on the home screen to activate. All relays will turn off once the program has ended.

A flow sensor can be added to track total liquid used and also to get a sense the amount of liquid used for a specific duration. See menu description for information on setup and calibration!

Serial feedback is also available. In version v0.3.1 the serial feedback shows all generated times upon the start of a new run. It will also print the activation times for the solenoid/pump relay as they happen. Output should look something like this:

    EVEN
    I: 1, T:    0, D:10
    I: 2, T:  350, D:10
    I: 3, T:  710, D:10
    I: 4, T: 1070, D:10
    I: 5, T: 1430, D:10
    I: 6, T: 1790, D:10
    I: 7, T: 2150, D:10
    I: 8, T: 2510, D:10
    I: 9, T: 2870, D:10
    I:10, T: 3230, D:10
    --------------------
    TIME:    1, ON
    TIME:   11, OFF
    TIME:  351, ON
    TIME:  361, OFF
