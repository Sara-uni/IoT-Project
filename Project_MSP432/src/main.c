#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include <stdio.h>

#include <uart.h>
#include <init.h>
#include <string.h>

// Variable for storing temperature value
// float temp;

void sendTemperature()
{
    // Obtain temperature value and convert it into CELSIUS
    float temp = (TMP006_getTemp() - 32) / 1.8;

    char string[20];
    sprintf(string, "%.2f C", temp);
    _showText("Temperature:", string);

    char tempStr[10];
    snprintf(tempStr, sizeof(string), "%.2f", temp);

    char uartBuffer[128];
    snprintf(uartBuffer, sizeof(uartBuffer), "temperature,%s\n", tempStr);

    UART_sendString(uartBuffer);
}

void sendNoise()
{
    float noise = 55.94;

    char string[20];
    sprintf(string, "%.2f Db", noise);
    _showText("Noise:", string);

    char valuestr[10];
    snprintf(valuestr, sizeof(string), "%.2f", noise);

    char uartBuffer[128];
    snprintf(uartBuffer, sizeof(uartBuffer), "noise,%s\n", valuestr);

    // send the string via UART
    UART_sendString(uartBuffer);
}

void sendLight()
{
    float light = ;

    char string[20];
    sprintf(string, "%.2f lux", light);
    _showText("Light:", string);

    char valuestr[10];
    snprintf(valuestr, sizeof(string), "%.2f", light);

    char uartBuffer[128];
    snprintf(uartBuffer, sizeof(uartBuffer), "light,%s\n", valuestr);

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
        UART_receiveString(command, sizeof(command));
        if (command)
        {
            if (strcmp(command, "GET_TEMP") == 0)
            {
                sendTemperature();
            }
            if (strcmp(command, "GET_LIGHT") == 0)
            {
                sendLight();
            }
            if (strcmp(command, "GET_NOISE") == 0)
            {
                sendNoise();
            }
        }
    }
}
