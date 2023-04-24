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
;  MSP430FR243x Demo - Configure MCLK for 8MHz sourced from DCO.
;
;  Description: Default DCODIV is MCLK and SMCLK source.
;  By default, FR413x select XT1 as FLL reference.
;  If XT1 is present, the XIN and XOUT pin needs to configure.
;  If XT1 is absent, switch to select REFO as FLL reference automatically.
;  XT1 is considered to be absent in this example.
;  f(DCOCLK) = 2^FLLD * (FLLN+1) * (fFLLREFCLK / n).
;  FLLD = 0, FLLN =243, n=1, DIVM =1, f(DCOCLK) = 2^0 * (243+1)*32768Hz = 8MHz,
;  f(DCODIV) = (243+1)*32768Hz = 8MHz,
;  ACLK = default REFO ~32768Hz,  SMCLK = MCLK =f(DCODIV) = 8MHz.
;
;           MSP430FR2433
;         ---------------
;     /|\|               |
;      | |               |
;      --|RST            |
;        |               |
;        |          P1.3 |---> MCLK = 8MHz
;        |          P1.7 |---> SMCLK = 8MHz
;        |          P2.2 |---> ACLK = 32768Hz
;
;
;   Eason Zhou
;   Texas Instruments Inc.
;   January 2020
;   Built with Code Composer Studio v9.2
;******************************************************************************
            .cdecls C,LIST,"msp430.h"  ; Include device header file

;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .data
oldDcoTap    .uword 0xFFFF
newDcoTap    .uword 0xFFFF
newDcoDelta  .uword 0xFFFF
bestDcoDelta .uword 0xFFFF
csCtl0Copy   .uword 0
csCtl1Copy   .uword 0
csCtl0Read   .uword 0
csCtl1Read   .uword 0
dcoFreqTrim  .uword 3
endLoop      .uword 0
;-------------------------------------------------------------------------------

            .text                           ; Assemble to Flash memory
            .retain                         ; Ensure current section gets linked
            .retainrefs

_main
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT

            bis.w   #SCG0,SR                ; Disable FLL
            mov.w   #SELREF__REFOCLK,&CSCTL3; Set REFO as FLL reference source
            mov.w   #0x0080+DCOFTRIM0+DCOFTRIM1+DCORSEL_3,&CSCTL1; DCOFTRIM=3, DCO Range = 8MHz
            mov.w   #FLLD_0+243,&CSCTL2     ; DCODIV = 8MHz
            nop
            nop
            nop
            bic.w   #SCG0,SR                ; Enable FLL

SWTrim      mov.w   #0x100,&CSCTL0          ; DCO Tap = 256
ChkFFG      bic.w   #DCOFFG,&CSCTL7         ; Clear DCO fault flag
            bit.w   #DCOFFG,&CSCTL7         ; Test DCO fault flag
            jnz     ChkFFG                  ; If set, attempt to clear again
                                            ; If clear, continue

            mov.w   #24000,R13              ; Wait FLL lock status (FLLUNLOCK) to be stable
Delay       dec     R13                     ; Suggest to wait 24 cycles of divided FLL reference clock
            jnz     Delay

Unlock      bit.w   #DCOFFG,&CSCTL7         ; Test DCO fault flag
            jnz     DCOTrim

            mov.w   &CSCTL7,R13
            and.w   #FLLUNLOCK0|FLLUNLOCK1,R13
            jnz     Unlock                  ; Check if FLL is locked

DCOTrim     mov.w   &CSCTL0,csCtl0Read      ; Software Trim
            mov.w   &CSCTL1,csCtl1Read
            mov.w   newDcoTap,oldDcoTap     ;Record DCOTAP value of last time
            mov.w   csCtl0Read,R13          ; Get DCOTAP value of this time
            and.w   #0x1FF,R13
            mov.w   R13,newDcoTap
            mov.w   csCtl1Read,R13
            and.w   #0x70,R13
            rrum.w  #4,R13
            mov.w   R13,dcoFreqTrim         ; Get DCOFTRIM value

            cmp.w   #0x100,newDcoTap
            jhs     DCOTAPH
DCOTAPL     mov.w   newDcoTap,R13           ;DCOTAP < 256
            mov.w   #0x100,R12
            sub.w   R13,R12
            mov.w   R12,newDcoDelta
            cmp.w   #0xFFFF,oldDcoTap
            jeq     FtrimSub
            cmp.w   #0x100,oldDcoTap
            jl      FtrimSub
            mov.w   #1,endLoop              ;Stop while loop

FtrimSub    sub.w   #1,dcoFreqTrim          ;dcoFreqTrim-1
            mov.w   csCtl1Read,R13
            bic.w   #0x0070,R13
            mov.w   dcoFreqTrim,R12
            rlam.w  #4,R12
            add.w   R12,R13
            mov.w   R13,&CSCTL1
            jmp     RecordCS

DCOTAPH     mov.w   newDcoTap,R13           ;DCOTAP >= 256
            sub.w   #0x100,R13
            mov.w   R13,newDcoDelta
            cmp.w   #0x100,oldDcoTap
            jhs     FtrimPlus
            mov.w   #1,endLoop              ;Stop while loop

FtrimPlus   inc.w   dcoFreqTrim             ;dcoFreqTrim+1
            mov.w   csCtl1Read,R13
            bic.w   #0x0070,R13
            mov.w   dcoFreqTrim,R12
            rlam.w  #4,R12
            add.w   R12,R13
            mov.w   R13,&CSCTL1

RecordCS    cmp.w   bestDcoDelta,newDcoDelta
            jhs     ChkEnd
            mov.w   csCtl0Read,csCtl0Copy  ;Record DCOTAP closest to 256
            mov.w   csCtl1Read,csCtl1Copy
            mov.w   newDcoDelta,bestDcoDelta

ChkEnd      cmp.w   #0,endLoop             ;Poll until endLoop == 1
            jeq     SWTrim

            mov.w   csCtl0Copy,&CSCTL0     ;Reload locked DCOTAP
            mov.w   csCtl1Copy,&CSCTL1     ;Reload locked DCOFTRIM

Unlock1     mov.w   &CSCTL7,R13
            and.w   #FLLUNLOCK0|FLLUNLOCK1,R13
            jnz     Unlock1                ; Check if FLL is locked

Go_on       mov.w   #SELMS__DCOCLKDIV+SELA__REFOCLK,&CSCTL4   ; set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                            ; default DCODIV as MCLK and SMCLK source

            bis.b   #BIT0+BIT3+BIT7,&P1DIR  ; set ACLK/SMCLK/MCLK pin
            bis.b   #BIT3+BIT7,&P1SEL1
            bis.b   #BIT2,&P2DIR
            bis.b   #BIT2,&P2SEL1

            ; Disable the GPIO power-on default high-impedance mode
            ; to activate previously configured port settings
            bic.w   #LOCKLPM5,PM5CTL0

Mainloop    xor.b   #BIT0,&P1OUT
            mov.w   #(65000), R13           ; Arbitrary delay to blink the LED
Delay1      sub.w   #1, R13
            jne     Delay1
            jmp     Mainloop                ; endless loop
            NOP
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            .end
