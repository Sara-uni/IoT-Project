#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include <stdio.h>

#include <uart.h>
#include <init.h>
#include <string.h>
#include <ledControl.h>

int red_led = 0, green_led = 0, blue_led = 0;

void sendTemperature()
{
    // Obtain temperature value and convert it into CELSIUS
    float temp = (TMP006_getTemp() - 32) / 1.8;
    
    char string[20];
    sprintf(string, "%.2f C", temp);
    _showText("Temperature:", string);
    
    char tempStr[10];
    snprintf(tempStr, sizeof(tempStr), "%.2f", temp);
    
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
    snprintf(valuestr, sizeof(valuestr), "%.2f", noise);
    
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
    snprintf(valuestr, sizeof(valuestr), "%.2f", light);
    
    char uartBuffer[128];
    snprintf(uartBuffer, sizeof(uartBuffer), "light,%s\n", valuestr);
    
    // send the string via UART
    UART_sendString(uartBuffer);
}

void setColor(int r, int g, int b) {
    compareConfig_PWM_Red.compareValue = r;   // Setta il valore PWM per il rosso
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM_Red);

    compareConfig_PWM_Green.compareValue = g; // Setta il valore PWM per il verde
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM_Green);

    compareConfig_PWM_Blue.compareValue = b;  // Setta il valore PWM per il blu
    Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM_Blue);
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



int main(void)
{
    _hwInit();
    _uartInit();

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
                UART_sendString("OK\n");
                turnLed(BIT0, true);
                turnLed(BIT1, true);
                turnLed(BIT2, true);
            }
            else if (strcmp(command, "LED_OFF") == 0)
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
            else if (strcmp(command, "GET_LED") == 0)
            {
                char response[40];
                if (red_led || green_led || blue_led)
                {
                    sprintf(response, "true,\n");
                }
                else
                {
                    sprintf(response, "false,\n");
                }
                sprintf(response, "%s\n", response);
                UART_sendString(response);
            }
        }
    }
}
