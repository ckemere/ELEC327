A) I have 3 .c files, main, setup, and animations.
setup.c has functions that set up all the timers, SPI, buttons, buzzer, and ADC.

Animations.c has the led animation functions like flashled(), turnonled(), turnoffled(), and the animations for losing and winning. Flashled(), turnonled(), and turnoffled() all take an integer as input which corresponds to which led is to be changed.

Main.c initializes game variables, and has the main gameplay while loop. In the main() function, the functions from setup.c are called. The infinite while loop in main first checks if the game has been won or if the game has not been started and plays the corresponding animation functions from animations.c in a while loop until a button is pressed. When a button is pressed, the game starts and the leds are flashed up until the round the player is currently on( i.e. in the first round the first led is flashed etc.) then it is the player’s turn. The button pressed is detected and checked against what it should be. If it is right, the turn continues until the player has correctly pressed the amount of buttons equal to what round it is. If it is wrong the game is reset and the game over animation plays. For every cycle the player does not press a button, a counter is incremented and when that number passed a threshold, the game times out and ends.





B) I would give myself a 120 because I could not get the sound to work while maintaining that the gameplay also works. The code I have now for playing sounds is commented because when uncommented, the game will sometimes not run after the second round. 

1. 30/30
2. 5/10 - lights comes on corresponding to the button press, but not sound
3. 10/10
4. 10/10
5. 10/10
6. 10/10
7. 10/10
8. 5/10 - lights comes on corresponding to the button press, but not sound
9. 10/10
10. 20/20




C) To change the length of the game, change line 5 “#define gamelength __” to the desired gamelength.
