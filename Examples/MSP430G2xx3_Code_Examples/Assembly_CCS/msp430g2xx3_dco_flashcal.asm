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
;   MSP430G2xx3 Demo - DCO Calibration Constants Programmer
;
;   NOTE: THIS CODE REPLACES THE TI FACTORY-PROGRAMMED DCO CALIBRATION
;   CONSTANTS LOCATED IN INFOA WITH NEW VALUES. USE ONLY IF THE ORIGINAL
;   CONSTANTS WERE ACCIDENTALLY CORRUPTED OR ERASED.
;
;   Description: This code re-programs the G2xx2 DCO calibration constants.
;   A software FLL mechanism is used to set the DCO based on an external
;   32kHz reference clock. After each calibration, the values from the
;   clock system are read out and stored in a temporary variable. The final
;   frequency the DCO is set to is 1MHz, and this frequency is also used
;   during Flash programming of the constants. The program end is indicated
;   by the blinking LED.
;   ACLK = LFXT1/8 = 32768/8, MCLK = SMCLK = target DCO
;   //* External watch crystal installed on XIN XOUT is required for ACLK *//
;
;            MSP430G2xx3
;          ---------------
;      /|\|            XIN|-
;       | |               | 32kHz
;       --|RST        XOUT|-
;         |               |
;         |           P1.0|--> LED
;         |           P1.4|--> SMLCK = target DCO
;
;   D. Dang
;   Texas Instruments Inc.
;   December 2010
;   Built with Code Composer Essentials Version: 4.2.0
;*******************************************************************************
 .cdecls C,LIST,  "msp430.h"
;-------------------------------------------------------------------------------
DELTA_1MHZ  .EQU     244                    ; 244 x 4096Hz = 999.4Hz
DELTA_8MHZ  .EQU     1953                   ; 1953 x 4096Hz = 7.99MHz
DELTA_12MHZ .EQU     2930                   ; 2930 x 4096Hz = 12.00MHz
DELTA_16MHZ .EQU     3906                   ; 3906 x 4096Hz = 15.99MHz
;-------------------------------------------------------------------------------
            .data                           ; RAM variables
;------------------------------------------------------------------------------
 .bss      Const1,1                         ; Temp. storage for constants
 .bss      Const2,1
 .bss      Const3,1
 .bss      Const4,1
 .bss      Const5,1
 .bss      Const6,1
 .bss      Const7,1
 .bss      Const8,1
;------------------------------------------------------------------------------
            .text                           ; Progam Start
;------------------------------------------------------------------------------
RESET       mov.w   #0280h,SP               ; Initialize stackpointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
            push.w  #0ffffh                 ; LFXT1 startup delay
OscDelay    dec.w   0(SP)                   ; Do calc on the stack
            jnz     OscDelay                ;
            incd.w  SP                      ; Correct SP
SetupP1     clr.b   &P1OUT                  ; Clear P1 output latches
            mov.b   #010h,&P1SEL            ; P1.4 SMCLK output
            mov.b   #011h,&P1DIR            ; P1.0,4 output   
                                            ;
Main        mov.w   #DELTA_16MHZ,R12        ; Set DCO and obtain constants
            call    #Set_DCO                ;
            mov.b   &DCOCTL,&Const1         ;
            mov.b   &BCSCTL1,&Const2        ;
            mov.w   #DELTA_12MHZ,R12        ; Set DCO and obtain constants
            call    #Set_DCO                ;
            mov.b   &DCOCTL,&Const3         ;
            mov.b   &BCSCTL1,&Const4        ;
            mov.w   #DELTA_8MHZ,R12         ; Set DCO and obtain constants
            call    #Set_DCO                ;
            mov.b   &DCOCTL,&Const5         ;
            mov.b   &BCSCTL1,&Const6        ;
            mov.w   #DELTA_1MHZ,R12         ; Set DCO and obtain constants
            call    #Set_DCO                ;
            mov.b   &DCOCTL,&Const7         ;
            mov.b   &BCSCTL1,&Const8        ;
                                            ;
            mov.w   #FWKEY+FSSEL0+FN1,&FCTL2; MCLK/3 for Flash Timing Generator
            mov.w   #FWKEY+ERASE,&FCTL1     ; Set Erase bit
            mov.w   #FWKEY+LOCKA,&FCTL3     ; Clear LOCK & LOCKA bits
            mov.w   #0,&010c0h              ; Dummy write to erase Flash seg A
            mov.w   #FWKEY+WRT,&FCTL1       ; Set WRT bit for write operation
            mov.b   &Const1,&010f8h         ; re-flash DCO calibration data
            mov.b   &Const2,&010f9h         ; re-flash DCO calibration data
            mov.b   &Const3,&010fah         ; re-flash DCO calibration data
            mov.b   &Const4,&010fbh         ; re-flash DCO calibration data
            mov.b   &Const5,&010fch         ; re-flash DCO calibration data
            mov.b   &Const6,&010fdh         ; re-flash DCO calibration data
            mov.b   &Const7,&010feh         ; re-flash DCO calibration data
            mov.b   &Const8,&010ffh         ; re-flash DCO calibration data
            mov.w   #FWKEY,&FCTL1           ; Clear WRT bit
            mov.w   #FWKEY+LOCKA+LOCK,&FCTL3; Set LOCK & LOCKA bit
                                            ;
Mainloop    xor.b   #001h,&P1OUT            ; Toggle LED
            push.w  #20000                  ; Delay to TOS
Delay       dec.w   0(SP)                   ; Decrement TOS
            jnz     Delay                   ; Delay over?
            incd.w  SP                      ; Clean stack
            jmp     Mainloop                ;
                                            ;
;-------------------------------------------------------------------------------
Set_DCO;    Subroutine: Sets DCO to selected frequency based on Delta (R12).
;           R13, R14, and R15 are used, ACLK= 32768/8 Timer_A clocked by DCOCLK
;-------------------------------------------------------------------------------
            bis.b   #DIVA_3,&BCSCTL1        ; ACLK = LFXT1CLK/8
            clr.w   R15                     ;
Setup_CC2   mov.w   #CM_1+CCIS_1+CAP,&TACCTL0    ; CAP, ACLK
Setup_TA    mov.w   #TASSEL_2+MC_2+TACLR,&TACTL  ; SMCLK, Continous Mode, clear
Test_DCO    bit.w   #CCIFG,&TACCTL0         ; Test capture flag
            jz      Test_DCO                ;
            bic.w   #CCIFG,&TACCTL0         ; Clear capture flag
                                            ;
AdjDCO      mov.w   &TACCR0,R14             ; R14 = captured SMCLK
            sub.w   R15,R14                 ; R14 = capture difference
            mov.w   &TACCR0,R15             ; R15 = captured SMCLK
            cmp.w   R12,R14                 ; Delta (R12) = SMCLK/(32768/4)
            jlo     IncDCO                  ;
            jeq     DoneDCO                 ;
DecDCO      dec.b   &DCOCTL                 ; Slow DCO with DCO and MOD
            jc      Test_DCO                ; Slower?
            bit.b   #00fh,&BCSCTL1          ; Can RSELx be decremented
            jnc     Test_DCO                ; No
            dec.b   &BCSCTL1                ; Decrement RSEL.x
            jmp     Test_DCO                ;
IncDCO      inc.b   &DCOCTL                 ; Speed DCO with DCO and MOD
            jnc     Test_DCO                ; Faster?
            mov.b   &BCSCTL1,R13            ; Can RSEL.x be increased?
            and.b   #00fh,R13               ;
            cmp.b   #00fh,R13               ;
            jz      Test_DCO                ; No
            inc.b   &BCSCTL1                ; Increment RSEL.x
            jmp     Test_DCO                ;
DoneDCO     clr.w   &TACCTL0                ; Stop TACCR0
            clr.w   &TACTL                  ; Stop Timer_A
            bic.b   #DIVA_3,&BCSCTL1        ; ACLK = LFXT1CLK
            ret                             ; Return from subroutine
                                            ;
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET                   ;
            .end

