Code architecture
 - The overall structure of the code follows an FSM with three states: state 0, state 1, state 2. 
 - State 0 represents the game over state
 - State 1 represents the turn state
 - State 2 represents the win state 
 - Used button interrupts
 - starts in game over state (0), 
 - button press moves it to turn state (1)
 - simon turn function plays sequence
 - checks if a button is pressed or if the wrong button is pressed, if true, go to lose state (0), 
 - if false check if turn matches sequence length, if true go to win state (2)
 - if false remain in turn state and continue sequence 



Rubric: (10 pts each (mvp is 30 pts, code is 20 pts), 130 total)

1. MVP (minimally viable product): Does it actually run properly off of a battery and (occasionally) perform in a Simon-esque way? (30 pts)
	- Runs off of battery fine and performs the Simon game well. (30)

2. Gameplay-ButtonFeedback (when a button is pressed during play, does the light and sound asssociated with that button track the button press, 
i.e., start when it is depressed and stop when it is released?)
	- When a button is pressed, light and sound continually play until it is released. Game does not progress until button is released (10)

3. Gameplay-Timeout (does it game over after a while if no button is pressed?)
	- When no button is pressed for a period of time, Simon proceeds to a game over state (10)

4. Gameplay-MisplayingError (does it game over if the wrong button is pressed?)
	- When the wrong button is pressed, Simon proceeds to a game over state (10)

5. Gameplay-Win (does it do something special when the player wins?)
	- When the right sequence is played, Simon proceeds to a win state
	- During the win state a song is played and a pattern of lights (10)

6. Gameplay-Random (does it play a different sequence each time, including after power cycles?)
	- A different sequence is played after every win/game over
	- A different sequence is played after every power cycle. (10)

7. Style-Animations (are there animations for start/win and lose/game-over?)
	- There are animations for start/win and lose/game-over
	- Simple animations with different colors (7)
	- Win song and game over song 

8. Style-SoundLight (do lights and sounds work well together and convey information?)
	- Light turns on when a button is pushed
	- Unique and consistent light for each button
	- Sound plays at the same time a button is pushed
	- Unique and consistent sound for each button (10)

9. Is it easy to change the length of a sequence required to win?
	- Easy to change the length of a sequence required to win
	- Only need to change value of variable (length) (10)'
	- default is 5

10. Well-commented, logical code architecture 
(does the architecture make sense? are there good comments?, functions (libraries?) used intelligently?) - 20 pts
	- architecture makes sense
	- functions in beginning
	- FSM in middle
	- ISR at the end
	- only one header used, music.h (15)
	- could have separated functions into different headers


Total Points 122/130

To Run:
Run both the main.c file and the music.h in one project
To play press a button to start
default run settings