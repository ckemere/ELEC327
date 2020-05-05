Architecture and approach:
Files: main.c, rgb.h, spi_led.h, notes.h, playsounds.c, playsounds.h, random.c, random.h
main.c: main while loop with Simon logic
rgb.h: rgb values for different animations to be sent using spi
spi_led.c: initializes spi on the msp430 and functions to send spi frames. Three variations: send_frame() sends a byte frame given an array, 
all_off() turn off all leds and includes the start and end frames, button_press() lights a single led based on the input (0-3)
spi.h: the header file for spi_led.c
notes.h: note values and songs for the buzzer
playsounds.c: function playsound() plays a song given an notes array, note duration array, and length of array
playsounds.h: header file for playsound.c
random.c: rand() givens a number between 0 and 31 and srand initializes the PRNG 
random.h: header file for random.c, define RAND_MAX 
Description:
Main.c handles the higher level logic for the Simon game. The initial animation plays until button press(es) is detected. Then, it goes into the main game loop
which generates a random sequence each reset, plays the sequence, and waits for player's button presses. the variable "last_button" is assigned -1 if no pressed or
waiting and assigned 0-3 when the corresponding button is pressed. The other .c files implements functions that does the grunt work like sending led frames,
generating random number, and playing sounds. rgb.h and notes.h contains values for rgb values and note frequencies, respectively, to give a cleaner main.c

Self-Evaluation:
MVP (minimally viable product): Does it actually run properly off of a battery and (occasionally) perform in a Simon-esque way? (30/30 pts)
Gameplay-ButtonFeedback (when a button is pressed during play, does the light and sound asssociated with that button track the button press, 
i.e., start when it is depressed and stop when it is released?)(10/10)
Gameplay-Timeout (does it game over after a while if no button is pressed?) (10/10)
Gameplay-MisplayingError (does it game over if the wrong button is pressed?) (10/10)
Gameplay-Win (does it do something special when the player wins?) (10/10)
Gameplay-Random (does it play a different sequence each time, including after power cycles?) (10/10
Style-Animations (are there animations for start/win and lose/game-over?) (9/10)
Style-SoundLight (do lights and sounds work well together and convey information?) (10/10)
Is it easy to change the length of a sequence required to win? (10/10)
Well-commented, logical code architecture (does the architecture make sense? are there good comments?, functions (libraries?) used intelligently?) - (19/20)
Total: 128/130

Intructions:

 *   (P2.2) (button 1) ---------  (P2.3) (button 2)
 *                     |       |
 *                     |       |
 *   (P2.0)(button 0)  ---------  (P2.4) (button 3)
 *                        ||||


Video Demo: https://youtu.be/UzslehCL43U

1. On initial startup and resets, pressing buttons 0, 1, or 2 starts game in normal mode (buttons with green led lit when startup sound is
playing), pressing button 3 starts game in hard mode (button with red led lit when startup song is playing). 
Pressing both button 0 and button 1 simultaneously activates Insane mode (very short timeout and led duration)

2. To start a new game after winning or losing, press the button or button combination in step 1

3. On hard mode only, time performance is tracked. Simon board will flash 4 white LED's rapidly to indicate best time has been
beaten. Best time is only reset after a power reset and is kept if player loses or chooses another mode in between hard mode round

4. Game length required to win can be changed with the "#define GAME_LEN x" line at the top of main.c

Notes:
1. ADC samples on P1.0. Make sure p1.0 is floating
