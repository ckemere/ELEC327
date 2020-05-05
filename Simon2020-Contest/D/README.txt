README.txt

(a) Code Architecture and Approach (see comments for more details)
1) Initialization
	a) Initialize timers, clocks, and modules.
	b) Set the difficulty of the game.
	c) Define the victory/loss songs and store them in flash memory.

2) Startup
	a) Display an animation consisting of randomly lit LEDs.
	b) Decide what the first input position should be.
	c) Once the player pressed a button, go onto Display.
3) Display
	a) Display each press necessary for the player to succeed.
	b) Once all the necessary presses have been displayed, move on to Input Entry.
4) Input Entry
	a) Wait for a button press from the player.
	b) If the player takes too long, end the game.
	c) Once the player presses and releases a button, move on to Input Evaluation.
5) Input Evaluation
	a) Decide whether the input matches what the player should have entered.
	b) If the input is satisfactory, decide whther the player should 
		go to the next round or win the game.
	c) If the input is unsatisfactory, end the game.
6) Win
	a) Play a win animation consisting of turning all LEDs on and off frequently.
	b) Play a song in the background.
	c) Once the song ends, transition back to startup.
7) Game Over
	a) Play a game over animation consisting of turning off all LEDs, one-at-a-time.
	b) Play a song in the background.
	c) Once all LEDs are off, go back to startup.

(b) Rubric Score
1. Simon runs off battery and performs as intended. 
2. Each combination of buttons has its own sound and light display.
3. The game ends if the player takes too long.
4. The game ends if the player enters the wrong button(s).
5. The game performs a win animation, with sound, when the player wins.
6. The LED sequences are random, and they do change after power cycling.
7. There are animations for startup, lose, and win.
8. The lights and sound work together to convey game state and button presses.
9. Required sequence length can be changed by modifying the MAX_LENGTH constant.
10. Code is well-commented and uses several modular libraries.

(c) Modifications and Special Features
MAX_LENGTH determines the sequence length required to win.
DIFFICULTY determines (on a range from 1-4) how often double-button presses appear.