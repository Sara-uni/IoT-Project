#include <math.h>
#include <stdint.h>
#include <string.h>
#include <adc14.h>
#include <stdio.h>
#include <init.h>
#include "noiseControl.h"

#define ADC_RESOLUTION 16383
#define VREF 3.3
#define MIC_REFERENCE_VOLTAGE 0.001

uint16_t noise = 0;

int16_t requestNoiseMeasurement()
{
    ADC14_toggleConversionTrigger();
    while (!ADC14_isBusy())
        ;
    return ADC14_getResult(ADC_MEM0);
}

float getNoise()
{
    float noise = requestNoiseMeasurement();
    const int adc_max = 8191;
    if (noise == 0)
        return 0.0;

    double normalized = (double)adc_max / fabs((double)noise);
    return 20.0 * log10(normalized);
}

// void ADC14_IRQHandler(void)
// {
//     uint64_t status;

//     status = ADC14_getEnabledInterruptStatus();
//     ADC14_clearInterruptFlag(status);

//     if (status & ADC_INT0)
//     {
//         noise = ADC14_getResult(ADC_MEM0);
//         char string[20];
//         sprintf(string, "%.2f Db", getNoise());
//         _showText("Noise INT:", string);
//     }
// }