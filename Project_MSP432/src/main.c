#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include "HAL_OPT3001.h"
#include <stdio.h>
#include <uart.h>
#include <init.h>
#include <string.h>
#include <ledControl.h>
#include <noiseControl.h>

bool led = false;

void sendTemperature()
{
    // Obtain temperature value and convert it into CELSIUS
    float temp = (TMP006_getTemp() - 32) / 1.8;

    char tempStr[10];
    snprintf(tempStr, sizeof(tempStr), "%.2f", temp);

    char uartBuffer[128];
    snprintf(uartBuffer, sizeof(uartBuffer), "temperature,%s\n", tempStr);

    UART_sendString(uartBuffer);
}

void sendNoise()
{
    float noise = getNoise();

    char valuestr[10];
    snprintf(valuestr, sizeof(valuestr), "%.2f", noise);

    char uartBuffer[128];
    snprintf(uartBuffer, sizeof(uartBuffer), "noise,%s\n", valuestr);

    // send the string via UART
    UART_sendString(uartBuffer);
}

void sendLight()
{
    float light = OPT3001_getLux();

    char valuestr[10];
    snprintf(valuestr, sizeof(valuestr), "%.2f", light);

    char uartBuffer[128];
    snprintf(uartBuffer, sizeof(uartBuffer), "light,%s\n", valuestr);

    // send the string via UART
    UART_sendString(uartBuffer);
}

void setColor(int r, int g, int b)
{
    compareConfig_PWM_Red.compareValue = (r * 5000) / 255;
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM_Red);

    compareConfig_PWM_Green.compareValue = (g * 5000) / 255;
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM_Green);

    compareConfig_PWM_Blue.compareValue = (b * 5000) / 255;
    Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM_Blue);
}

int main(void)
{
    _hwInit();

    char command[60];
    while (1)
    {
        UART_receiveString(command, sizeof(command));
        if (sizeof(command) > 0)
        {
            if (strcmp(command, "GET_TEMP") == 0)
            {
                sendTemperature();
            }
            else if (strcmp(command, "GET_LIGHT") == 0)
            {
                sendLight();
            }
            else if (strcmp(command, "GET_NOISE") == 0)
            {
                sendNoise();
            }
            else if (strcmp(command, "LED_ON") == 0)
            {
                setColor(255, 255, 255);
                led = true;
                UART_sendString("led,OK\n");
            }
            else if (strcmp(command, "LED_OFF") == 0)
            {
                setColor(0, 0, 0);
                led = false;
                UART_sendString("led,OK\n");
            }
            else if (strncmp(command, "SET_COLOR", 9) == 0)
            {
                int r = 0, g = 0, b = 0;
                sscanf(command, "SET_COLOR(%d, %d, %d)", &r, &g, &b);

                setColor(r, g, b);
                led = true;
                UART_sendString("setColor,OK\n");
            }
            else if (strcmp(command, "GET_LED") == 0)
            {
                led ? UART_sendString("ledStatus,true\n") : UART_sendString("ledStatus,false\n");
            }
            else if (strncmp(command, "PRINT", 5) == 0)
            {
                char title[30], text[30];
                sscanf(command, "PRINT(%[^,], %[^)])", title, text);
                _showText(title, text);
            }
        }
    }
}
