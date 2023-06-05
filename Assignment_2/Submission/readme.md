# CS723 Assignment 2 - Cruise Control in Esterel

Program files can be found in /src

Compiling the code requires Esterel to be installed, along with Xeve for simulation.

Code can be compiled with 

```
cd src
make clean
make CruiseControl.xes
```
and simulated by running
```
./CruiseControl.xes
```

## Inputs
### Pure Bool
 - On -- Turns the Cruise Control system on
 - Off -- Turns the Cruise Control system off
 - Resume -- Resumes the Cruise Control system after braking
 - Set -- Attempts to set the Cruise Control speed to the car's current speed
 - QuickAccel -- Attempts to decrease the Cruise Control speed by a set amount
 - QuickDecel -- Attempts to increase the Cruise Control speed by a set amount
### Float
 - Accel -- How far the accelerator pedal has been pressed
 - Brake -- How far the decelerator pedal has been pressed
 - Speed -- How fast the car is currently going
## System Parameters
 - SpeedMin - 30.0 km/h - Min speed for cc operation
 -  SpeedMax - 150.0 km/h - Max speed for cc operation
 -  SpeedInc - 2.5 km/h - Push button speed increase
 -  PedalsMin - 3.0 % - Sensitivity of car pedals
 * Constants for car speed regulation algorithm and control
 -  Kp - 8.113
 -  Ki - 0.5
 -  ThrottleSatMax - 45.0 %

