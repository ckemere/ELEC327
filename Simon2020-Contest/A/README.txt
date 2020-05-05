
README.txt

a) briefly describe your code architecture and approach

I used an FSM structure to create the different game states and tried my best to use functions from previous homeworks instead of creating new ones. There are five different states corresponding to START, DISPLAY, SIMON, GAME_OVER, and WIN. The transitions between states are dependent on pushing a certain button or sequence of buttons for every state.

Interrupts are used to detect button pushes as well as implement the gameplay timer.

notes.h and playsounds.h contain everything needed to play songs on the buzzer.

setup.h contains all setup functions for initializing essentially everything.

random.h contains all functions for generating random numbers.



(b) the score that you think you should get based on the rubric below

Based on the rubric, I should get at least 110/130. I filled out the rubric below.



(c) how to access or run any special features

Game begins with start sequence, push any button to continue. 

The simon game begins. Simon sequence will display and then user must push the same sequence of buttons.

Any error or waiting more than 30 seconds during gameplay will result in Game Over. Push any button to restart game.

No errors will results in a Winner Mode. Push any button to restart game.

note: songs must finish before each state change in START, WIN, and GAME_OVER. Does not require extra button pushes, just wait a second or so.

-------------------------------------------------------
RUBRIC:

DONE?
Yes     MVP (minimally viable product): Does it actually run properly off of a battery and (occasionally) perform in a Simon-esque way?

maybe   Gameplay-ButtonFeedback (when a button is pressed during play, does the light and sound asssociated with that button track the button press, i.e., start when it 
		is depressed and stop when it is released?)

YES      Gameplay-Timeout (does it game over after a while if no button is pressed?)

Yes     Gameplay-MisplayingError (does it game over if the wrong button is pressed?)
 
Yes     Gameplay-Win (does it do something special when the player wins?)
        
Yes     Gameplay-Random (does it play a different sequence each time, including after power cycles?)

Yes     Style-Animations (are there animations for start/win and lose/game-over?)

Yes     Style-SoundLight (do lights and sounds work well together and convey information?)

No      Is it easy to change the length of a sequence required to win?
        
Yes     Well-commented, logical code architecture (does the architecture make sense? are there good comments?, functions (libraries?) used intelligently?)

