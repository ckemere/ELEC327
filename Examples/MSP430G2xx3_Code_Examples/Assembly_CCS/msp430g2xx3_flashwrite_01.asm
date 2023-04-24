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
;   MSP430G2xx3 Demo - Flash In-System Programming, Copy SegC to SegD
;
;   Description: This program first erases flash seg C, then it increments all
;   values in seg C, then it erases seg C, then copies seg C to seg D.
;   Assumed MCLK 771kHz - 1428kHz.
;   //* Set Breakpoint in the Mainloop to avoid Stressing Flash *//
;
;                MSP430G2xx3
;             -----------------
;         /|\|              XIN|-
;          | |                 |
;          --|RST          XOUT|-
;            |                 |
;
;
;   D. Dang
;   Texas Instruments Inc.
;   December 2010
;   Built with Code Composer Essentials Version: 4.2.0
;*******************************************************************************
value      .equ      R4
 .cdecls C,LIST,  "msp430.h"
;------------------------------------------------------------------------------
            .text                           ; Program Start
;------------------------------------------------------------------------------
RESET       mov.w   #0280h,SP               ; Initialize stackpointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
CheckCal    cmp.b   #0xFF,&CALBC1_1MHZ      ; Check calibration constant
            jne     Load                    ; if not erased, load.   
Trap        jmp     Trap                    ; if erased do not load, trap CPU!
Load        clr.b   &DCOCTL                 ; Select lowest DCOx and MODx settings
            mov.b   &CALBC1_1MHZ,&BCSCTL1   ; Set DCO to 1MHz
            mov.b   &CALDCO_1MHZ,&DCOCTL
            clr.b   value                   ; value = value to write to flash
                                            ;                                                   
Mainloop    call    #Write_SegC             ; Copy value to segment C
            inc.b   value                   ;
            call    #CopyC2D                ;
            jmp     Mainloop                ; Repeat,SET BREAKPOINT HERE
                                            ;
;-------------------------------------------------------------------------------
Write_SegC ;Input = value, holds value to write to Seg C, R5 used as working reg.
;-------------------------------------------------------------------------------
            mov.w   #01040h,R5              ;
Timing      mov.w   #FWKEY+FSSEL0+FN1,&FCTL2  ; *Timing generator = MCLK/3
Erase_SegC  mov.w   #FWKEY,&FCTL3           ; Lock = 0
            mov.w   #FWKEY+ERASE,&FCTL1     ; Erase bit = 1, allow interrupts
            mov.w   #0,&01040h              ; Dummy write to SegC to erase
Prog_SegC   mov.w   #FWKEY+WRT,&FCTL1       ; Write bit = 1, block interrupts
            mov.w   #FWKEY,&FCTL3           ; Lock = 0
Prog_L1     mov.b   value,0(R5)             ;
            inc.w   R5                      ;
            cmp.w   #01080h,R5              ;
            jne     Prog_L1                 ;
            mov.w   #FWKEY+LOCK,&FCTL3      ; Lock = 1;
            ret                             ;
                                            ;
;-------------------------------------------------------------------------------
CopyC2D    ;Copy Seg C to Seg D, R5 used as working reg.
;-------------------------------------------------------------------------------
            mov.w   #FWKEY+FSSEL0+FN1,&FCTL2  ; *Timing generator = MCLK/3
Erase_SegD  mov.w   #FWKEY,&FCTL3           ; Lock = 0
            mov.w   #FWKEY+ERASE,&FCTL1     ; Erase bit = 1
            mov.w   #0,&01000h              ; Dummy write to SegD to erase
            mov.w   #01040h,R5              ; R5 = First byte in Seg C
Prog_SegD   mov.w   #FWKEY+WRT,&FCTL1       ; Write bit = 1, block interrupts
            mov.w   #FWKEY,&FCTL3           ; Lock = 0
Prog_L2     mov.b   @R5+,-65(R5)            ; Copy Seg C to Seg D
            cmp.w   #01080h,R5              ;
            jne     Prog_L2                 ;
            mov.w   #FWKEY+LOCK,&FCTL3      ; Lock = 1
            ret

;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET                   ;
            .end
