HOW TO PLAY -------------------------------------------------------------------------------------------------------

     ____________________
    /     Simon 2020     \
   /                      \        B0 = Button 0 (Blue)
  |    B3            B0    |        B1 = Button 1 (Green)
  |                        |        B2 = Button 2 (Red)
  |       L3      L0       |        B3 = Button 3 (Yellow/Magenta)
  |           BZ           |        L0 = LED 0 (Blue)
  |       L2      L1       |        L1 = LED 1 (Green)
  |                        |        L2 = LED 2 (Red)
  |    B2            B1    |        L3 = LED 3 (Yellow/Magenga)
  \                       /
   \_________....________/         BZ = Piezo Buzzer
             ||||

 NOTE: Depending on the construction of your SIMON board, the colors may be different. However, the colors
 will be consistent throughout gameplay.

-------------------- 0 - IDLE --------------------

Upon startup, the game will be in idle state with the LEDs lighting up individually in a circle. Pressing
and holding one of the buttons will send the game into the corresponding state. The button's functions are
as follows:

Button 0 = PLAY - Medium Difficulty
Button 1 = CHECK HIGH-SCORE
Button 2 = PLAY - Hard Difficulty
Button 3 = PLAY - Easy Difficulty

-------------------- 1 - PLAY --------------------

A random sequence of LEDs will light up and buzz with a corresponding tone. After the chain completes, the
player must push the corresponding buttons in the order of the previously displayed sequence. The player has
a limited amount of time to push each button depending on what difficulty was selected. If the player
successfully repeats the sequence, all LEDs will flash green once. At the beginning of the game, the sequence
is of length 1. After repeating the chain, it will increase by one in length and repeat. This pattern repeats
until the player successfully repeats a chain of length equal to the victory length, at which point the player
wins and the game ends. The victory length is represented as the directive VICTORY_LEN and can be adjusted
below. However, due to the high memory utilization of this game, it cannot be made much larger and continue to
function normally.

------------------ 2 - GAME OVER ------------------

If the player enters an incorrect choice in repeating the sequence, or fails to enter a selection quickly enough
and the game times out, then the game will end and reach a game over state. The longest sequence which the player
successfully entered will be saved to the corresponding difficulty's high score if it is longer than the previously
saved value.

Depending on whether the game ends due to a victory or a failure, a corresponding animation will play. Once the
animation is complete, press any button to return to idle.

-------------- 3 - CHECK HIGH-SCORE ---------------

Upon entering Check High-Score state, all 4 LEDs will light up with their corresponding colors. The button's
functions in this state are as follows:

Button 0 = Display High-Score - Medium Difficulty
Button 1 = Return to IDLE
Button 2 = Display High-Score - Hard Difficulty
Button 3 = Display High-Score - Easy Difficulty

When displaying a high score, the LEDs will all flash the color of the corresponding difficulty a number of times
equal to that difficulty's high-score, then will return to IDLE.

