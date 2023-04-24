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
//   MSP430FR243x Demo - Long word writes to FRAM
//
//   Description: Use long word write to write to 512 byte blocks of FRAM.
//   Toggle LED after every 100 writes.
//   ACLK = REFO, MCLK = SMCLK = default DCODIV = ~1MHz
//
//   It is recommended to disable interrupt while code is writing to main FRAM,
//   because it is dangerous for running the interrupt services routine with 
//   PFWP disabled.
//   This code examples is using DFWP which wouldn't have dangerous. But it is 
//   recommended to disable interrupt if user is using PFWP.
//
//           MSP430FR2433
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |          P1.0 |---> LED
//
//   Ling Zhu
//   Texas Instruments Inc.
//   Feb 2015
//   Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

void FRAMWrite(void);

unsigned char count = 0;
unsigned long *FRAM_write_ptr;
unsigned long data;

#define FRAM_TEST_START 0x1800


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output directionOUT

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    data = 0x11111111;                      // Initialize dummy data

    while(1)
    {
        data += 0x00010001;
        FRAM_write_ptr = (unsigned long *)FRAM_TEST_START;
        FRAMWrite();
        count++;
        if (count > 100)
        {
            P1OUT ^= 0x01;                  // Toggle LED to show 512 bytes
            count = 0;                      // have been written
            data = 0x11111111;
        }
    }
}

void FRAMWrite (void)
{
    unsigned int i=0;

    SYSCFG0 = FRWPPW | PFWP;
    for (i = 0; i < 128; i++)
    {
        *FRAM_write_ptr++ = data;
    }
    SYSCFG0 = FRWPPW | PFWP | DFWP;
}
