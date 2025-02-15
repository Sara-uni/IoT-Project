#include <math.h>
#include <stdint.h>
#include <string.h>
#include <adc14.h>
#include <stdio.h>
#include <init.h>
#include "noiseControl.h"

volatile int16_t noise = 0;
volatile bool newDataAvailable = false;

int16_t requestNoiseMeasurement()
{
    ADC14_toggleConversionTrigger();
    while (!newDataAvailable)
    {
        __sleep();
    }

    newDataAvailable = false;
}

float getNoise()
{
    requestNoiseMeasurement();
    if (noise == 0)
        return 0.0;
    // v_ref = 1 so we have to do the logarithm only
    return 20.0 * log10(fabs((double)noise));
}

void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if (status & ADC_INT0)
    {
        noise = ADC14_getResult(ADC_MEM0);
        newDataAvailable = true;
    }
}