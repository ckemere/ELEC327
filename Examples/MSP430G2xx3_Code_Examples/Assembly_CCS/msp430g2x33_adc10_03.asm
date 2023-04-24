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
;   MSP430G2x33/G2x53 Demo - ADC10, Sample A10 Temp, Set P1.0 if Temp ++ ~2C
;
;   Description: Set ADC10 and the integrated temperature sensor to detect
;   temperature gradients. The temperature sensor output voltage is sampled
;   ~ every 120ms and compared with the defined delta values using an ISR.
;   (ADC10OSC/4)/64 determines sample time which needs to be greater than
;   30us for temperature sensor.
;   ADC10 is operated in repeat-single channel mode with the sample and
;   convert trigger sourced from Timer_A CCR1. The ADC10IFG at the end
;   of each converstion will trigger an ISR.
;   ACLK = n/a, MCLK = SMCLK = default DCO ~1.2MHz, ADC10CLK = ADC10OSC
;
;                MSP430G2x33/G2x53
;             -----------------
;         /|\|              XIN|-
;          | |                 |
;          --|RST          XOUT|-
;            |                 |
;            |A10 (Temp)   P1.0|-->LED
;
;   D. Dang
;   Texas Instruments Inc.
;   December 2010
;   Built with Code Composer Essentials Version: 4.2.0
;*******************************************************************************
 .cdecls C,LIST,  "msp430.h"
;-------------------------------------------------------------------------------
ADCDeltaOn  .equ     3                      ;  ~ 2 Deg C delta for LED on
;------------------------------------------------------------------------------
            .text                           ; Progam Start
;------------------------------------------------------------------------------
RESET       mov.w   #0280h,SP               ; Initialize stackpointer
            mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
            mov.w   #ADC10DIV_3+INCH_10+SHS_1+CONSEQ_2,&ADC10CTL1
            mov.w   #SREF_1+ADC10SHT_3+REF2_5V+ADC10IE+REFON+ADC10ON,&ADC10CTL0
            mov.w   #30,&TACCR0             ; Delay to allow Ref to settle
            bis.w   #CCIE,&TACCTL0          ; Compare-mode interrupt.
            mov.w   #TACLR+MC_1+TASSEL_2,&TACTL; up mode, SMCLK
            bis.w   #LPM0+GIE,SR            ; Enter LPM0, enable interrupts
            bic.w   #CCIE,&TACCTL0          ; Disable timer interrupt
            dint                            ; Disable Interrupts
            bis.w   #ENC,&ADC10CTL0
            mov.w   #OUTMOD_4,&TACCTL1      ; Toggle on EQU1 (TAR = 0)
            mov.w   #TASSEL_2+MC_2,&TACTL   ; SMCLK, cont-mode
ADC_Wait    bit.w   #ADC10IFG,&ADC10CTL0    ; First conversion?
            jnc     ADC_Wait
            mov.w   &ADC10MEM,R4            ; Read out 1st ADC value
            add.w   #ADCDeltaOn,R4          ;
            clr.b   &P1OUT                  ; Clear P1
            bis.b   #01h,&P1DIR             ; P1.0 as output
                                            ;
Mainloop    bis.w   #LPM0+GIE,SR            ; Enter LPM0, enable interrupts
            nop                             ; Required only for debugger

;-------------------------------------------------------------------------------
TA0_ISR;    ISR for TACCR0
;-------------------------------------------------------------------------------
            clr.w   &TACTL                  ; Clear Timer_A control registers
            bic.w   #LPM0,0(SP)             ; Exit LPM0 on reti
            reti                            ;
;-------------------------------------------------------------------------------
ADC10_ISR;
;-------------------------------------------------------------------------------
            cmp.w   R4,&ADC10MEM            ; ADC10MEM = A10 > R4
            jlo     ADC_ISR_1               ; Again
            bis.b   #01h,&P1OUT             ; P1.0 = 1
            reti                            ;
ADC_ISR_1   bic.b   #01h,&P1OUT             ; P1.0 = 0
            reti                            ;
                                            ;
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET                   ;
            .sect   ".int05"                ; ADC10 Vector
            .short  ADC10_ISR               ;
            .sect   ".int09"                ; Timer_A0 Vector
            .short  TA0_ISR                 ;
            .end
