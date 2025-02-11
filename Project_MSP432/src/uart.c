#include "uart.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

//------------------UART config----------------------------

const eUSCI_UART_Config uartConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // Sorgente di clock SMCLK
        13,                                            // BRDIV = 13
        0,                                             // UCxBRF = 0
        37,                                            // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                        // Nessuna parità
        EUSCI_A_UART_LSB_FIRST,                        // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
        EUSCI_A_UART_MODE,                             // Modalità UART
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling abilitato
                                                       // EUSCI_A_UART_8_BIT_LEN                   // 8 bit di lunghezza dati
};

void _uartInit()
{
    WDT_A_holdTimer();

    /* Configurazione dei pin:
     * - P1.2 e P1.3 configurati per la modalità periferica UART.
     * - P1.0 configurato come uscita (LED).
     */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

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
}

void UART_sendString(char *str)
{
    while (*str != '\0')
    {
        UART_transmitData(EUSCI_A0_BASE, *str);
        while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG))
            ;
        str++;
    }
}

void UART_receiveString(char *buffer, int maxLength)
{
    uint16_t index = 0;

    while (index < (maxLength - 1))
    {
        while (!(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG))
            ;
        char receivedChar = UART_receiveData(EUSCI_A0_BASE);
        if (receivedChar == '\n' || receivedChar == '\r')
        {
            break;
        }
        buffer[index++] = receivedChar;
    }

    buffer[index] = '\0';
}
