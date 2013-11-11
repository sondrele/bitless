#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_system.h"
#include "em_timer.h"
#include "em_letimer.h"
#include "em_chip.h"

void setupSWO(void)
{
  /* Enable GPIO Clock. */
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
  /* Enable Serial wire output pin */
  GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;
#if defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY) || defined(_EFM32_LEOPARD_FAMILY)
  /* Set location 0 */
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC0;

  /* Enable output on pin - GPIO Port F, Pin 2 */
  GPIO->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
  GPIO->P[5].MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;
#else
  /* Set location 1 */
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC1;
  /* Enable output on pin */
  GPIO->P[2].MODEH &= ~(_GPIO_P_MODEH_MODE15_MASK);
  GPIO->P[2].MODEH |= GPIO_P_MODEH_MODE15_PUSHPULL;
#endif
  /* Enable debug clock AUXHFRCO */
  CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;

  while(!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY));

  /* Enable trace in core debug */
  CoreDebug->DHCSR |= 1;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  /* Enable PC and IRQ sampling output */
  DWT->CTRL = 0x400113FF;
  /* Set TPIU prescaler to 16. */
  TPI->ACPR = 0xf;
  /* Set protocol to NRZ */
  TPI->SPPR = 2;
  /* Disable continuous formatting */
  TPI->FFCR = 0x100;
  /* Unlock ITM and output data */
  ITM->LAR = 0xC5ACCE55;
  ITM->TCR = 0x10009;
}


void TIMER0_IRQHandler( void )
{
	TIMER_IntClear( TIMER0, TIMER_IF_OF );

	//GPIO_PinOutToggle( gpioPortE, 2 );
	//GPIO_PinOutToggle( gpioPortE, 3 );
	GPIO_PinOutToggle( gpioPortC, 0 );
	GPIO_PinOutToggle( gpioPortD, 0 );
}

void setupTIMER( void ) 
{

	CMU_ClockEnable( cmuClock_GPIO, true );
	CMU_ClockEnable( cmuClock_HFPER, true );
	CMU_ClockEnable( cmuClock_TIMER0, true );

	//GPIO_PinModeSet( gpioPortE, 2, gpioModePushPullDrive, 0 );
	//GPIO_PinModeSet( gpioPortE, 3, gpioModePushPullDrive, 0 );
	GPIO_PinModeSet( gpioPortD, 0, gpioModePushPullDrive, 0 );
	GPIO_PinModeSet( gpioPortC, 0, gpioModePushPullDrive, 0 );
	GPIO_PinOutSet( gpioPortE, 2 );

	TIMER_Init_TypeDef timerInit = {
		.enable = true,
		.debugRun = true,
		.prescale = timerPrescale1,
		.clkSel = timerClkSelHFPerClk,
		.fallAction = timerInputActionNone,
		.riseAction = timerInputActionNone,
		.mode = timerModeUp,
		.dmaClrAct = false,
		.quadModeX4 = false,
		.oneShot = false,
		.sync = false
	};

	TIMER_IntEnable( TIMER0, TIMER_IF_OF );
	NVIC_EnableIRQ( TIMER0_IRQn );
	uint32_t top = CMU_ClockFreqGet(cmuClock_HFPER) / 11025;
  TIMER_TopSet(TIMER0, top);
	TIMER_Init( TIMER0, &timerInit);
}

int main(void) {
	
	CHIP_Init();


	setupTIMER();
	
	while(1) 
		EMU_EnterEM1();

}

