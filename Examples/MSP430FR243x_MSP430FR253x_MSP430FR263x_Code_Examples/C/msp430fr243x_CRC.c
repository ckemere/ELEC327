/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430FR243x Demo - CRC16, Compare CRC output with software-based algorithm
//
//   Description: An array of 16 random 16-bit values are moved through the CRC
//   module, as well as a software-based CRC-CCIT-BR algorithm. Due to the fact
//   that the software-based algorithm handles 8-bit inputs only, the 16-bit
//   words are broken into two 8-bit words before being run through (lower byte
//   first). The outputs of both methods are then compared to ensure that the
//   operation of the CRC module is consistent with the expected outcome. If
//   the values of each output are equal, set P1.0, else reset.
//
//   ACLK = default REFO ~32768Hz, MCLK = SMCLK = default DCODIV ~1MHz.
//
//                MSP430FR2433
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |             P1.0|--> LED
//
//   Cen Fang
//   Texas Instruments Inc.
//   June 2013
//   Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

unsigned int CRC_Init = 0xFFFF;
unsigned int i;
unsigned int CRC_Input[]= {0x0fc0, 0x1096, 0x5042, 0x0010,  // 16 random 16-bit numbers
                           0x7ff7, 0xf86a, 0xb58e, 0x7651,  // these numbers can be
                           0x8b88, 0x0679, 0x0123, 0x9599,  // modified if desired
                           0xc58c, 0xd1e2, 0xe144, 0xb691 };
unsigned int CRC_Results;
unsigned int SW_Results;
unsigned int CRC_New;

// Software Algorithm Function Definition
unsigned int CCITT_Update(unsigned int, unsigned int);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    P1OUT &= ~BIT0;                         // Clear P1.0 output state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    // Init CRC
    CRCINIRES = CRC_Init;                   // Init CRC with 0xFFFF

    for(i=0;i<16;i++)
    {
        // Input random values into CRC Hardware
        CRCDIRB = CRC_Input[i];             // Input data in CRC
        __no_operation();
    }
    CRC_Results = CRCINIRES;                // Save results (per CRC-CCITT standard)

    CRC_New = CRC_Init;                     // Put CRC Init value into CRC_New
    for(i = 0; i < 16; i++)
    {
        // Input values into Software algorithm (requires 8-bit inputs)
        // Clear upper 8 bits to get lower byte
        unsigned int LowByte = (CRC_Input[i] & 0x00FF);
        // Shift right 8 bits to get upper byte
        unsigned int UpByte = (CRC_Input[i] >> 8);
        // First input lower byte
        CRC_New = CCITT_Update(CRC_New,LowByte);
        // Then input upper byte
        CRC_New = CCITT_Update(CRC_New,UpByte);
    }
    SW_Results = CRC_New;

    // Compare data output results
    if(CRC_Results==SW_Results)             // if data is equal
        P1OUT |= BIT0;                      // set the LED
    else
        P1OUT &= ~BIT0;                     // if not, clear LED

    while(1);                               // infinite loop
}

// Software algorithm - CCITT CRC16 code
unsigned int CCITT_Update(unsigned int init, unsigned int input)
{
    unsigned int CCITT;
    CCITT =(unsigned char)(init >> 8)|(init << 8);
    CCITT ^= input;
    CCITT ^= (unsigned char)(CCITT & 0xFF) >> 4;
    CCITT ^= (CCITT << 8) << 4;
    CCITT ^= ((CCITT & 0xFF) << 4) << 1;
    return CCITT;
}
