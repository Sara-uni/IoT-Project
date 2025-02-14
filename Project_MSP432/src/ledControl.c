#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "ledControl.h"

/* Timer_A Compare Configuration Parameter (PWM) for the LEDs */
Timer_A_CompareModeConfig compareConfig_PWM_Red = {
    TIMER_A_CAPTURECOMPARE_REGISTER_3,
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
    TIMER_A_OUTPUTMODE_TOGGLE_SET,
    0                                          // Duty Cycle for red LED
};

Timer_A_CompareModeConfig compareConfig_PWM_Green = {
    TIMER_A_CAPTURECOMPARE_REGISTER_1,
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
    TIMER_A_OUTPUTMODE_TOGGLE_SET,
    0                                          // Duty Cycle for green LED
};

Timer_A_CompareModeConfig compareConfig_PWM_Blue = {
    TIMER_A_CAPTURECOMPARE_REGISTER_1,
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
    TIMER_A_OUTPUTMODE_TOGGLE_SET,
    0                                          // Duty Cycle for blue LED
};

// Timer_A Up Configuration Parameter
const Timer_A_UpModeConfig upConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK,                      // SMCLK = 3 MHz
    TIMER_A_CLOCKSOURCE_DIVIDER_5,                  // SMCLK/5 = 600 kHz
    10000,                                          // 16.667 ms tick period
    TIMER_A_TAIE_INTERRUPT_DISABLE,
    TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
    TIMER_A_DO_CLEAR
};

// Function that initialize the PWM and LED pins
void _ledInit() {
    // Set the PWM pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);  // Red (P2.4)
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);  // Green (P2.6)
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);  // Blue (P5.6)

    // Set and start timer with Up mode
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

    // Initialize the led with a 0 Duty Cycle (turned off)
    setColor(0, 0, 0);
}
