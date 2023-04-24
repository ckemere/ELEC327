; --COPYRIGHT--,BSD_EX
;  Copyright (c) 2014, Texas Instruments Incorporated
;  All rights reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions
;  are met:
;
;  *  Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;
;  *  Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;
;  *  Neither the name of Texas Instruments Incorporated nor the names of
;     its contributors may be used to endorse or promote products derived
;     from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
;  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
;  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
;  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
;  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
;  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
;  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
;  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
;  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
; ******************************************************************************
;
;                        MSP430 CODE EXAMPLE DISCLAIMER
;
;  MSP430 code examples are self-contained low-level programs that typically
;  demonstrate a single peripheral function or device feature in a highly
;  concise manner. For this the code may rely on the device's power-on default
;  register values and settings such as the clock configuration and care must
;  be taken when combining code from several examples to avoid potential side
;  effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
;  for an API functional library-approach to peripheral configuration.
;
; --/COPYRIGHT--
;******************************************************************************
;  MSP430FR243x Demo - Timer0_A3 Capture of ACLK
;
;  Description; Capture a number of periods of the ACLK clock and store them in an array.
;  When the set number of periods is captured the program is trapped and the LED on
;  P1.0 is toggled. At this point halt the program execution read out the values using
;  the debugger.
;  ACLK = REFOCLK = 32kHz, MCLK = SMCLK = default DCODIV = 1MHz.
;
;                MSP430FR2433
;             -----------------
;         /|\|                 |
;          | |             P1.2|<-- TA0.2
;          --|RST              |  |
;            |             P2.2|--> ACLK
;            |                 |
;            |             P1.0|-->LED
;
;  
;  Wei Zhao
;  Texas Instruments Inc.
;  Jan 2014
;  Built with Code Composer Studio v6.0
;******************************************************************************
            .cdecls C,LIST,"msp430.h"                       ; Include device header file
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack                                  ; Make stack linker segment ?known?
                
            .global _main
            .text                                           ; Assemble to Flash memory
            .retain                                         ; Ensure current section gets linked
            .retainrefs
_main
            
RESET       mov.w   #__STACK_END,SP                         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL                  ; Stop WDT

            bis.b   #BIT0,&P1DIR                            ; P1.0 LED
            bis.b   #BIT0,&P1OUT
            bis.b   #BIT2,&P1SEL1                           ; TA0.CCI2A input capture pin, second function
            bis.b   #BIT2,&P1REN                            ; Enable internal pull-down resistor
            bic.b   #BIT2,&P1OUT

            bis.b   #BIT2,&P2SEL1                           ; Set as ACLK pin, second function
            bis.b   #BIT2,&P2DIR                            ; output ACLK pin
            bic.w   #LOCKLPM5,&PM5CTL0                      ; Unlock I/O pin

            bis.w   #SCG0,SR                                ; Disable FLL
            bis.w   #SELREF__REFOCLK,&CSCTL3                ; Set REFO as FLL reference source
            mov.w   #0,&CSCTL0                              ; clear DCO and MOD registers
            bic.w   #DCORSEL_7,&CSCTL1                      ; Clear DCO frequency select bits first
            bis.w   #DCORSEL_2,&CSCTL1                      ; Set DCO = 4MHz
            mov.w   #FLLD_1+60,&CSCTL2                      ; DCODIV = 2MHz
            nop
            nop
            nop
            bic.w   #SCG0,SR                ; Enable FLL
Unlock      mov.w   &CSCTL7,R13
            and.w   #FLLUNLOCK0|FLLUNLOCK1,R13
            jnz     Unlock                  ; Check if FLL is locked

Go_on       bis.w   #SELMS__DCOCLKDIV|SELA__REFOCLK,&CSCTL4 ; set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                                            ; default DCODIV as MCLK and SMCLK source
            bis.w   #DIVM__1|DIVS__2,&CSCTL5                ; SMCLK = 1MHz, MCLK = 2MHz

            bis.w   #CM_1|CCIS_0|SCS|CAP|CCIE,&TA0CCTL2     ; Capture rising edge,
                                                            ; Use CCI2A=ACLK,
                                                            ; Synchronous capture,
                                                            ; Enable capture mode,
                                                            ; Enable capture interrupt
    
            bis.w   #TASSEL_2|MC_2|TACLR,&TA0CTL            ; Use SMCLK as clock source,
                                                            ; Start timer in continuous mode
            mov.w   #0,R15
Mainloop    bis.w   #LPM0+GIE,SR                            ; Enter LPM0 w/ interrupt
            nop                                             ; For debugger

;-------------------------------------------------------------------------------
TIMER0_A1_ISR;    ISR for TA0CCR2
;-------------------------------------------------------------------------------
            add     &TA0IV,PC                               ; Add offset to PC
            reti                                            ; No interrupt
            reti                                            ; CCR1 not used
            jmp     CCIFG_2_HND                                            
            reti                                            ; reserved
            reti                                            ; reserved
            reti                                            ; reserved
            reti                                            ; reserved
            reti                                            ; overflow
            reti
            
CCIFG_2_HND mov.w   &TA0CCR2,0X2000(R15)
            add.w   #0x2,R15
            cmp.w   #0x002A,R15
            jlo     L1
Toggle      xor.b   #BIT0,&P1OUT
            mov.w   #50000,R14
L2          dec.w   R14
            jnz     L2
            jmp     Toggle
L1          reti
            
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   RESET_VECTOR                            ; MSP430 RESET Vector
            .short  RESET                                   
            .sect   TIMER0_A1_VECTOR                        ; Timer_A0 Vector
            .short  TIMER0_A1_ISR
            .end
