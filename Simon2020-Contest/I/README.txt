

*** GAMEPLAY ***
Upon turning on, the game will have a "start" animation consisting of 4 LED colors rotating through the LEDs, which will continue playing until the player presses any button. After a button is pressed, the game will immediately start. A sequence of length 6 will be played, and then when the player inputs the correct sequence using the buttons, the same sequence with one additional tone at the end will be played. This pattern will continue until the player either eventually correctly plays a sequence of (sequencelength), which can be changed easily at the beginning of the simonmain.c file, or if the player loses by playing an incorrect tone (the player will lose immediately an incorrect button is pressed). The player could also lose if they wait too long to press a button (around .5 s), in which case they lose by "timing out".

There will both be "win" animations and "lose" animations which will be the green and red LEDs flashing on the board respectively. When either the "win" or the the "lose" animation is playing, the player can exit the animation by pressing any button, which will bring them back to the "start" animation, repeating the cycle.





