#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

extern Timer_A_CompareModeConfig compareConfig_PWM_Red;
extern Timer_A_CompareModeConfig compareConfig_PWM_Green;
extern Timer_A_CompareModeConfig compareConfig_PWM_Blue;

void setColor(int r, int g, int b);

void _ledInit(void);

#endif
