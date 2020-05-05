ELEC 327 Simon Final Project
05/04/2020

A. Code architecture and approach

General Structure:
Global variables for the games are defined prior to the main function. 
Code is organized by first calling several functions, defined in 
setup.c, to properly set up SPI, ADC, and other modules. Then a while 
loop begins. Pressing button 1 begins a whack-a-mole game and pressing 
button 2 begins Simon (game rules described in simon.c). Pressing 
button 3 (4) increases (decreases) game length, which has a maximum 
of 10 and minimum of 1. 

Button Presses:
Button presses are recorded using the watchdog timer interrupt, which 
sets or clears flags for each button. To prevent long button holds from
registering as double presses, a while loop begins with each button 
press, which is broken once the corresponding flag is cleared. In the 
games, appropriate logic is set within this loop (checking game failure 
conditions, number of button presses, etc). Also, timeout mechanics are
handled by a counter in the watchdog interrupt.

Diagram:
       SIMON
 _________________
|button2   button3|
|      buzzer     |
|button1   button4|
 _________________    
       PORTS

Modules:
Three code modules are used outside of simon.c. Setup.c contains code to
configure USCI, ADC, buttons, buzzers, and the watchdog interrupt. random.c
contains code to generate random numbers as well as ADC code to generate a 
random 16 bit seed. playSound.c contains code for game over, start, winner,
and other animations and sounds. 

Games:
Games are activated by button press. First, a random seed is generated each
game. Then, the rand() function generates random patterns for the game. Then,
a for loop is used to properly execute the game (playing the correct pattern
to be copied). A while loop nested within this for loop contains button logic 
and also tests for failure conditions. 

B. Score

1.  MVP - 30 pts, game works appropriately.
2.  Gameplay - Yes, buttons are functional as desired.
3.  Gameplay - Yes, time out works, and retriggers after each button press.
4.  Gameplay - Yes, misplaying error works properly.
5.  Gameplay - Yes, winning animation present.
6.  Gameplay - Yes, animations are random.
7.  Style - Yes, I have animations for start, game-over, and winning.
8.  Style - Yes, animations convey info (red for game over, green for win, 
    rainbow for start)
9.  Yes, easy to change length via button presses (buttons 3 and 4). To increase 
    max game length, change PatternLength to the desired max (currently 10). 
    This line is located before main(), where all global variables are defined. 
10. Well, commented, architecture makes sense, libraries are used - 20 pts.

C. Special features. 

No special features are meant to be accessed by changing the code (you can change 
max game length by changing the PatternLength variable though, as described in
part B). 

Pressing button 1 starts the whack-a-mole game. Button 2 starts simon. Buttons 3 (4) 
increase (decrease) game length by 1 for each button press. 

Button software debouncing is working. (Recieved board with capacitors removed). Works
because the watchdog timer polls the buttons, setting appropriate flags for buttons pressed
or not pressed. The interval between WDT interrupts is longer than the transient period 
in button presses. (Tested by implementing counter within the WDT interrupt. No change in
functionality is noticed). 
