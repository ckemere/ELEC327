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
;   MSP430FR24xx Demo - USCI_A0 External Loopback test @ 115200 baud
;
;  Description: This demo connects TX to RX of the MSP430 UART
;  The example code shows proper initialization of registers
;  and interrupts to receive and transmit data. If data is incorrect P1.0 LED is
;  turned ON.
;  ACLK = n/a, MCLK = SMCLK = BRCLK = DCODIV ~1MHz.
;
;                MSP430FR2433
;             -----------------
;         /|\|                 |
;          | |                 |
;          --|RST              |
;            |                 |
;            |                 |
;            |     P1.4/UCA0TXD|----
;            |                 |   |
;            |     P1.5/UCA0RXD|----
;            |                 |
;            |            P1.0 |--> LED
;            |                 |
;
;   Ling Zhu
;   Texas Instruments Inc.
;   July 2015
;   Code Composer Studio v6.0
;*******************************************************************************
            .cdecls C,LIST,"msp430.h"        ; Include device header file
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.

RXData      .set    R6
TXData      .set    R7
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack                         ; Make stack linker segment ?known?

            .text                                  ; Assemble to Flash memory
            .retain                                ; Ensure current section gets linked
            .retainrefs

RESET       mov.w   #__STACK_END,SP                ; Initialize stack pointer
            mov.w    #WDTPW+WDTHOLD,&WDTCTL        ; Stop WDT

            bic.w   #LOCKLPM5,PM5CTL0              ; Unlock I/O pins

            bis.b   #BIT0,&P1DIR
            bic.b   #BIT0,&P1OUT                   ; P1.0 out low

            bis.b   #BIT4+BIT5,&P1SEL0             ; Configure UART pins

            bis.w   #UCSWRST,&UCA0CTLW0            ; Configure UART 0
            bis.w   #UCSSEL__SMCLK,&UCA0CTLW0      ; Set ACLK = 32768 as UCBRCLK
            mov.b   #3,&UCA0BR0                    ; 1000000/115200 = 8.68
            bis.w   #0xD600,&UCA0MCTLW             ; 1000000/115200 - INT(1000000/115200)=0.68
                                                   ; UCBRSx value = 0xD6 (See UG)
            clr.b   &UCA0BR1

            bic.w   #UCSWRST,&UCA0CTLW0            ; release from reset
            bis.w   #UCRXIE,&UCA0IE                ; enable RX interrupt
            
            mov.w   #254,TXData

Main_loop   bit.w   #UCTXIFG,&UCA0IFG
            jz      Main_loop
            mov.w   TXData,&UCA0TXBUF

            nop
            bis.w   #LPM0+GIE,SR                   ; Enter LPM0, enable interrupt
            nop
            jmp     Main_loop

;------------------------------------------------------------------------------
USCI_ISR ;    USCI Interrupt Service Routine
;------------------------------------------------------------------------------
            add.w   &UCA0IV,PC
            reti                                   ; Vector  0: No interrupt
            jmp     UARTev                         ; Vector  2: USCI_UART_UCRXIFG
            reti                                   ; Vector  4: USCI_UART_UCTXIFG
            reti                                   ; Vector  6: USCI_UART_UCSTTIFG
            reti                                   ; Vector  8: USCI_UART_UCTXCPTIFG
  
UARTev      bic.w   #UCRXIFG,&UCA0IFG              ; Clear interrupt
            mov.w   &UCA0RXBUF,RXData              ; Clear buffer
            cmp.w   TXData,RXData  
            jz      Go_on  

            bis.b   #BIT0,&P1OUT                   ; If incorrect turn on P1.0
Trap        jmp     Trap                           ; trap CPU

Go_on       add.w   #1,TXData                      ; increment data byte
            and.w   #0x00FF,TXData

            clr.w   0(SP)                          ; Exit LPM0 on reti
            nop
            reti
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   RESET_VECTOR                   ; MSP430 RESET Vector
            .short  RESET                          ;
            .sect   USCI_A0_VECTOR                 ; USCI_A0_VECTOR
            .short  USCI_ISR                       ;
            .end
