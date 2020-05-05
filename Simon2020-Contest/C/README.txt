ELEC 327 Midterm/Final Project: SIMON

a) Code first initializes loop variables, LED arrays used, and state variables, along with initializing I/O's on the board. The game operates off of two arrays: sequence, which contains the randomly generated "key", and inputs, which stores player button presses each turn as they are input.

- Upon startup, sequence is filled with random values (0-3) which will correspond to the button/LED pairings on the board. Random values are seeded from an ADC read (5 bit state) and shuffled using LSFR of length 5 and taking the 2 LSBs to be used in sequence. For increased variance, the LSFR algorithm is performed 3 times for each value placed into sequence.
- The board then enters standby, where the loss animation (LEDs turning on red clockwise) is played until a button is pressed. Once any button is pressed, the standby state variable goes low and the main game begins.
- A turn consists of simon's turn - displaying of the sequence one by one - and the players turn - player mimics the sequence back. Each LED/button pairing has an associated tone of the buzzer, which is set by changing CCR0/CCR1 values for TA1. Navigation between and within simons turn and the players turn are controlled by respectively named state variables, and if the player makes an incorrect input or the timeout of 2 seconds triggers (TA0) with no player input, then the board immediately returns to the standby state, playing the same loss/startup animation and loading a new sequence ready to play again.
- If the player reaches the max turn limit without any mistakes, then they win and the win animation is played. The board remains playing the win animation until power cycled

b)
MVP: 30
Button feedback: 10
Timeout: 10
Misplay Error: 10
Win: 10 (gameplay wise)
Random: 10
Animations: 10? (as a style maybe not full 10)
SoundLight: 10
Change Length of Sequence: 10
Well commented / Code Architecture: 15~20

Total: 120/130 ~ I could have done more to improve it (add sounds to animations or condense some repeated code into modular library/functions) but met all the specifications of a simon game with good functionality.

c) To change the length of the sequence required to win, change the ROUNDS variable found on line 4 of the main.c