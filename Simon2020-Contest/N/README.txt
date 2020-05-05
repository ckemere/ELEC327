ELEC 327 Simon Game Final Project:
Submitted May 3, 2020


Hello grader! In this document you will find the list of files associated with this project, the grade I believe
I should receive based on the rubric, and a brief description of my approach. There are no special capabilties included.

List of Files:
       The header comments included in each of the .c's will be included below for a summary
1. animations.c/.h
2. game.c/.h
3. initialize.c/.h
4. lights.c/.h
5. main.c
6. random.c/.h

Grade based on rubric:
130/130

It does run off of a battery in a simon-esque way, with a button press the corresponding light and sound occur for
and equivalent amount of time. It times out if no buttons are pressed, and plays a start, win, and lose animation
and the length of the sequence can easily be changed as one of the first variables in the main.c. The code has been
thoroughly commented, and I believe it is logical.

Description of approach:
At the very highest level, my code runs as a state machine in one of 4 states (0 - start, 1 - game, 2 - win, 3 - lose)
and each state has a corresponding function to dictate what happens during it. Additionally, it switches state in one 
of two ways. State 0 goes to state 1 via a button press ISR, state 1 goes to either state 2 or 3 based on the game
outcome, and states 2 and 3 go back to 0 via another button press ISR.

Within states 0, 2, and 3, there are functions that run animations until a state change. These call on light and sound
functions specified in other .c files. The light uses the UCI functionality of the msp430, and the sound uses PWM with
a Piezzo buzzer. The color and sound are set via the functions, and then the MSP430 is put into LPM3 until the watchdog
timer fires to move to the next color.

State 1 is more complicated and is housed in its own game.c file. The playgame function first generates the full sequence
that'll be played using a pseudorandom number generator (more specifically, a 5-bit LFSR seeded from sampling the ADC on 
an unattached pin) in random.c. It stores this sequence of 2 bit numbers (0-3 for each of the buttons) and also generates
a corresponding sequence of the sounds associated with the buttons. It then plays the first n values (light and sound) of 
the sequence. Then after it plays all the values, it runs a checking for loop once for each of the n values. This calls another
function (recordpress) which notes the value of the button pressed. The loop carries on to the next value in the sequence (or
increments n up by 1 if the player has correctly matched the whole sequence). If the player gets to the end of the sequence 
(checked by ending the sequence with a -1), the win state is returned. If the player ever presses a button that doesn't match, 
the lose state is returned.

Finally, the recordpress function runs by continually checking if buttons are pressed. If nothing is pressed, a value of -1 is 
assigned to currentpress, which is then passed to previouspress. If any button is pressed, that corresponding button number is 
assigned to currentpress, and the sound and light for that button are played. If previouspress and currentpress don't match, a 
button has either been pressed or released, so the function checks for them being different, and for previouspress not equalling 
-1, and then returns the value of previouspress. This ensures it returns the value of the last button pressed when it is released.
Every cycle, a timeout counter increments so that if no buttons are pressed, it returns -1 to induce a fail.