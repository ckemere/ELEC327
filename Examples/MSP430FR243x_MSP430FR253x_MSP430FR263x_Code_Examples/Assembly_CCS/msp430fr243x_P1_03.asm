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
;  MSP430FR243x Demo - Software Port Interrupt Service on P1.3 from LPM3
;
;  Description: A Hi/Lo transition on P1.3 will trigger P1ISR the first time.
;  On hitting the P1ISR, device exits LPM3 mode and executes section of code in
;  main() which includes toggling an LED.
;
;  ACLK = default REFO ~32768Hz, MCLK = SMCLK = default DCODIV ~1MHz
;
;               MSP430FR2433
;            -----------------
;        /|\|                 |
;         | |                 |
;         --|RST              |
;           |                 |
;     /|\   |                 |
;      --o--|P1.3         P1.0|-->LED
;     \|/   |                 |
;           |                 |
;
;   Cen Fang
;   Wei Zhao
;   Texas Instruments Inc.
;   September 2013
;   Built with Code Composer Studio v6.0
;******************************************************************************
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack                  ; Make stack linker segment ?known?

            .text                           ; Assemble to Flash memory
            .retain                         ; Ensure current section gets linked
            .retainrefs

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
SetupP1     bic.b   #BIT0,&P1OUT            ; Clear P1.0 output
            bis.b   #BIT0,&P1DIR            ; P1.0 output

            bis.b   #BIT3,&P1OUT            ; Configure P1.3 as pulled-up
            bis.b   #BIT3,&P1REN            ; P1.3 pull-up register enable
            bis.b   #BIT3,&P1IES            ; P1.3 Lo/Hi edge
            bis.b   #BIT3,&P1IE             ; P1.3 interrupt enabled

            bic.w   #LOCKLPM5,PM5CTL0       ; Unlock I/O pins
            
            bic.b   #BIT3,&P1IFG            ; P1.3 IFG cleared

Mainloop    bis.w   #LPM3+GIE,SR            ; Enter LPM3 w/ interrupt
            nop                             ; for debug
            xor.b   #BIT0,&P1OUT            ; P1.0 = toggle
            jmp     Mainloop
;------------------------------------------------------------------------------
P1_ISR ;    Port 1 Interrupt
;------------------------------------------------------------------------------
            bic.b   #BIT3,&P1IFG            ; Clear P1.3 IFG
            bic.w   #LPM3,0(SP)             ; Exit LPM3
            reti
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   RESET_VECTOR            ; MSP430 RESET Vector
            .short  RESET
            .sect   PORT1_VECTOR            ; Port 1 Vector
            .short  P1_ISR
            .end
