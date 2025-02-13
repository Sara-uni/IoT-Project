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
    float light = 1543.49;

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

void turnLed(uint8_t pin, bool mode) {
    if (pin == BIT0) {
        red_led = mode;
    } else if (pin == BIT1) {
        green_led = mode;
    } else if (pin == BIT2) {
        blue_led = mode;
    }
    if (mode) {
        P2OUT |= pin;
    } else {
        P2OUT &= ~pin;
    }
}

void setColor(int red, int green, int blue)
{
    P2->DIR |= (BIT0 | BIT1 | BIT2);
    P2->SEL0 |= (BIT0 | BIT1 | BIT2);
    P2->SEL1 &= ~(BIT0 | BIT1 | BIT2);

    TIMER_A0->CCR[0] = 1000 - 1;

    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[1] = (int)(red * 3.92);

    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[2] = (int)(green * 3.92);

    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[3] = (int)(blue * 3.92);
    
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1;
}

int main(void)
{
    _hwInit();
    _uartInit();
    _initLED();

    char command[30];
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
                UART_sendString("OK\n");
                turnLed(BIT0, true);
                turnLed(BIT1, true);
                turnLed(BIT2, true);
            }
            if (strcmp(command, "LED_OFF") == 0)
            {
                UART_sendString("OK\n");
                turnLed(BIT0, false);
                turnLed(BIT1, false);
                turnLed(BIT2, false);
            }
            if (strncmp(command, "SET_COLOR", 9) == 0)
            {
                int r = 0, g = 0, b = 0;
                sscanf(command, "SET_COLOR(%d, %d, %d)", &r, &g, &b);
                
                setColor(r, g, b);
                UART_sendString("OK\n");
            }
            if (strcmp(command, "GET_LED") == 0)
            {
                char response[40];
                if (red_led || green_led || blue_led)
                {
                    sprintf(response, "true");
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
