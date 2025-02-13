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

void setColor(char *color)
{
    int r = 0, g = 0, b = 0;
    sscanf(color, "%d, %d, %d", &r, &g, &b);  // Estrae i valori di r, g, b dalla stringa

    // Configura i pin P2.0 (rosso), P2.1 (verde), P2.2 (blu) come uscite PWM
    P2->DIR |= (BIT0 | BIT1 | BIT2);      // Imposta i pin come output
    P2->SEL0 |= (BIT0 | BIT1 | BIT2);     // Seleziona la funzione PWM
    P2->SEL1 &= ~(BIT0 | BIT1 | BIT2);    // Assicura che sia selezionata la funzione alternativa 1 (PWM)

    // Configura Timer_A0 per controllare i tre LED
    TIMER_A0->CCR[0] = 1000 - 1;          // Imposta il periodo del PWM (1000 cicli)

    // Configura PWM per il LED rosso su CCR1
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // PWM reset/set
    TIMER_A0->CCR[1] = (int)(r * 3.92);   // Imposta il duty cycle per il rosso (0-100%)

    // Configura PWM per il LED verde su CCR2
    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7; // PWM reset/set
    TIMER_A0->CCR[2] = (int)(g * 3.92);   // Imposta il duty cycle per il verde (0-100%)

    // Configura PWM per il LED blu su CCR3
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7; // PWM reset/set
    TIMER_A0->CCR[3] = (int)(b * 3.92);   // Imposta il duty cycle per il blu (0-100%)

    // Avvia Timer_A in modalità up
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1;  // Usa SMCLK, modalità up
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
            if (strcmp(command, "SET_COLOR") == 0)
            {
                char colorCommand[20];
                UART_receiveString(colorCommand, sizeof(colorCommand));
                // setColor(colorCommand);
                turnLed(BIT0, true);
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
