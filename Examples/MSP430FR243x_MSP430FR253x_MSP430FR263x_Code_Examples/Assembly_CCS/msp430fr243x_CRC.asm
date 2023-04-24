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
;   MSP430FR24xx Demo - CRC16, Compare CRC output with software-based algorithm
;
;   Description: An array of 16 random 16-bit values are moved through the CRC
;   module, as well as a software-based CRC-CCIT-BR algorithm. Due to the fact
;   that the software-based algorithm handles 8-bit inputs only, the 16-bit
;   words are broken into 2 8-bit words before being run through (lower byte
;   first). The outputs of both methods are then compared to ensure that the
;   operation of the CRC module is consistent with the expected outcome. If
;   the values of each output are equal, set P1.0, else reset.
;
;   ACLK = default REFO ~32768Hz, MCLK = SMCLK = default DCODIV ~1MHz.
;
;                MSP430FR2433
;             -----------------
;         /|\|                 |
;          | |                 |
;          --|RST              |
;            |                 |
;            |             P1.0|--> LED
;
;   Cen Fang
;   Texas Instruments Inc.
;   August 2013
;   Built with Code Composer Studio v6.0
;******************************************************************************
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"  ; Include device header file
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack                  ; Make stack linker segment ?known?

            .text                           ; Assemble to Flash memory
            .retain                         ; Ensure current section gets linked
            .retainrefs

CRC_Init    .word   0xFFFF
i           .set    R5
CRC_Results .set    R6
SW_Results  .set    R7
CRC_New     .set    R8
LowByte     .set    R9
UpByte      .set    R10

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
            bis.b   #0x01,&P1DIR
            bic.b   #0x01,&P1OUT            ; Clear LED to start
            bic.w   #LOCKLPM5,PM5CTL0           ; Unlock I/O pins
            mov.w   CRC_Init,&CRCINIRES     ; Init CRC with 0xFFFF
            mov.w   #0x10,i                 ; load index to 16
            mov.w   #CRC_Input,R11          ; Start of input array
Hardware    mov.w   @R11,&CRCDIRB           ; Input data in CRC
            incd.w  R11                     ; Advance pointer in array
            nop
            dec.w   i                       ; Decrement index
            cmp.w   #0x00,i
            jne     Hardware                ; If more data, then input again
            mov.w   CRCINIRES,CRC_Results  ; Save results

            mov.w   #0x10,i                 ; load index to 16
            mov.w   #CRC_Input,R11          ; Start of input array
Software    mov.w   @R11,LowByte
            bic.w   #0xFF00,LowByte         ; Clear first 8-bits for just lower byte
            mov.w   @R11,UpByte
            swpb    UpByte                  ; swap bytes and clear first 8-bits for
            bic.w   #0xFF00,UpByte          ; just upper byte
            incd.w  R11                     ; Advance pointer in array
            cmp.w   #0x10,i
            jne     Reinit                  ; if not first run, reinit
            mov.w   CRC_Init,R12            ; First run, init with 0xFFFF
            mov.w   LowByte,R13             ; Input first byte
            calla   #CCITT_Update
            jmp     Upper
Reinit      mov.w   CRC_New,R12             ; Init with previous result
            mov.w   LowByte,R13             ; Input next byte
            calla   #CCITT_Update
Upper       mov.w   CRC_New,R12             ; Init with previous result
            mov.w   UpByte,R13              ; Input next byte
            calla   #CCITT_Update
            dec.w   i                       ; Decrement index
            jnz     Software                ; If more data, then input again
            mov.w   CRC_New,SW_Results      ; Save results

            cmp.w   CRC_Results,SW_Results  ; Was CRC successful?
            jeq     Success                 ; Yes, jump to success
            jmp     Mainloop                ; No, then don't turn on LED
Success     bis.b   #0x01,&P1OUT            ; Success - turn on LED
Mainloop    jmp     Mainloop                ; loop forever
            nop                             ; for debug
                                            ;
;-------------------------------------------------------------------------------
CRC_Input ; Random array of 16 16-bit values - values may be changed if desired
;-------------------------------------------------------------------------------
            .word   0x0fc0
            .word   0x1096
            .word   0x5042
            .word   0x0010
            .word   0x7ff7
            .word   0xf86a
            .word   0xb58e
            .word   0x7651
            .word   0x8b88
            .word   0x0679
            .word   0x0123
            .word   0x9599
            .word   0xc58c
            .word   0xd1e2
            .word   0xe144
            .word   0xb691
;-------------------------------------------------------------------------------
CCITT_Update ; Software algorithm - CCITT CRC16 code
;-------------------------------------------------------------------------------
             swpb   R12
             mov.w  R12,CRC_New
             xor.w  R13,CRC_New
             mov.w  CRC_New,R12
             bic.w  #0xFF00,R12
             rpt    #4
             rra    R12
             xor.w  R12,CRC_New
             mov.w  CRC_New,R12
             rpt    #12
             rla    R12
             xor.w  R12,CRC_New
             mov.w  CRC_New,R12
             bic.w  #0xFF00,R12
             rpt    #5
             rla    R12
             xor.w  R12,CRC_New
             reta
             nop
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
             .sect   RESET_VECTOR
             .short  RESET
             .end
