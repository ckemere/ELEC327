Part A: Architecture
-------------------------------------------------------------------------------
My code architecture is based around the play_simon() function in simon.c.
the code first sets up the game, initializing button press interrupts as
well as game state variables. Then, the function plays a starting
animation until the player presses (and releases) a button. Then, the
code will play a light sequence for the player to duplicate with button
presses, starting with length one and going from there to the value of 
MAX_LENGTH (defined in config.h).

My code's game state is built around waiting for the state of the the button
presses to change in LPM3. My code will wait for buttons to be pressed by
checking the value of the BTN_STATE variable, which has bits 0,1,2, and 3 set
depending on which buttons are pressed. If a timeout is required, the 
GAME_STATE variable will be updated to reflect that, and it will be checked
along with the BTN_STATE variable.


Part B: Score
-------------------------------------------------------------------------------
1. (30/30 pts) my code does run off of a battery, and performs in the simon-esq 
    manner. it starts with a game start sequence, waiting for the player to 
    press a button to start the game, then it plays increasingly long sequences 
    of LEDS, checking the player for correct button presses after each 
    sequence. If the player presses a wrong button or waits too long to press 
    one, it plays a game loss animation, and resets the game after a button 
    press. If the player successfully presses MAX_LENGTH buttons in a row, 
    the game plays a win animation, and resets the game after a button press.

2. (10/10 pts) The light for the relevent button does illuminate, and a sound 
    plays while the button is pressed. the function btn_effects() makes these 
    effects based on the value of BTN_STATE passed to it. The variables
    buttonX_tone and buttonX_led in game_data.c control the colors and sounds
    for the buttons.

3. (10/10 pts) The timeout is implemented via the usage of GAME_STATE and
    the watchdog timer interrupt. Change TIMEOUT_DELAY (in config.h) to
    change the length of this timeout. loss_animation() shows the
    game over animation.

4. (10/10 pts) The code moves to game loss state and calls loss_animation()
    if the value of BTN_STATE does not match the expected button press
    after a button is pressed. The result is that when a button is released,
    the game loss animation will play if it was the incorrect button.

5. (10/10 pts) If the player does not press a button wrong and i reaches the 
    value of GAME_LENGTH, the function win_animation() plays and makes green 
    leds, with the "mario" theme. (and randomly interspersed red LEDs)

6. (10/10 pts) The gameplay sequence is random. This is accomplished by making
    a random seed in setup_game(), which is called after game loss or after a 
    power cycle, and resetting the seed via srand() to this value every time 
    the same random sequence must be created for checking the player pattern or
    displaying the challenge sequence of leds for them to press.

7. (10/10 pts) There are animations. See win_animation() and loss_animation()
    win animation was described in part 5, and loss_animation() plays flashing
    red lights.

8. (10/10 pts) The sounds are designed to convey success and failure. The
    win audio is upbeat, and the loss audio sounds like a warning. The red
    flashing leds show loss clearly, while the green leds show a winning
    result.

9. (10/10 pts) Yes, just change GAME_LENGTH in config.h. Alternatively, see
    part C for a way to change the game length on the fly using an options
    menu.

10. (20/20 pts) I feel that my code was well commented and well laid out. I
    think good evidence of this is that I was able to reuse much of my code
    from prior labs without significant modification, due to it being in 
    library format.


Part C: Special Features
-------------------------------------------------------------------------------
My board includes a special options menu accessible during the startup 
sequence. It lets you change the game speed, length, and disable or enable
audio. These settings persist until the next power cycle.

OPTIONS MENU USAGE INSTRUCTIONS:

Part 1: Entry
To enter the options menu, wait until you are on the startup screen, which
can be recognized by the cycling rainbow LEDs and the pacman theme playing.

THEN, hold multiple buttons on simon for 3 seconds. You'll know you've reached
the options menu when all leds blink white.

THEN, wait for the options menu leds to flash blue, red, green, and yellow.
At this point the menu is ready for your selection.

Part 2: Option selection
The button functions are diagrammed below. Note this is with reference to the
"Simon 2020" text being at the top of the board.

       S2 (BLUE): --------- S3 (YELLOW)
               |            |        
               |            |        
       S1 (RED) ----------- S4 (GREEN)

The buttons do the following:
S2: BLUE- Audio
    The BLUE button toggles audio on and off. It plays a upwards tone when
    enabling audio, and descending one when disabling it.
S3: YELLOW- Game Speed
    The YELLOW button changes the game speed. See INCREMENT MENU below for
    how to use the menu this button launches.
S4: GREEN- Game Length
    The GREEN button changes the game speed. See INCREMENT MENU below for
    how to use the menu this button launches.
S1: RED- Exit
    The RED button exits this menu. After pressing, starting animation will
    restart.

Part 3: Saving
The options menu automatically stores user settings into RAM by changing
revelant variables. The menu has stored the current setting and is ready for
new input once the leds flash white, and the multicolored leds reappear.

INCREMENT MENU
The increment menu allows you to increase or decrease game variables
in steps. The button layout is described below
(with "Simon 2020" text upwards):

              S2 ---------  S3
               |            |        
               |            |        
              S1----------- S4

S2 and S1: Exit
    These buttons exit the increment menu.

S3: Increment
    This button raises the value being modified
S4: Decrement
    This button lowers the value being modified

Increment Menu Value Display:
The increment menu shows the current value of the setting by which leds
are illuminated bright white (dimly lit white LEDs are not enabled). The
menu also plays a chirp tone when incrementing or decrementing the value of
a variable, as many times as it actually raises the value (so if the value is
raised by 2 for every button press, the chirp will play twice)
