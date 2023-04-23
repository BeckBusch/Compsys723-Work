# CS723 Assignment 1 Instructions

## Set Up Board
Connect the Board to a monitor using the VGA port.  
Connect a keyboard to the board using the PS2 port.

## Program Board
The DE2-115 board must first be programmed using the Quartus programer. freq_relay_controller.sof needs to be uploaded to the board.

## Upload ELF
Using NIOS II Run configurations, ProjectFinal.elf needs to be uploaded to the board.

## Run simulation
Output of the simulation will be displayed on the VGA monitor.  
Loads forced off by the Simulation will be indicated with the green leds.  
The on/off state of loads are indicated by the red leds.  
Use Switch 0-7 to manually control the loads, and Button 3 to toggle maintenance mode.  
Use R to increase RoC threshold and E to decrease it.  
Use F to increase Frequency threshold and D to decrease it.