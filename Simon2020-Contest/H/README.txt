**IMPORTANT NOTE FOR GRADERS: To change the length of the sequence required to win, change the variable n_rounds, it is in the first line of the main function in the main c file. 
				I was not sure on what it meant to make this length easy to change, but given that it is the first line in the main file, I consider this easy to change. 

(a) Approach / Code Architecture:
	The approach I took to writing my code was to format the game like a state machine using state flags (int variables set to either 0 or 1, one of them can also 
	be set to 2).Various flags are initialized so that the game begins at a"Start Sequence" state. In this state, an animation and music plays while the player has 
	four options controlled by the buttons : 3 difficulty levels for the Simon Game (Easy, Medium, Hard) and a 4th bonus option to play a special song and light 
	animation. Depending on which button is pressed (or if no buttons are pressed and the game time outs), the state flags are modyfied so that either the "Gameplay"
	Sequence begins or the special music player sequence begins or the game ending sequence begins (will go to game ending if start sequence timeout occurs). 
	
	The Gameplay Sequence will go on until all rounds have been complered correctly (rounds are played using a nested for loop, as a round consists of a sequence of
	color LED patterns, each time incrementing by one until the full sequence is played or an error is made). The flag checking if the correct move has been made keeps
	the game going until it is completed or a mistake is made. Based on how the gameplay sequence ends, the win , lose, and end sequence flags are set so that either
	the winning end sequence or the losing end sequence is executed. 
	
	The End Sequence uses the win and lose flags to determine which animation should be played (game won or game lost). 


 (b) Score : 	Based on the Rubric, I think I should receive a 120/130 score, as my random number generator is not working, so the gameplay sequence is not unique upon each
		time the game is played (-10). I think I could have written a couple more functions as my main function is quite long, so I would dock 5 points off my 
		code architecture points. However, I did include the game difficulty mode feature and music player(can select easy medium or hard based on which button 
		is pressed at thestart, and you can also play a special song with a new animation), so I would add 5 bonus points. 


(c) Special Features :	When the start sequence / start song is playing, depending on which button you press you will get different gameplay modes or a music player mode. 

				/* BUTTON LAYOUT */
   WHITE LED: MUSIC PLAYER	/* (P2.2) S2 --------- S3 (P2.3) */	BLUE LED : EASY GAMEPLAY MODE
    				/*         |            |        */
    				/*         |            |        */
   RED LED: HARD GAMEPLAY MODE	/* (P2.0) S1 --------- S4 (P2.4) */	ORANGE LED : MEDIUM GAMEPLAY MODE




    