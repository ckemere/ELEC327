ELEC 327 SIMON Final Project



Hello, and welcome to my SIMON Project!

To run my code, make sure that you have my main.c, leds.h, play_sound.c, play_sound.h, random.c, and random.h files all in the same project.

CODE ARCHITECTURE----------------------------------------------------------------------------------

The main code architecture includes:
	-leds.h file
		-sets up the LED arrays needed for game animations and button inputs
	-random.c file
		-implements random number generation to create a random sequence for SIMON game
	-random.h file
		-header file for random function that sets values for length of random sequence
	-play_sound.c file
		-increments through array of notes and rests needed for SIMON game 
	-play_sound.h file
		-header file for play_sound that sets frequencies for various notes and assigns notes to each color 
	-main.c file
		-sets values for different LED color states and game states
		-uses play_sound function to create victory animation, failure animation, and starting animation
		-sets up SPI, watchdog timer, ADC sampling, timer for play_sound, and pins for button inputs and outputs
		-assigns a sound to a corresponding colors and sets up LEDs on SIMON board
		-reads button presses in as input and creates state machine to verify whether correct button was pressed
		-assigns random number generated to corresponding color
		-in main: 
			-sets up high score function
			-starts SIMON board at starting animation
			-starts game based on chosen difficulty level
			-goes into failure mode if wrong color is chosen or times out
			-goes into victory mode if end of color sequence is reached
		
GAME LOGISTICS----------------------------------------------------------------------------------		
		
In main.c, you can set how many levels you would like to set the SIMON game for on line 55 of main.c. Please note: recommended max length is 5. 

Once you have run the code, you can start playing the SIMON game!
The game starts with the starting animation, at which you have one of four choices:

	-Easy Level: to choose easy difficulty level, press the bottom right button on the SIMON board. 
		You will have four seconds to input each color that 'SIMON' tells you. This resets after a new level is reached or a button is pressed. 
		
	-Medium Level: to choose medium difficulty level, press the bottom left button on the SIMON board. 
		You will have three seconds to input each color that 'SIMON' tells you.This resets after a new level is reached or a button is pressed. 
		
	-Hard Level: to choose easy difficulty level, press the top right button on the SIMON board. 
		You will have one second to input each color that 'SIMON' tells you. This resets after a new level is reached or a button is pressed. 
		
	-High Score Mode: high score mode allows you to view the highest level you have reached at each difficulty level while the board has been running. 
				 to choose high score mode, press the top left button on the SIMON board. 
				 Next, choose which difficulty level high score you would like to view. 
				 
				 For example, by pressing the top right button, I choose to view the hard difficulty high score. The SIMON board would flash that color
				 for as many levels I reached in that difficulty level. If I were to have lost on the fourth level in the previous turn, the board would 
				 flash four times, accordingly. 
				 
				 By pressing the top left button, you can return to the starting animation. 
			
If you have chosen a difficulty level, the board will flash that color and the game will begin!
For each level you pass correctly, the board will flash all LEDs green once (or blue, depending on your board layout)

If you choose the wrong color or the game times out, the game will go into a failure animation! :(
From here, you can press any button to return to the starting animation and try again.

If you are able to get through all of the levels correctly, the victory animation will play! :D
From here, you can also press any button to return to the starting animation and play again. 

Have fun!


I have created a youtube video to showcase the SIMON Project, found at this link: https://youtu.be/ATg1Zwa2diM

SCORE-----------------------------------------------------------------------------------------------

MVP (minimally viable product): 30 pts
Gameplay-ButtonFeedback: 10 pts
Gameplay-Timeout: 10 pts
Gameplay-MisplayingError: 10 pts
Gameplay-Win: 10 pts
Gameplay-Random: 10 pts
Style-Animations: 10 pts
Style-SoundLight: 10 pts
Is it easy to change the length of a sequence required to win: 10 pts
Well-commented, logical code architecture: 20 pts

Bonuses: created youtube video, added high score mode, and allows user to choose difficulty of game. 

TOTAL POINTS: 130 PTS + BONUS



