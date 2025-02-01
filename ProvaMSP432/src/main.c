/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 UART - Loopback with 24MHz DCO BRCLK (utilizzando EUSCI_A0)
 *
 * Description: Questo demo collega il TX al RX della UART del MSP432.
 * Il codice dimostra la corretta inizializzazione dei registri ed
 * l'uso degli interrupt per ricevere e trasmettere dati. Se i dati
 * ricevuti non corrispondono a quelli inviati, il LED su P1.0 viene acceso.
 *
 *  MCLK = HSMCLK = SMCLK = DCO a 24MHz
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *       RST -|     P1.3/UCA0TXD|----|
 *            |                 |    |
 *           -|                 |    |
 *            |     P1.2/UCA0RXD|----|
 *            |                 |
 *            |             P1.0|---> LED
 *            |                 |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

uint8_t TXData = 55;
uint8_t RXData = 0;

/* Configurazione della UART:
 * I parametri qui indicati permettono a EUSCI_A0 di operare a 115200 baud rate.
 * I valori sono stati calcolati utilizzando l'apposito tool online fornito da TI:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_Config uartConfig =
{
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // Sorgente di clock SMCLK
    13,                                      // BRDIV = 13
    0,                                       // UCxBRF = 0
    37,                                      // UCxBRS = 37
    EUSCI_A_UART_NO_PARITY,                  // Nessuna parità
    EUSCI_A_UART_LSB_FIRST,                  // MSB First
    EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
    EUSCI_A_UART_MODE,                       // Modalità UART
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling abilitato
    //EUSCI_A_UART_8_BIT_LEN                   // 8 bit di lunghezza dati
};

void UART_sendString(char* str) {
    // Cicla attraverso ogni carattere della stringa
    while (*str != '\0') {
        // Invia il carattere via UART
        UART_transmitData(EUSCI_A0_BASE, *str);
        // Aspetta che il carattere venga trasmesso prima di passare al successivo
        while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        str++;  // Passa al prossimo carattere
    }
}

int main(void)
{
    /* Fermare il WDT */
    WDT_A_holdTimer();

    /* Configurazione dei pin:
     * - P1.2 e P1.3 configurati per la modalità periferica UART.
     * - P1.0 configurato come uscita (LED).
     */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
             GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Impostazione del DCO a 24MHz (necessario aumentare il Vcore) -> CPU a 24 MHz! */
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);

    /* Configurazione del modulo UART su EUSCI_A0 */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Abilitare il modulo UART */
    UART_enableModule(EUSCI_A0_BASE);

    /* Abilitare gli interrupt UART per la ricezione */
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);
    Interrupt_enableSleepOnIsrExit();

    while(1)
    {
        /* Trasmissione dati */
        //UART_transmitData(EUSCI_A0_BASE, TXData);
        UART_sendString("temperature,23.7,2025-01-28T12:34:56.789Z\n");
        UART_sendString("temperature,12.3,1939-12-27T06:23:45.112Z\n");
    }
}

/* ISR per EUSCI_A0 UART - Esegue l'eco dei dati ricevuti */ /*
void EUSCIA0_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        RXData = UART_receiveData(EUSCI_A0_BASE);

        if(RXData != TXData)              // Verifica del dato ricevuto
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            while(1);                   // Arresta la CPU in caso di errore
        }
        TXData++;
        Interrupt_disableSleepOnIsrExit();
    }
}*/
