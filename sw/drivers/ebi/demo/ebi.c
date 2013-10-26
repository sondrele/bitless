
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_ebi.h"
#include "em_cmu.h"
#include "em_dbg.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"

#if defined( BSP_DK_3200 )                          /* GxxxDK */
  #define EXT_SRAM_BASE_ADDRESS     ((volatile uint16_t*) BC_SRAM_BASE)

#elif defined( BSP_DK_3201 )                        /* DK3750, DK3650 or DK3550 */
  #define EXT_SRAM_BASE_ADDRESS     ((volatile uint16_t*) BC_PSRAM_BASE)

#endif

#define TEST_ARRAY_SIZE              32

/* Global variables */
uint16_t test[TEST_ARRAY_SIZE] = { 0x7BC1, 0x1EE2, 0x2E40, 0x9F96, 0xE93D, 0x7E11, 0x7393, 0x172A,
                                   0xAE2D, 0x8A57, 0x1E03, 0xAC9C, 0x9EB7, 0x6FAC, 0x45AF, 0x8E51,
                                   0x30C8, 0x1C46, 0xA35C, 0xE411, 0xE5FB, 0xC119, 0x1A0A, 0x52EF,
                                   0xF69F, 0x2445, 0xDF4F, 0x9B17, 0xAD2B, 0x417B, 0xE66C, 0x3710 };

uint16_t answer[TEST_ARRAY_SIZE] = { 0x0000, 0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 
                                     0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF,
                                     0x0000, 0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 
                                     0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF};

/* Counts 1ms timeTicks */
volatile uint32_t msTicks;

/* Local prototypes */
void Delay(uint32_t dlyTicks);

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}
/**************************************************************************//**
 * @brief
 *   Main function.
 *****************************************************************************/
int main(void)
{
    bool error = false;
    uint32_t i;

    /* Chip revision alignment and errata fixes */
    CHIP_Init();

    /* Initialize DK and EBI, this function also calls the EbiInit(void) */
    /* function which configures the EFM32 EBI interface and enables the */
    /* pins for 16 bit multiplex mode. */
    BSP_Init(BSP_INIT_DEFAULT);

    /* Wait for AEM state to give access to the EFM32. */
    /* AEM state is toggled by pressing the AEM button on the DK. */
    /* The bsp package for the different DK's unfortunately has different */
    /* defines for the registers. */
#if defined( BSP_DK_3200 )                          /* GxxxDK */

    while(BSP_RegisterRead (BC_AEMSTATE) != BC_AEMSTATE_EFM);

#elif defined( BSP_DK_3201 )                        /* DK3750, DK3650 or DK3550 */

    while(BSP_RegisterRead (&BC_REGISTER->UIF_AEM) != BC_UIF_AEM_EFM);

#endif

    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

    /* Write external SRAM */
    for (i=0 ; i<TEST_ARRAY_SIZE ; i++) {
        /* Write data in the External SRAM */
        *(uint16_t*)(EXT_SRAM_BASE_ADDRESS + i) = test[i];
    }

    /* Read external SRAM*/
    for (i=0 ; i<TEST_ARRAY_SIZE; i++) {
        /* Read data from External SRAM */
        answer[i] = *(uint16_t*)(EXT_SRAM_BASE_ADDRESS + i);
    }


    /* Test the difference between buffers. */
    for (i = 0; i < TEST_ARRAY_SIZE; i++) {
        if (test[i] != answer[i]) {
            error = true;
        }
    }

    if (error) {
    /* Write and Read operation  FAILED */
        while (1) {
            BSP_LedsSet(0x00ff);
            Delay(500);
            BSP_LedsSet(0xff00);
        }
    }
    else {
        /* Write and Read operation  SUCCESS */
        while (1) {
            BSP_LedsSet(0x00ff);
        }
    }
}
