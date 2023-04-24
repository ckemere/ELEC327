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
;  MSP430FR243x Demo - Timer0_A3, Toggle P1.0, CCR0 Up Mode ISR, DCO SMCLK
;
;  Description: Toggle P1.0 using software and TA_0 ISR. Timer0_A is
;  configured for up mode, thus the timer overflows when TAR counts
;  to CCR0. In this example, CCR0 is loaded with 50000.
;  ACLK = TACLK = 32768Hz, MCLK = SMCLK = TACLK = 4MHz
;
;
;           MSP430FR2433
;         ---------------
;     /|\|               |
;      | |               |
;      --|RST            |
;        |               |
;        |           P1.0|-->LED
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

            bis.w   #SCG0,SR                                ; Disable FLL
            bis.w   #SELREF__REFOCLK,&CSCTL3                ; Set REFOCLK as FLL reference source
            mov.w   #0,&CSCTL0                              ; clear DCO and MOD registers
            bic.w   #DCORSEL_7,&CSCTL1                      ; Clear DCO frequency select bits first            
            bis.w   #DCORSEL_3,&CSCTL1                      ; Set DCOCLK = 8MHz
            mov.w   #FLLD_1+121,&CSCTL2                     ; FLLD = 1, by default, DCODIV = DCO/2 = 4MHz
            nop
            nop
            nop
            bic.w   #SCG0,SR                                ; Enable FLL
Unlock      mov.w   &CSCTL7,R13
            and.w   #FLLUNLOCK0|FLLUNLOCK1,R13
            jnz     Unlock                                  ; Check if FLL is locked

            bis.w   #SELMS__DCOCLKDIV|SELA__REFOCLK,&CSCTL4 ; set ACLK = REFOCLK = 32768Hz, DCOCLK as MCLK and SMCLK source
            bis.w   #DIVM0|DIVS0,&CSCTL5                    ; SMCLK = MCLK = DCODIV = 4MHz
            
            bis.b   #BIT0,&P1DIR                            ; Set P1.0 to output direction
            bis.b   #BIT0,&P1OUT                            ; P1.0 high
            bic.w   #LOCKLPM5,&PM5CTL0                      ; Unlock I/O pins

            mov.w   #TASSEL_1|MC_2|TACLR|TAIE,&TA0CTL       ; ACLK, count mode, clear TAR, enable interrupt

Mainloop    bis.w   #LPM3+GIE,SR                            ; Enter LPM3, enable interrupts
            nop                                             ; For debugger
;-------------------------------------------------------------------------------
TIMER0_A1_ISR;    ISR for overflow
;-------------------------------------------------------------------------------
            add     &TA0IV,PC                               ; Add offset to PC
            reti                                            ; No interrupt
            reti                                            ; TA0IV_TA0CCR1
            reti                                            ; TA0IV_TA0CCR2
            reti                                            ; reserved
            reti                                            ; reserved
            reti                                            ; reserved
            reti                                            ; reserved
            xor.b   #BIT0,&P1OUT                            ; TAIFG overflow
            reti
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   RESET_VECTOR                            ; MSP430 RESET Vector
            .short  RESET                                   ;
            .sect   TIMER0_A1_VECTOR                        ; Timer_A0 Vector
            .short  TIMER0_A1_ISR
            .end
