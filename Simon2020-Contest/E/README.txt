Simon README


----------------------------

A. Code Architecture

The code is sorted into many .c and .h files so that each component required to make the Simon board work is handled separately.

>Simon-Specific:
	- main.c: 
		Initiates the setup and then starts the menu.
	- game_code.c/.h: 
		Contains all the code that is specialized for Simon, such as  the various option selection menus, Simon gameplay and the bonus piano mode.
	- simon_setup.c/.h: 
		Integrates the register setup procedures for the LEDs, buzzer, RNG, and buttons with each other so that they can be used together and don't conflict.

>General Peripheral functionality:
	- sound_player.c/.h: 	
		Contains methods for producing sound through the Piezo buzzer, and the ISR for timing sounds.
	- spi_lighter.c/.h:		
		Contains methods for controlling the LEDs.
	- songbook.h: 		
		Contains the code for the four songs used in the game, with light shows.
	- note_definitions.h: 	
		Contains the definitions for the notes that the buzzer can play in terms of their half-periods in clock cycles.
	- button.c/.h: 
		Contains the ISR that detects button presses
	- random.c/.h:
		Contains methods for seeding and using the random number generation.


In broad strokes, the user experience of the Simon board software (game_mode.c) is generally structured as a hierarchical system of menus that the user starts at the top of and goes deeper into, with the deepest level being the Simon game itself. This menu is explained in section C.

----------------------------
----------------------------

B. Score

	Based on the rubric, I believe I should get close to 130/130 points, plus some bonus credit as listed in http://elec327.github.io/bonus.

	Required:
	1. MVP: 30/30
		It plays Simon correctly and runs off of a battery.
	2. Gameplay-ButtonFeedback: 10/10 
		Buttons do start light and sound when depressed, and stop when released.
	3. Gameplay-Timeout: 10/10
		The game over screen triggers after a while if no button is pressed.
	4. Gameplay-MisplayingError: 10/10
		The game over screen triggers if the wrong button is pressed.
	5. Gameplay-Win: 10/10
		There is a win animation (green dancing lights with happy music) upon finishing the game.
	6. Gameplay-Random: 10/10
		It plays a different sequence each time.
	7. Style-Animations: 10/10
		There are four animations: one for opening a menu/booting up (blue), starting a game (white), losing the game (red), and winning the game (green).
	8. Style-SoundLight: 10/10
		The lights and sound work together to indicate which button is pressed at any given time, to perform animations, to deliver the Simon sequences to the player, to operate the piano mode, etc.
	9. Is it easy to change the length of a sequence required to win?: 10/10
		Yes, it's player-determined in the third menu (described in section C)
	10. Well-commented, logical code architecture (does the architecture make sense? Are there good comments? Functions (libraries?) used intelligently?): Depends/20
		The code architecture is well-organized and logical in my opinion, as explained in section A. I think I should get full credit for that part of this element.
		The code is mostly commented, but I wasn't able to finish commenting for game_code.c and button.c in time; however, I re-uploaded a more thoroughly commented version to Canvas late with this .txt file, which has full comments for game_code.c and button.c and some updates to the comments for other files as well (and no differences in terms of the functionality of the product). If Prof. Kemere decides to rerelease with that late version, then I think I should get 20/20; otherwise, I believe it would be fair to deduct about 5 points here for incomplete comments.

	Bonus: (methods of access described in section C of this README)

	1. Allow the user to select the level of difficulty
		This is enabled through the "general difficulty" menu that the user sees after they choose whether or not they want double and/or triple button presses. Higher difficulty levels give shorter timeout periods and faster initial sequence plays.
	2. Use the PCB for something else in addition to Simon
		There's a freestyle piano mode enabled through the main menu. 14 different notes can be played by using different combinations of buttons.
	3. Add double or triple button presses to the pattern
		This is enabled through the main menu. The user may pick single, double, or triple/quadruple button options as described in section C.

*******************************************************************************
		PLEASE NOTE: In order to allow the user time to perform the double, triple, and quadruple presses, I placed a short delay time between when buttons are pushed and when they are read (This delay is 50ms long for the double button mode, 200ms long for the triple button mode, and it doesn't exist in the single button mode). 
		This prevents multi-button presses from getting accidentally registered as single presses; however, it means that in modes with multi-button presses, the button will not immediately light up or play the corresponding sound when you push it. Please see that during single button mode, the immediate button response can be seen.
*******************************************************************************
	

----------------------------
----------------------------

C. How to access and run special features

Here's how to navigate the menus:

1. Main Menu/Button Difficulty Menu
	This is where you start on boot up, and where you return after each game.
	Three "button difficulty" settings are presented here, and the extra Piano Mode can also be accessed from here.
	If a button difficulty setting is chosen, then the user proceeds to the general difficulty menu. If the user instead chooses the piano mode, then they go into piano mode.

	a. Lower left button (blue): Single button (only one button will need to be pushed at a time during the game)
	b. Upper left button (green): Double button (two buttons may need to simultaneously be pushed during the game, in addition to the single button events)
	c. Upper right button (red): All button (two, three, or four buttons may need to be simultaneously be pushed during the game, in addition to the single button events)
	d. Lower right button (white): Access bonus piano mode, which contains 14 different notes that can be produced using combinations of the buttons. Exit this mode by pressing all four buttons at once.

2. General Difficulty Menu
	
	Four "general difficulty" settings are presented here. This refers to the speed of the game: how quickly the game tells you the sequence it wants you to play, and how quickly you have to respond before the timeout kicks in.
	If a button difficulty setting is chosen, then the user proceeds to the sequence length menu.

	a. Lower left button (cyan): Easy
	b. Upper left button (yellow): Medium
	c. Upper right button (purple): Hard
	d. Lower right button (red): Impossible (not really impossible)

3. Sequence Length Menu

	Four options for the length of the game sequence needed to win are presented here.
	The game will start after this final menu selection is made.

	a. Lower left button (dimmest red): Length 5 sequence
	b. Upper left button (dim red): Length 10 sequence
	c. Upper right button (bright red): Length 15 sequence
	d. Lower right button (brightest red): Length 100 sequence
		Note: it may take a while to load the length 100 sequence depending on how long it takes for the game to find 100 usable random numbers. The animation with white lights will let you know when the game is about to begin.




----------------------------