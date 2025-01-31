#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include <uart.h>
#include <init.h>

/* Variable for storing temperature value*/
float temp;

//obtain current time with milliseconds
void utcTime(char *buffer, size_t size){
    struct timeval tv;
    struct tm *tm_info;

    gettimeofday(&tv, NULL);
    tm_info = gmtime(&tv.tv_sec);

    int ms = tv.tv_usec / 1000;
    strftime(buffer, size, "%Y-%m-%dT%H:%M:%S", tm_info);

    snprintf(buffer + strlen(buffer), size - strlen(buffer), ".%03dZ", ms);
}

 
int main(void)
{

    _hwInit();

    while (1)
    {
        // Obtain temperature value and convert it into CELSIUS 
        temp = (TMP006_getTemp() - 32) / 1.8;

        char string[10];
        _showTextTemp(string, temp); //shows text on the display

        char time[100];
        utcTime(time, sizeof(time));

        /*questo è il primo tentativo che avevamo fatto per provare ad inviare i dati, i restanti
        tentativi li abbiamo provati su un progetto di prova per non rovinare questo qui
        ma non funzionava nulla lo stesso :(  */
        UART_sendString("temperature,");
        UART_sendString(string);
        UART_sendString(",");
        UART_sendString(time);
        UART_sendString("\n");

        /*l'idea è che mandi i dati in formato "type,value,time" e poi vengano convertiti
        nel formato JSON direttamente dal ESP32*/

    }
}
