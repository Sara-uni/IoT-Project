#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "ledControl.h"

/* Timer_A Compare Configuration Parameter (PWM) per i LED */
Timer_A_CompareModeConfig compareConfig_PWM_Red = {
    TIMER_A_CAPTURECOMPARE_REGISTER_3,          // Usa CCR1 per il rosso
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disabilita interrupt
    TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output
    0                                          // Duty Cycle per il rosso
};

Timer_A_CompareModeConfig compareConfig_PWM_Green = {
    TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Usa CCR3 per il verde
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disabilita interrupt
    TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output
    0                                          // Duty Cycle per il verde
};

Timer_A_CompareModeConfig compareConfig_PWM_Blue = {
    TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Usa CCR1 per il blu
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disabilita interrupt
    TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output
    0                                          // Duty Cycle per il blu
};

/* Timer_A Up Configuration Parameter */
const Timer_A_UpModeConfig upConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK,                      // SMCLK = 3 MHz
    TIMER_A_CLOCKSOURCE_DIVIDER_5,                  // SMCLK/12 = 250 kHz
    10000,                                          // 40 ms tick period
    TIMER_A_TAIE_INTERRUPT_DISABLE,                 // Disabilita interrupt
    TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,            // Disabilita CCR0 interrupt
    TIMER_A_DO_CLEAR                                // Clear value
};

// Funzione che inizializza il PWM e i pin per i LED
void _ledInit() {
    // Configura i pin per il PWM
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);  // Rosso (P2.4)
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);  // Verde (P2.6)
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);  // Blu (P5.6)

    // Configura e avvia Timer_A per Up Mode
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

    // Inizializza i registri di confronto per generare PWM
    setColor(0, 0, 0);  // Imposta il colore iniziale a nero (tutti spenti)
}
