#include "init.h"
#include "uart.h"
#include "ledControl.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include <stdio.h>

//-----------------TEMPERATURE---------------------------

void _temperatureSensorInit()
{
    /* Temperature Sensor initialization */
    /* Initialize I2C communication */
    Init_I2C_GPIO();
    I2C_init();
    /* Initialize TMP006 temperature sensor */
    TMP006_init();
    __delay_cycles(100000);
}

//-----------------MICROPHONE----------------------------

void _micInit(){
    /* Initializing ADC (MCLK/1/1) */
      ADC14_enableModule();
      ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_NOROUTE);


      /* Configuring GPIOs (4.3 A10) */
      GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN3,
                                                 GPIO_TERTIARY_MODULE_FUNCTION);

      /* Configuring ADC Memory */
      ADC14_configureSingleSampleMode(ADC_MEM0, true);
      ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                      ADC_INPUT_A10, false);

      /* Set ADC result format to signed binary */
      ADC14_setResultFormat(ADC_SIGNED_BINARY);

      /* Enabling manual iteration mode and interrupts */
      ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
      ADC14_enableInterrupt(ADC_INT0);

      /* Enabling Interrupts */
      Interrupt_enableInterrupt(INT_ADC14);
      Interrupt_enableMaster();

      /* Triggering the start of the sample */
      ADC14_enableConversion();
      ADC14_toggleConversionTrigger();
}

//---------------------------------------------------

Graphics_Context g_sContext;

void _graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

void _hwInit()
{
    /* Halting WDT and disabling master interrupts */
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    /* Set the core voltage level to VCORE1 */
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1*/
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Initializes Clock System */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Configuring P1.0 as output */
    // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    // GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    
    _graphicsInit();
    _temperatureSensorInit();
    _micInit();
    _uartInit();
    _ledInit();
}

void _showText(char *title, char *text)
{
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *)title, AUTO_STRING_LENGTH, 65, 50,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *)text, AUTO_STRING_LENGTH, 65, 70,
                                OPAQUE_TEXT);
}