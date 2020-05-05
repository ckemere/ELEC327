#include <msp430.h>
#include "simon_setup.h" // All the setup for Simon board is stored here.
#include "sound_player.h" // sound_player.c contains the PlaySound function.
#include "songbook.h" // The arrays that define songs are stored here.
#include "random.h"
#include "spi_lighter.h"
#include "button.h"
#include "game_code.h"


//These variables help with managing the WDT timeout (explained in the ISR)
int alloted_time;
int WDT_counter;
int timer_burned;

//This is the code for piano mode, which allows the player to freely play musical notes on the buttons. Multiple button presses at once produce different sounds than single ones. Ascending from Button1 -> Button1 + 2 -> Button2 -> Button2 + 3 -> etc. produces a C major scale.
void piano(void) {
    ClearLEDs();
    //I use this type of dont_quit variable frequently to decide whether or not to keep going through the code.
    int dont_quit = 1;
    while(dont_quit) {
        switch(button_pressed) {
            case NO_BUTTON:
                StopNote();
                ClearLEDs();
                break;
            case BUTTON1:
                PlayNote(C4);
                ColorLED(1, 0, 0, 0x64);
                break;
            case BUTTON2:
                PlayNote(E4);
                ColorLED(2, 0x64, 0, 0);
                break;
            case BUTTON3:
                PlayNote(G4);
                ColorLED(3, 0, 0x4E, 0x16);
                break;
            case BUTTON4:
                PlayNote(B4);
                ColorLED(4, 0x21, 0x21, 0x21);
                break;

            case (BUTTON1 + BUTTON2):
                PlayNote(D4);
                ColorLED(1, 0, 0, 0x64);
                ColorLED(2, 0x64, 0, 0);
                break;
            case (BUTTON2 + BUTTON3):
                PlayNote(F4);
                ColorLED(2, 0x64, 0, 0);
                ColorLED(3, 0, 0x4E, 0x16);
                break;
            case (BUTTON3 + BUTTON4):
                PlayNote(A4);
                ColorLED(3, 0, 0x4E, 0x16);
                ColorLED(4, 0x21, 0x21, 0x21);
                break;
            case (BUTTON4 + BUTTON1):
                PlayNote(C5);
                ColorLED(1, 0, 0, 0x64);
                ColorLED(4, 0x21, 0x21, 0x21);

                break;
            case (BUTTON1 + BUTTON3):
                PlayNote(Eb4);
                ColorLED(1, 0, 0, 0x64);
                ColorLED(3, 0, 0x4E, 0x16);
                break;
            case (BUTTON2 + BUTTON4):
                PlayNote(Gb5);
                ColorLED(2, 0x64, 0, 0);
                ColorLED(4, 0x21, 0x21, 0x21);
                break;

            case (BUTTON1 + BUTTON2 + BUTTON3): // All buttons except 4
                PlayNote(D5);
                ColorLED(1, 0, 0, 0x64);
                ColorLED(2, 0x64, 0, 0);
                ColorLED(3, 0, 0x4E, 0x16);
                break;
            case (BUTTON2 + BUTTON3 + BUTTON4): // All buttons except 1
                PlayNote(E5);
                ColorLED(2, 0x64, 0, 0);
                ColorLED(3, 0, 0x4E, 0x16);
                ColorLED(4, 0x21, 0x21, 0x21);
                break;
            case (BUTTON3 + BUTTON4 + BUTTON1): // All buttons except 2
                PlayNote(F5);
                ColorLED(1, 0, 0, 0x64);
                ColorLED(3, 0, 0x4E, 0x16);
                ColorLED(4, 0x21, 0x21, 0x21);
                break;
            case (BUTTON4 + BUTTON1 + BUTTON2): // All buttons except 3
                PlayNote(G5);
                ColorLED(1, 0, 0, 0x64);
                ColorLED(2, 0x64, 0, 0);
                ColorLED(4, 0x21, 0x21, 0x21);
                break;

            case (BUTTON1 + BUTTON2 + BUTTON3 + BUTTON4): // Press all four buttons at once to quit
                dont_quit = 0;
                ColorLED(1, 0, 0, 0x64);
                ColorLED(2, 0x64, 0, 0);
                ColorLED(3, 0, 0x4E, 0x16);
                ColorLED(4, 0x21, 0x21, 0x21);
                break;

            default: // I'm pretty sure you can't reach this state
                break;
        }
    }
    StopNote();
    timerDelay(12000);
    //Give it a little time before exiting to create a clear break among modes.

}

//This function starts the timeout timer, which uses the WDT. It starts when a player releases a button during gameplay.
void startTimer(int time_to_give){
    WDT_counter = 0;
    alloted_time = time_to_give;
    WDTCTL = WDT_ADLY_16;
    IE1 |= WDTIE;                             // Enable WDT interrupt
}

//This function stops the timeout timer, such as when pressing a button or quitting the game.
void resetTimer(void){
    WDT_counter = 0;
    WDTCTL = WDTPW + WDTHOLD;                // Stop WDT
    IE1 &= ~WDTIE;                           // Disable WDT interrupt
}


//This is the main function that controls the game of Simon.
void simon(int difficulty, int button_difficulty, int game_length) {

    StopNote();
    ClearLEDs();

    timer_burned = 0;

    //The literal number of difficulty is how many entries are in the Simon game sequence
    //PlaySong(victory_fanfare_intro_notes, victory_fanfare_intro_durations, victory_fanfare_intro_colors, victory_fanfare_intro_length, 18000, FINITE, UNSTOPPABLE);

    unsigned int game_timeout_duration;
    unsigned int sequence_idx = 0;
    unsigned int replay_sequence_idx = 0;
    unsigned int answer;

    //Deciding game settings
    switch(difficulty) {
        case EASY:
            game_timeout_duration = 125; //2 seconds after each button press for easy
            break;
        case MEDIUM:
            game_timeout_duration = 47; // 0.75 seconds after each button press for medium
            break;
        case HARD:
            game_timeout_duration = 31; // 0.5 seconds after each button press for hard
            break;
        case IMPOSSIBLE:
            game_timeout_duration = 20; // 0.333 seconds after each button press for impossible
        default:
            game_timeout_duration = 47; // 0.75 seconds after each button press for medium
            break;
    }

    int button_options;
    int bits_needed;
    switch(button_difficulty) {
        case SINGLE_BUTTON:
            button_options = 4;
            bits_needed = 2;
            break;
        case DOUBLE_BUTTON:
            button_options = 10;
            bits_needed = 4;
            break;
        case ALL_BUTTON:
            button_options = 15;
            bits_needed = 4;
            break;
        default: //Just in case the button difficulty didn't make it in somehow, the default case is single button usage.
            button_options = 4;
            bits_needed = 2;
            break;
    }

    songGameStart();

    //Reseeding the RNG after every game to add additional entropy
    ADC10CTL0 |= ENC + ADC10SC;                     // Sampling and conversion start
    __bis_SR_register(LPM3_bits + GIE);             // Wait in sleep until the ADC has finished its conversion of the P1.6 value, then run the ISR (see "ADC10 Timer interrupt service routine" comment)
    __bis_SR_register(GIE);                         // Default behavior of my ISR is to disable interrupts on its way out, so I reverse that immediately after.


    //Preparing the game sequence
    volatile int game_sequence[100]; //I can't make an array of undefined length in C, but 100 is the standard length for long Simon games (world records for Simon are measured in how many sets of 100 are completed consecutively)
    for (sequence_idx = 0; sequence_idx < 100; sequence_idx++) {
        if (sequence_idx < game_length) {
            game_sequence[sequence_idx] = (rand((button_options-1), bits_needed) + 1); //random number between 1 and 4 for four buttons
        }
        else {
            game_sequence[sequence_idx] = 0;
        }
    }
    //Game sequence is ready now


    //Gameplay!
    for (sequence_idx = 0; sequence_idx < game_length; sequence_idx++) {

        //Tell the player the part of the sequence they need to hear

        resetTimer();
        for (replay_sequence_idx = 0; replay_sequence_idx < (sequence_idx + 1); replay_sequence_idx++) {

            StopNote();
            ClearLEDs();
            timerDelay((game_timeout_duration << 5));


            switch(game_sequence[replay_sequence_idx]) {
                case 1: //Button 1
                    PlayNote(C4);
                    ColorLED(1, 0, 0, 0x64);
                    break;
                case 2: //Button 2
                    PlayNote(E4);
                    ColorLED(2, 0x64, 0, 0);
                    break;
                case 3: //Button 3
                    PlayNote(G4);
                    ColorLED(3, 0, 0x4E, 0x16);
                    break;
                case 4: //Button 4
                    PlayNote(B4);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;

                case 5:
                    PlayNote(D4);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(2, 0x64, 0, 0);
                    break;
                case 6:
                    PlayNote(F4);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(3, 0, 0x4E, 0x16);
                    break;
                case 7:
                    PlayNote(A4);
                    ColorLED(3, 0, 0x4E, 0x16);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                case 8:
                    PlayNote(C5);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                case 9:
                    PlayNote(Eb4);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(3, 0, 0x4E, 0x16);
                    break;
                case 10:
                    PlayNote(Gb4);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;

                case 11: // All buttons except 4
                    PlayNote(D5);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(3, 0, 0x4E, 0x16);
                    break;

                case 12: // All buttons except 1
                    PlayNote(E5);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(3, 0, 0x4E, 0x16);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                case 13: // All buttons except 2
                    PlayNote(F5);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(3, 0, 0x4E, 0x16);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                case 14: // All buttons except 3
                    PlayNote(G5);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;

                case 15:
                    PlayNote(C6);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(3, 0, 0x4E, 0x16);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;

                default:
                    game_sequence[replay_sequence_idx] = 4;
                    PlayNote(B4);
                    break;
            }
            timerDelay((game_timeout_duration << 5));
            StopNote();
            ClearLEDs();
        }
        startTimer(game_timeout_duration);

        //Let the player answer


        for (replay_sequence_idx = 0; replay_sequence_idx < (sequence_idx + 1); replay_sequence_idx++) {

            button_pressed = 0;

            while ((button_pressed == 0) && (timer_burned == 0)); //as long as no button is pressed and the watchdog timer hasn't elapsed, then we're hanging out

            resetTimer();
            if (timer_burned) {
                timer_burned = 0;
                button_pressed = 0;
                songLose();
                return;
            }

            volatile int button_storage = button_pressed;

            if (button_difficulty == DOUBLE_BUTTON) {
                timerDelay(600); //Give the player 50ms to complete a double-button press, if two-button presses are allowed (i.e. if there are more than 4 options for answers to give)
            }
            else if (button_difficulty == ALL_BUTTON) {
                timerDelay(2400); //Give the player 200ms to complete a multi-button press, if three- and four-button presses are allowed (i.e. if there are more than 10 options for answers to give)
            }

            button_storage = button_storage | button_pressed;

            switch(button_storage) {
                case BUTTON1: //Button 1
                    answer = 1;
                    PlayNote(C4);
                    ColorLED(1, 0, 0, 0x64);
                    break;
                case BUTTON2: //Button 2
                    answer = 2;
                    PlayNote(E4);
                    ColorLED(2, 0x64, 0, 0);
                    break;
                case BUTTON3: //Button 3
                    answer = 3;
                    PlayNote(G4);
                    ColorLED(3, 0, 0x4E, 0x16);
                    break;
                case BUTTON4: //Button 4
                    answer = 4;
                    PlayNote(B4);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;

                case (BUTTON1 + BUTTON2):
                    answer = 5;
                    PlayNote(D4);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(2, 0x64, 0, 0);
                    break;
                case (BUTTON2 + BUTTON3):
                    answer = 6;
                    PlayNote(F4);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(3, 0, 0x4E, 0x16);
                    break;
                case (BUTTON3 + BUTTON4):
                    answer = 7;
                    PlayNote(A4);
                    ColorLED(3, 0, 0x4E, 0x16);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                case (BUTTON4 + BUTTON1):
                    answer = 8;
                    PlayNote(C5);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                case (BUTTON1 + BUTTON3):
                    answer = 9;
                    PlayNote(Eb4);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(3, 0, 0x4E, 0x16);
                    break;
                case (BUTTON2 + BUTTON4):
                    answer = 10;
                    PlayNote(Gb4);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;

                case (BUTTON1 + BUTTON2 + BUTTON3): // All buttons except 4
                    answer = 11;
                    PlayNote(D5);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(3, 0, 0x4E, 0x16);
                    break;

                case (BUTTON2 + BUTTON3 + BUTTON4): // All buttons except 1
                    answer = 12;
                    PlayNote(E5);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(3, 0, 0x4E, 0x16);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                case (BUTTON3 + BUTTON4 + BUTTON1): // All buttons except 2
                    answer = 13;
                    PlayNote(F5);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(3, 0, 0x4E, 0x16);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                case (BUTTON4 + BUTTON1 + BUTTON2): // All buttons except 3
                    answer = 14;
                    PlayNote(G5);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;

                case (BUTTON1 + BUTTON2 + BUTTON3 + BUTTON4):
                    answer = 15;
                    PlayNote(C6);
                    ColorLED(1, 0, 0, 0x64);
                    ColorLED(2, 0x64, 0, 0);
                    ColorLED(3, 0, 0x4E, 0x16);
                    ColorLED(4, 0x21, 0x21, 0x21);
                    break;
                default:
                    answer = 16; //this shouldn't happen
                    break;
            }

            //Was it the correct answer?
            if (game_sequence[replay_sequence_idx] == answer) {

                while (button_pressed != 0);
                StopNote();
                ClearLEDs();

                startTimer(game_timeout_duration);

            }
            //If it was the wrong button, then kick the player out of the loop with a sad song
            else {
                button_pressed = 0;
                timer_burned = 0;
                resetTimer();
                songLose();

                return;
            }

            //restart timer



        }



    }

    button_pressed = 0;
    timer_burned = 0;
    resetTimer();
    songWin();




}


void pre_simon_menu_2(int difficulty, int button_difficulty) {
    timerDelay(3000);
    songStart();
    int dont_exit_loop = 1;
    while(dont_exit_loop) {
        ColorLED(1, 0, 0, 0x08);
        ColorLED(2, 0, 0, 0x16);
        ColorLED(3, 0, 0, 0x32);
        ColorLED(4, 0, 0, 0x64);
        switch(button_pressed) {
            case BUTTON1:

                ClearLEDs();
                PlayNote(G5);
                ColorLED(1, 0, 0, 0x08);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                simon(difficulty, button_difficulty, FIVE);

                dont_exit_loop = 0;

                break;
            case BUTTON2:

                ClearLEDs();
                PlayNote(B5);
                ColorLED(2, 0, 0, 0x16);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                simon(difficulty, button_difficulty, TEN);

                dont_exit_loop = 0;

                break;
            case BUTTON3:

                ClearLEDs();
                PlayNote(D6);
                ColorLED(3, 0, 0, 0x32);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                simon(difficulty, button_difficulty, FIFTEEN);

                dont_exit_loop = 0;

                break;
            case BUTTON4:

                ClearLEDs();
                PlayNote(F6);
                ColorLED(4, 0, 0, 0x64);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                simon(difficulty, button_difficulty, HUNDRED);

                dont_exit_loop = 0;

                break;
            default:
                break;
        }
    }

}

void pre_simon_menu_1(int button_difficulty) {
    timerDelay(3000);
    songStart();
    int dont_exit_loop = 1;
    while(dont_exit_loop) {
        ColorLED(1, 0x32, 0x32, 0);
        ColorLED(2, 0, 0x32, 0x32);
        ColorLED(3, 0x32, 0, 0x32);
        ColorLED(4, 0, 0, 0x64);
        switch(button_pressed) {
            case BUTTON1:

                ClearLEDs();
                PlayNote(F4);
                ColorLED(1, 0x32, 0x32, 0);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                pre_simon_menu_2(EASY, button_difficulty);

                dont_exit_loop = 0;

                break;
            case BUTTON2:

                ClearLEDs();
                PlayNote(A4);
                ColorLED(2, 0, 0x32, 0x32);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                pre_simon_menu_2(MEDIUM, button_difficulty);

                dont_exit_loop = 0;

                break;
            case BUTTON3:

                ClearLEDs();
                PlayNote(C5);
                ColorLED(3, 0x32, 0, 0x32);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                pre_simon_menu_2(HARD, button_difficulty);

                dont_exit_loop = 0;

                break;
            case BUTTON4:

                ClearLEDs();
                PlayNote(E5);
                ColorLED(4, 0, 0, 0x64);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                pre_simon_menu_2(IMPOSSIBLE, button_difficulty);

                dont_exit_loop = 0;

                break;
            default:
                break;
        }
    }

}

void start_menu(void) {
    timerDelay(3000);
    songStart();
    while(1) {
        ColorLED(1, 0x64, 0, 0);
        ColorLED(2, 0, 0x64, 0);
        ColorLED(3, 0, 0, 0x64);
        ColorLED(4, 0x21, 0x21, 0x21);
        switch(button_pressed) {
            case BUTTON1:

                ClearLEDs();
                PlayNote(C4);
                ColorLED(1, 0x64, 0, 0);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                pre_simon_menu_1(SINGLE_BUTTON);

                songStart();
                break;
            case BUTTON2:

                ClearLEDs();
                PlayNote(G4);
                ColorLED(2, 0, 0x64, 0);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                pre_simon_menu_1(DOUBLE_BUTTON);
                songStart();
                break;
            case BUTTON3:

                ClearLEDs();
                PlayNote(C5);
                ColorLED(3, 0, 0, 0x64);

                while (button_pressed != 0);

                StopNote();
                ClearLEDs();

                pre_simon_menu_1(ALL_BUTTON);
                songStart();
                break;
            case BUTTON4:
                piano();
                songStart();
                break;
            default:
                break;
        }
    }
}

// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{

    //I'm using the WDT to control the timeout for how long a player can go without pressing a button in the Simon game before being kicked out.
    if (WDT_counter >= alloted_time) {
        timer_burned = 1;
    }
    else {
        WDT_counter++; //When the WDT is activated by the startTimer function, it will start counting. When it hits the alloted_time for the game, then it will send a notice to the code that the player needs to be kicked.
    }
}
