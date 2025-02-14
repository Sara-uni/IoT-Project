#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include <stdio.h>

#include <uart.h>
#include <init.h>
#include <string.h>

bool red_led, green_led, blue_led;

void _initLED()
{
    P2DIR |= (BIT0 | BIT1 | BIT2);
    P2OUT &= ~(BIT0 | BIT1 | BIT2);
    red_led = false;
    green_led = false;
    blue_led = false;
}

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
    int16_t noise;
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

void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if(status & ADC_INT0)
    {
        noise = ADC14_getResult(ADC_MEM0);

    }

}

void sendLight()
{
    /* Obtain lux value from OPT3001 */
    float light = OPT3001_getLux();

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

void setColor(char *color)
{
    int r, g, b;
    sscanf(color, "%d,%d,%d", &r, &g, &b);
    // setLEDColor(r, g, b);
}

void turnRed(bool mode)
{
    red_led = mode;
    if (mode)
    {
        P2OUT |= BIT0;
    }
    else
    {
        P2OUT &= ~BIT0;
    }
}

void turnGreen(bool mode)
{
    green_led = mode;
    if (mode)
    {
        P2OUT |= BIT1;
    }
    else
    {
        P2OUT &= ~BIT1;
    }
}

void turnBlue(bool mode)
{
    blue_led = mode;
    if (mode)
    {
        P2OUT |= BIT2;
    }
    else
    {
        P2OUT &= ~BIT2;
    }
}

int main(void)
{
    _hwInit();
    _uartInit();
    _initLED();

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
            if (strcmp(command, "LED_ON") == 0)
            {
                UART_sendString("OK");
                turnRed(true);
                turnGreen(true);
                turnBlue(true);
            }
            if (strcmp(command, "LED_OFF") == 0)
            {
                UART_sendString("OK");
                turnRed(false);
                turnGreen(false);
                turnBlue(false);
            }
            if (strcmp(command, "SET_COLOR") == 0)
            {
                char colorCommand[20];
                UART_receiveString(colorCommand, sizeof(colorCommand));
                setColor(colorCommand);
            }
            if (strcmp(command, "GET_LED") == 0)
            {
                char response[40];
                if (red_led || green_led || blue_led)
                {
                    sprintf(response, "true,255,255,255");
                }
                else
                {
                    sprintf(response, "false,");
                }
                sprintf(response, "%s\n", response);
                _showText("STATUS: ", response);
                UART_sendString(response);
            }
        }
    }
}
