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
;   MSP430G2x33/G2x53 Demo - ADC10, DTC Sample A1 32x, AVcc, TA0 Trig, DCO
;
;   Description: A1 is sampled in 32x burst using DTC ever 16/second
;   (ACLK/2048) with reference to AVcc. All.  Activity is interrupt driven.
;   Timer_A in upmode uses TA0 toggle to drive ADC10 conversion. Sample burst
;   is automatically triggered by TA0 rising edge every 16 ACLK cycles.
;   ADC10_ISR will exit from LPM3 mode and return CPU active. Internal ADC10OSC
;   times sample (16x) and conversion (13x). DTC transfers conversion code to
;   RAM 200h - 240h. In the Mainloop P1.0 is toggled. Normal Mode is LPM3.
;   //* An external watch crystal on XIN XOUT is required for ACLK *//
;
;                MSP430G2x33/G2x53
;             -----------------
;         /|\|              XIN|-
;          | |                 | 32kHz
;          --|RST          XOUT|-
;            |                 |
;        >---|P1.1/A1     P1.0 |--> LED
;
;   D. Dang
;   Texas Instruments Inc.
;   December 2010
;   Built with Code Composer Essentials Version: 4.2.0
;*******************************************************************************
 .cdecls C,LIST,  "msp430.h"

;------------------------------------------------------------------------------
            .text                           ; Progam Start
;------------------------------------------------------------------------------
RESET       mov.w   #0280h,SP               ; Initialize stackpointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
SetupADC10  mov.w   #INCH_1+SHS_2+CONSEQ_2,&ADC10CTL1 ; TA0 trigger
            mov.w   #ADC10SHT_2+MSC+ADC10ON+ADC10IE,&ADC10CTL0;
            mov.b   #020h,&ADC10DTC1        ; 32 conversions
SetupP1     bis.b   #001h,&P1DIR            ; P1.0 output
            bis.b   #002h,&ADC10AE0         ; P1.1 ADC10 option select
            mov.w   #1024-1,&TACCR0         ; PWM Period
SetupC0     mov.w   #OUTMOD_4,&TACCTL0      ; TACCR0 toggle
SetupTA     mov.w   #TASSEL_1+MC_1,TACTL    ; ACLK, up mode
                                            ;
Mainloop    bic.w   #ENC,&ADC10CTL0         ;
busy_test   bit     #BUSY,&ADC10CTL1        ; ADC10 core inactive?
            jnz     busy_test               ;
            mov.w   #0200h,&ADC10SA         ; Data buffer start
            bis.w   #ENC,&ADC10CTL0         ; Sampling and conversion ready
            bis.w   #LPM3+GIE,SR            ; LPM3, ADC10_ISR will force exit
            xor.b   #001h,&P1OUT            ; P1.0 = toggle
            jmp     Mainloop                ; Again
                                            ;
;-------------------------------------------------------------------------------
ADC10_ISR;  Exit LPM3 on reti
;-------------------------------------------------------------------------------
            bic.w   #LPM3,0(SP)             ; Exit LPM3 on reti
            reti                            ;
                                            ;
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET                   ;
            .sect   ".int05"                ; ADC10 Vector
            .short  ADC10_ISR               ;
            .end
