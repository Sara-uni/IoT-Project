#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include <stdio.h>

#include <uart.h>
#include <init.h>

// Variable for storing temperature value
// float temp;

void sendTemperature()
{
    // Obtain temperature value and convert it into CELSIUS
    float temp = (TMP006_getTemp() - 32) / 1.8;

    char string[10];
    _showTextTemp(string, temp); // shows text on the display

    char tempStr[10];
    snprintf(tempStr, sizeof(string), "%.2f", temp);

    char uartBuffer[128];
    snprintf(uartBuffer, sizeof(uartBuffer), "temperature,%s\n", tempStr);

    // send the string via UART
    UART_sendString(uartBuffer);
}

int main(void)
{
    _hwInit();
    _uartInit();

    char command[20];
    while (1)
    {
        if (UART_receiveString(command, sizeof(command)))
        {
            if (strcmp(command, "GET_TEMP") == 0)
            {
                sendTemperature();
            }
        }
    }
}
