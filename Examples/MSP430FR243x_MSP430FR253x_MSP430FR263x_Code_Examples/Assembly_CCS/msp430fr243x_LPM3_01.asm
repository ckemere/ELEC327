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
;*******************************************************************************
;  MSP430FR243x Demo - Enters LPM3 with ACLK = XT1CLK = 32768Hz.
;
;  Description: Configures ACLK to be sourced from XT1 crystal. Enter LPM3 until
;               WDT ISR is executed and toggles the LED on P1.0.
;  ACLK = 32768Hz, MCLK = SMCLK ~ 1MHz,
;
;           MSP430FR2433
;         ---------------
;     /|\|               |
;      | |               |
;      | |      XIN(P2.0)|--
;      --|RST            |  ~32768Hz
;        |     XOUT(P2.1)|--
;        |               |
;        |           P1.0|-->LED
;
;  William Goh
;  Texas Instruments Inc.
;  January 2014
;  Built with Code Composer Studio v6.0
;*******************************************************************************
            .cdecls C,LIST,"msp430.h"  ; Include device header file
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack                  ; Make stack linker segment ?known?

;-------------------------------------------------------------------------------
            .global _main
            .text                           ; Assemble to Flash memory
            .retain                         ; Ensure current section gets linked
            .retainrefs
_main
RESET       mov.w   #__STACK_END,SP         ; Initialize stack pointer
            mov.w   #WDT_ADLY_1000,&WDTCTL  ; WDT is clocked by ACLK, enter WDT ISR every 1s

            bis.w   #WDTIE,&SFRIE1          ; Enable WDT interrupt

            bis.b   #BIT0 + BIT1,&P2SEL0    ; P2.0: XOUT; P2.1: XI1
            mov.w   #SELMS__DCOCLKDIV + SELA__XT1CLK,&CSCTL4
                                            ; MCLK=SMCLK=DCO; ACLK=XT1

            ; Port configuration
            clr.b   &P1OUT
            clr.b   &P2OUT
            clr.b   &P3OUT
            mov.b   #0xFF,&P1DIR
            mov.b   #0xFF,&P2DIR
            mov.b   #0xFF,&P3DIR

            ; Disable the GPIO power-on default high-impedance mode
            ; to activate previously configured port settings
            bic.w   #LOCKLPM5,&PM5CTL0

OSCFlag     bic.w   #XT1OFFG+DCOFFG,&CSCTL7 ; Clear XT1 fault flag
            bic.w   #OFIFG,&SFRIFG1
            bit.w   #OFIFG,&SFRIFG1         ; Test oscillator fault flag
            jnz     OSCFlag

            bis.w   #LPM3+GIE,SR            ; Enter LPM3
            nop

;-------------------------------------------------------------------------------
WDT_ISR;    Toggle P1.0
;-------------------------------------------------------------------------------
            xor.b   #BIT0,&P1OUT            ; Toggle P1.0
            reti                            ;

;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   RESET_VECTOR            ; MSP430 RESET Vector
            .short  RESET                   ;
            .sect   WDT_VECTOR              ; WDT Vector
            .short  WDT_ISR                 ;
            .end
