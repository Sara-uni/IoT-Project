#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

extern Timer_A_CompareModeConfig compareConfig_PWM_Red;
extern Timer_A_CompareModeConfig compareConfig_PWM_Green;
extern Timer_A_CompareModeConfig compareConfig_PWM_Blue;

// Dichiarazione della funzione setColor che viene utilizzata in ledControl.c
void setColor(int r, int g, int b);

// Funzione per l'inizializzazione dei LED
void _ledInit(void);

#endif /* LEDCONTROL_H_ */
