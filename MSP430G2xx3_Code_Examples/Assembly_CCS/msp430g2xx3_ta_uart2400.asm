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
;   MSP430G2xx3 Demo - Timer_A, Ultra-Low Pwr UART 2400 Echo, 32kHz ACLK
;
;   Description: Use Timer_A CCR0 hardware output modes and SCCI data latch
;   to implement UART function @ 2400 baud. Software does not directly read and
;   write to RX and TX pins, instead proper use of output modes and SCCI data
;   latch are demonstrated. Use of these hardware features eliminates ISR
;   latency effects as hardware insures that output and input bit latching and
;   timing are perfectly synchronised with Timer_A regardless of other
;   software activity. In the Mainloop the UART function readies the UART to
;   receive one character and waits in LPM3 with all activity interrupt driven.
;   After a character has been received, the UART receive function forces exit
;   from LPM3 in the Mainloop which echo's back the received character.
;   ACLK = TACLK = LFXT1 = 32768Hz, MCLK = SMCLK = default DCO
;   //* An external watch crystal is required on XIN XOUT for ACLK *//	
;
;                MSP430G2xx3
;             -----------------
;         /|\|              XIN|-
;          | |                 | 32kHz
;          --|RST          XOUT|-
;            |                 |
;            |   CCI0B/TXD/P1.5|-------->
;            |                 | 2400 8N1
;            |   CCI0A/RXD/P1.1|<--------
;
RXD        .equ     002h                    ; RXD on P1.1
TXD        .equ     020h                    ; TXD on P1.5
;
;   CPU Registers Used
RXTXData   .equ     R4
BitCnt     .equ     R5
;
;   Conditions for 2400 Baud SW UART, ACLK = 32768
Bitime_5    .equ     6                      ; ~0.5 bit length + small adjustment
Bitime      .equ     14                     ; 427us bit length ~ 2341 baud
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
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop Watchdog Timer
SetupTA     mov.w   #TASSEL_1+MC_2,&TACTL   ; ACLK, continuous mode
SetupC0     mov.w   #OUT,&CCTL0             ; TXD Idle as Mark
SetupP1     bis.b   #TXD+RXD,&P1SEL         ;
            bis.b   #TXD,&P1DIR             ;
                                            ;
Mainloop    call    #RX_Ready               ; UART ready to RX one Byte
            bis.w   #LPM3+GIE,SR            ; Enter LPM3 w/ int until Byte RXed
            call    #TX_Byte                ; TX Back RXed Byte Received
            jmp     Mainloop                ;
                                            ;
;-------------------------------------------------------------------------------
TX_Byte   ; Subroutine Transmits Character from RXTXData Buffer
;-------------------------------------------------------------------------------
TX_1        mov.w   &TAR,&CCR0              ; Current state of TA counter
            cmp.w   &TAR,&CCR0              ; !!Prevent async capature!!
            jne     TX_1                    ;
            add.w   #Bitime,&CCR0           ; Some time till first bit
            bis.w   #0100h, RXTXData        ; Add mark stop bit to RXTXData
            rla.w   RXTXData                ; Add space start bit
            mov.w   #10,BitCnt              ; Load Bit counter, 8data + ST/SP
            mov.w   #CCIS0+OUTMOD0+CCIE,&CCTL0    ; TXD = mark = idle
TX_Wait     bit.w   #CCIE,&CCTL0            ; Wait for TX completion
            jnz     TX_Wait                 ;
            ret                             ;
                                            ;
;-------------------------------------------------------------------------------
RX_Ready  ; Subroutine Readies UART to Receive Character into RXTXData Buffer
;-------------------------------------------------------------------------------
            mov.w   #8,BitCnt              ; Load Bit Counter, 8 data bits
SetupRX     mov.w   #CM1+SCS+OUTMOD0+CAP+CCIE,&CCTL0  ; Neg Edge,Sync,cap
            ret                             ;
                                            ;
;-------------------------------------------------------------------------------
TA0_ISR  ;  RXTXData Buffer holds UART Data
;-------------------------------------------------------------------------------
            add.w   #Bitime,&CCR0           ; Time to next bit
            bit.w   #CCIS0,&CCTL0           ; RX on CCI0B?
            jnz     UART_TX                 ; Jump --> TX
UART_RX     bit.w   #CAP,&CCTL0             ; Capture mode = start bit edge
            jz      RX_Bit                  ; Start bit edge?
RX_Edge     bic.w   #CAP,&CCTL0             ; Switch to compare mode
            add.w   #Bitime_5,&CCR0         ; First databit 1.5 bits from edge
            reti                            ;
RX_Bit      bit.w   #SCCI,&CCTL0            ; Get bit waiting in receive latch
            rrc.b   RXTXData                ; Store received bit
RX_Test     dec.w   BitCnt                  ; All bits RXed?
            jnz     RX_Next                 ; Next bit?
;>>>>>>>>>> Decode of Received Byte Here <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
RX_Comp     bic.w   #CCIE,&CCTL0            ; All bits RXed, disable interrupt
            mov.w   #GIE,0(SP)              ; Decode byte = active in Mainloop
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
RX_Next     reti                            ;
                                            ;
UART_TX     cmp.w   #00h,BitCnt             ;
            jne     TX_Next                 ; Next bit?
            bic.w   #CCIE,&CCTL0            ; All Bits TX or RX, Disable Int.
            reti                            ;
TX_Next     bic.w   #OUTMOD2,&CCTL0         ; TX Mark
            rra.w   RXTXData                ; LSB is shifted to carry
            jc      TX_Test                 ; Jump --> bit = 1
TX_Space    bis.w   #OUTMOD2,&CCTL0         ; TX Space
TX_Test     dec.w   BitCnt                  ; All bits sent (or received)?
            reti                            ;
                                            ;
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET                   ;
            .sect   ".int09"                ; Timer_A0 Vector
            .short  TA0_ISR                 ;
            .end
