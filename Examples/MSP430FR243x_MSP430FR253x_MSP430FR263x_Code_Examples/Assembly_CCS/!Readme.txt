File Name                             Description
----------------------------------------------------------------------------------------
msp430fr243x_1.asm                    Toggle P1.0 using software
msp430fr243x_adc10_01.asm             ADC, Sample A1, AVcc Ref, Set LED if A1 > 0.5*AVcc
msp430fr243x_adc10_02.asm             ADC, Sample A1, internal 1.5V Ref, Set P1.0 if A1 > 0.5V
msp430fr243x_adc10_10.asm             ADC, Sample A2/A1/A0, internal 1.5V Ref.
msp430fr243x_adc10_11.asm             ADC, Sample A1, internal 1.5V Ref, TA0.1 Trig, Set P1.0 if A1>0.5V
msp430fr243x_adc10_21.asm             ADC, Window Comparator, 1.5V ref, Timer trigger, DCO
msp430fr243x_CRC.asm                  CRC16, Compare CRC output with software-based algorithm
msp430fr243x_CS_01.asm                Configure MCLK for 8MHz sourced from DCO.
msp430fr243x_CS_02.asm                Configure MCLK for 8MHz and XT1 sourcing ACLK and FLLREF
msp430fr243x_CS_03.asm                Configure MCLK for 16MHz operation, and REFO sourcing FLLREF and ACLK
msp430fr243x_CS_05.asm                Configure MCLK for 16MHz operation, and XT1 sourcing FLLREF and ACLK
msp430fr243x_euscia0_uart_01.asm      eUSCI_A0 UART echo at 9600 baud using BRCLK = 8MHz
msp430fr243x_euscia0_uart_03.asm      USCI_A0 External Loopback test @ 115200 baud
msp430fr243x_framwrite.asm            Long word writes to FRAM
msp430fr243x_LPM3_01.asm              Enters LPM3 with ACLK = XT1CLK = 32768Hz.
msp430fr243x_mpy_01.asm               16x16 Unsigned Multiply
msp430fr243x_mpy_10.asm               32x32 Unsigned Multiply
msp430fr243x_mpy_11.asm               32x32 Unsigned Multiply Accumulate
msp430fr243x_P1_01.asm                Software Poll P1.3, Set P1.0 if P1.3 = 1
msp430fr243x_P1_03.asm                Software Port Interrupt Service on P1.3 from LPM3
msp430fr243x_ta0_01.asm               Timer0_A3, Toggle P1.0, CCR0 Count Mode ISR, DCO SMCLK
msp430fr243x_ta0_02.asm               Timer0_A3, Toggle P1.0, CCR0 Up Mode ISR, DCO SMCLK
msp430fr243x_ta0_04.asm               Timer0_A3, Toggle P1.0, Overflow ISR, 32kHz ACLK
msp430fr243x_ta0_16.asm               Timer0_A3, PWM TA0.1-2, Up Mode, DCO SMCLK
msp430fr243x_ta0_17.asm               Timer0_A3, PWM TA0.1-2, Up Mode, 32KHz ACLK
msp430fr243x_ta0_20.asm               Timer0_A3, PWM TA0.1-2, Up/Down Mode, 32kHz ACLK
msp430fr243x_ta0_22.asm               Timer_A, Ultra-Low Pwr Pulse Accumulator
msp430fr243x_ta0_capture.asm          Timer0_A3 Capture of ACLK
msp430fr243x_ta1_05.asm               Timer1_A3, Toggle P1.0, CCR0 Cont Mode ISR, 32KHz ACLK
msp430fr243x_ta1_06.asm               Timer1_A3, Toggle P1.0, CCR0 UP Mode ISR, 32KHz ACLK
msp430fr243x_ta1_16.asm               Timer1_A3, PWM TA1.1-2, Up Mode, DCO SMCLK
msp430fr243x_ta1_17.asm               Timer1_A3, PWM TA1.1-2, Up Mode, 32KHz ACLK
msp430fr243x_uscia0_spi_09.asm        eUSCI_A0, SPI 3-Wire Master Incremented Data
msp430fr243x_uscia0_spi_10.asm        eUSCI_A0, SPI 3-Wire Slave Data Echo
msp430fr243x_uscia0_spi_11.asm        eUSCI_A0, SPI 4-Wire Master Incremented Data
msp430fr243x_uscia0_spi_12.asm        eUSCI_A0, SPI 4-Wire Slave Data Echo
msp430fr243x_WDT_01.asm               WDT, Toggle P1.0, Interval Overflow ISR, DCO SMCLK
msp430fr243x_WDT_02.asm               WDT, Toggle P1.0, Interval Overflow ISR, 32kHz ACLK
msp430fr243x_WDT_04.asm               WDT,  Failsafe Clock, WDT mode, DCO SMCLK
msp430fr243x_WDT_05.asm               WDT,  Reset on Invalid Address fetch, Toggle P1.0
msp430fr243x_WDT_06.asm               WDT,  Failsafe Clock, WDT mode, 32kHz ACLK

