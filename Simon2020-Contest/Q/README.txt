This simon board works under the rules in the project description:

	"On the nth turn, the device plays the first n elements of the sequence.
	 So one tone/LED on turn one, two on turn two, etc. The pattern of the 
	 n-1 elements of the sequence are the same as the previous turn. Each turn, 
	 after the sequence is played, the player must push the appropriate buttons 
	 to recreate the sequence. If they do it correctly, the game proceeds to the 
	 n+1th turn. If they make an error or wait too long (measured by the time 
	 since the last button press), they lose. Winning corresponds to playing the 
	 full M-element sequence. If the player loses, the device should play a 
	 “Game Over - Lost” animation that continues until it is reset. Similarly, if 
	 the player wins, the device should play a “Game Over - Won” animation."

The game will show a sequence of lights in which a sound of a frequency corresponding to
the specific led lit while it is lit. When pressing the button after the sequence is shown,
it will light up and play its note as the button is pressed and play until it is let go.
There are 3 different light sequences. First, when a button is pressed after winning/losing
or before the game has been played, a startup light sequence will light up each led in a
circle on the board to show where each color is at. Then the game will start. When you lose,
each led will light up red in a circular pattern after a gameover sound sequence is played.
The similar happens when you wine, but it will light up white instead and play the winning
sound sequence.

Problems with the board:

The board seems to draw too much power to be run on its own and the following error appears
"Warning: On-board LDO is now used to supply the target with power". Therefore, the game only
works when plugged in and not on batteries.

Rubric:
1. MVP: Everything works but points should be taken off since the game doesn't run off of
	a battery
2. ButtonFeedback: Full points, the sound/light starts as button pressed and turns
	off as it is let go.
3. Timeout: Full points
4. MisplayingError: Full points
5. Win: Full points
6. Random: Full points
7. Animations: Full points
8. SoundLight: Full points
9. At the very top of the code there are two things to change, the length of an array and a
	variable "game_length". After that the game will run perfectly at the length
	requested. Maybe a point or 2 off since this process could be simplified a bit
	if I had more time.
10. Full points, or close to full points, code has been structured to look clear and well
	commented out.
 
