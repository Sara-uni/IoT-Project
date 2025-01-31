#include "uart.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

//------------------UART config----------------------------
void _uartInit()
{
    /* Configura i pin per UART (P3.2 -> TX, P3.3 -> RX) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                                               GPIO_PIN2 | GPIO_PIN3,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configura eUSCI_A0 per UART a 115200 baud */
    const eUSCI_UART_Config uartConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,           // SMCLK Clock Source
        13,                                      // BRDIV = 13
        0,                                       // UCxBRF = 0
        37,                                      // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
    };


    /* Inizializza UART */
    UART_initModule(EUSCI_A2_BASE, &uartConfig);
    UART_enableModule(EUSCI_A2_BASE);
}

void UART_sendString(char *str)
{
    while (*str)
    {
        UART_transmitData(EUSCI_A2_BASE, *str);
        str++;
    }
}