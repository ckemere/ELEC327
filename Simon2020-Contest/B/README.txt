ELEC 327 - Final Project: Simon+
Due: 5/4/2020

This project contains the following files that should be compiled together:
- simon.c
- random.h
- random.c
- pwm_songs.h
- pwm_songs.c
- led_spi.h
- leg_spi.c
- mazes.h
- mazes.c

How to use:
 - First compile all 9 files and load the code onto a simon board
 - On powerup the board will play a song and display the main menu
 - from this menu either Simon or Hydrathon can be started
 - there are also settings controls

Note: Colors are as I see them on my board, if your LEDs have a different RGB order than
mine then you may see different colors.

Main Memu:
     (Red/Blue)        (Orange/Green)
          (B2)-----O-----(B3)
           |     Simon     |
           |  [@]     [@]  |
          []      [/]      O
	   |  [@]     [@]  |
           |               |
          (B1)-----------(B4)
      (Yellow)    |||   (Magenta)

  - (B1) Takes you to the Difficulty submenu (Described Below)
  - (B2) Begins a game of Simon
  - (B3) Begins a game of Hydrathon
  - (B4) Mutes all sounds

Difficulty Submenu:
 There is one difficulty level for the board that is shared between both games.
 There are 4 possible difficulty levels (Easy, Normal, Difficult, and Expert).
 Normal difficulty is the default upon power-up. Winning a game will cause the
 difficulty level to increase on stage unless it is already at expert.

 (Blinking Red-Green)   (Teal)
          (L2)-----O-----(B3)
           |     Simon     |
           |  [@]     [@]  |
          []      [/]      O
	   |  [@]     [@]  |
           |               |
          (B1)-----------(B4)
        (Green)   |||    (Red)

  - (B1) Reduces the difficulty level one stage
  - (L2) This button is currently inactive, but the color of this
     light indicates the current difficulty level.
     Green - Easy, Yellow - Normal, Orange - Difficult, Red - Expert
  - (B3) Returns to the main menu
  - (B4) Increases the difficulty level one stage

How to play Simon:
- Simon will blink all 4 LEDs once and play a qick chirt to tell you the game has started
- Simon will then flash a light and play a tone that correspond to one of the buttons
- after a fraction of a second you will be able to press the buttons to light up the lights
     - the goal is to press the same sequence of buttons as simon indicates
     - for each correct sequence complete another light will be added to the end of the current sequence
- if you press the wrong button or fail to press the correct button within a short period of time then you lose
     and simon will play an annoying noise and flash angrily
- if you reach the longest sequence length (end_len in the code) and enter it without any mistakes then simon will
     play a different song to indicate you have won
- when you are in either end-state of the game pressing any button will take you back to the main menu

     The number of button presses needed to win depends on the difficulty
         - Easy requires 7
         - Normal requires 14
         - Difficult requires 28
         - Expert requires 56

How to play Hydrathon:
 - You are stuck in a maze of dunes in the dessert and need to find an oasis
 - Hold the board so diagonally so that the lights are to the left, right, top, and bottom of the center
 - To begin the game will play a falling sound and the lights will change colors rapidly, when all the lights
	go off you may begin. (Originally you can't see any special terrain so no lights will be illuminated)
 - Your character is positioned in the middle of the simon board, on the piezo buzzer 
 - Your field of view is limited by a sandstorm, you can only see a short way (one tile) in each direction
 - the colored light correspond to the following things
     - Off - this is just normal desert that you can travel accross
     - Orange - this is a dune too steep to cross
     - Red - a pitfall, stepping on this wont kill you but it will sap your stamina
     - Blue - a well, visiting these will restore your stamina (you start on one of these)
     - Green - the oasis, your destination
 - press a button to move in that direction, but keep track of where you've been
 - if you reach the Oasis a song will play and the lights will flash colorfully, pressing
         any button will take you back to the menu
 - if you run out of stamina you lose, red lights will flash, and annoying noises will play
         pressing any button will return you to the menu

     Your max stamina depends on the difficulty
         - On Easy you can move 49 times without refreshment and there is no pitfall
         - On Normal you can move 41 times without refreshment
         - On Difficult you can move 33 times without refreshment
         - on Expert you can only move 25 times without refreshment

Projected Score:
 1. MVP - The code runs on battery power and performs remakably like Simon, including having the
	same color lights in the same positions, and playig the same noted with each light
	as the original game. (Worth all 30 Points)
 2. Button Feedback - In the menus and During simon Gameplay the buttons will show their
	light and make their distinctive sound for as long as they are held down. (Worth all 10 Points)
 3. Timeout - The game will end if the next button is not pressed soon enough after the last one is 
	pressed, this can supercede a button being held down. (Worth all 10 points)
 4. Missed Input - If a button that is not the next one in the sequence is pressed the game will end.
		If the previous button is still being held down then any new button presses will not 
		be counted as either correct or incorrect. (Worth 9 or 10 points)
 5. Win State - When a game is won a song and animation are played to indicate this. It is a happy sounding
		song and the light are very colorful. (Worth all 10 points)
 6. Randomness - The sequence played in each game is different regardless of whether it occurs just after 
		being turned on or after playing a previous game. (Worth all 10 points)
 7. Animations - There are distinct and appropriate animations for winning or losing. (Worth all 10 points) 
 8. Lights and Sounds - Lights and sounds generally work well together to convey information, there are
		limitations when it comes to the menus, but substantial effort is made. (Worth all 10 points)
 9. Changing sequence length - it is easy to change the sequence length in the code (simply change the end_len
				variable in simon.c), and even possible to change the sequence length without
				changing the code by changing the difficulty level setting. (Worth all 10 points)
 10. Code - The code is extremely throuroghly commented with indications of what almost every line is doing
		and extensive function commnents. The architecture of the code is straigntforawrd, and mostly
		uses good-practive techniques. Libraries are used to separate different core functionalities.
		(Worth all 20 points)
 Bonus Additions:
 11. Menus - The addition of menus allows for the selection of different games and the implmentatiion
		of additional settings, like difficulty or muting. (Probably Worth ~5 bonus points)
 12. Difficulty Levels - I have implemented different difficulty levels for Simon as well as a menu
			to adject which one is being played. This allows for the length of the sequence
			to be adjusted without changing the code. The difficluty level also automatically
			increments after a victory to give automatic progession. (Probably worth ~10 bonus points)
 13. Adding a Second game - I designed and implemented a maze game for the simon board. It can be accessed from the
			main menu just like simon can. The game was less technically challenging to implement than
			simon, but still posed some signifiant hurdles. (Depending on how impressed you are, probably 
			worth 20-60 bonus points)
 Total Estimated Score: ~170 points
