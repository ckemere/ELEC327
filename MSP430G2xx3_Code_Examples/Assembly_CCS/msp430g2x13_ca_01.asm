; --COPYRIGHT--,BSD_EX
;  Copyright (c) 2012, Texas Instruments Incorporated
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
;*******************************************************************************
;   MSP430G2x13/G2x53 Demo - Comp_A, Output Reference Voltages on P1.1
;
;   Description: Output comparator_A reference levels on P1.1.  Program will
;   cycle through the on-chip comparator_A reference voltages with output on
;   P1.1. Normal mode is LPM0 with TA0 triggering interrupt for next mode.
;   ACLK = n/a, MCLK = SMCLK = default DCO
;
;             MSP430G2x13/G2x53
;             -----------------
;         /|\|              XIN|-
;          | |                 |
;          --|RST          XOUT|-
;            |                 |
;            |         CA1/P1.1|-->Vref
;            |                 |
;
;   D. Dang
;   Texas Instruments Inc.
;   December 2010
;   Built with Code Composer Essentials Version: 4.2.0
;*******************************************************************************
 .cdecls C,LIST,  "msp430.h"
;------------------------------------------------------------------------------
            .text                           ; Program Start
;------------------------------------------------------------------------------
RESET       mov.w   #0280h,SP               ; Initialize stackpointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
            mov.b   #P2CA4,&CACTL2          ; CA1
SetupC0     mov.w   #CCIE,&CCTL0            ; CCR0 interrupt enabled
SetupTA     mov.w   #TASSEL_2+ID_3+MC_2,&TACTL   ; SMCLK/8, contmode
            eint                            ; General enable interrupts
                                            ;
Mainloop    clr.b   &CACTL1                 ; No reference voltage
            bis.w   #CPUOFF,SR              ; CPU off
Ref1        mov.b   #CAREF0+CAON,&CACTL1    ; 0.25*Vcc, Comp. on
            bis.w   #CPUOFF,SR              ; CPU off
Ref2        mov.b   #CAREF1+CAON,&CACTL1    ; 0.5*Vcc, Comp. on
            bis.w   #CPUOFF,SR              ; CPU off
Ref3        mov.b   #CAREF1+CAREF0+CAON,&CACTL1     ; 0.55V on P2.3, Comp. on
            bis.w   #CPUOFF,SR              ; CPU off
            jmp     Mainloop                ;

;-------------------------------------------------------------------------------
TA0_ISR;    Exit LPM0
;-------------------------------------------------------------------------------
            bic.w   #CPUOFF,0(SP)           ; Exit LPM0 on RETI
            reti                            ;
                                            ;
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET                   ;
            .sect   ".int09"                ; TimerA0 Vector
            .short  TA0_ISR                 ;        
            .end
