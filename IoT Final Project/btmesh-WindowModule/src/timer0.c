/*
 * timer0.c
 *
 *  Created on: 26-Sep-2017
 *      Author: Gunj Manseta
 */
//#include "em_timer.h"
#include "em_timer.h"
#include "timer0.h"
#include "cmu.h"

volatile bool TIMER_INT_SERVED = false;

void TIMER0_init()
{
	cmu_init_TIMER0();

	/* Select TIMER0 parameters */
	const TIMER_Init_TypeDef timerInit =
	{
	.enable     = false,				/* Disable timer when init complete. Use Enable function instead */
	.debugRun   = true,					/* Stop counter during debug halt. */
	.prescale   = timerPrescale64,		/* No prescaling required. So, prescaler = 1 */
	.clkSel     = timerClkSelHFPerClk,	/* Using HFPERCLK */
	.fallAction = timerInputActionNone,	/* No action */
	.riseAction = timerInputActionNone,	/* No action */
	.mode       = timerModeDown,		/* Up-counting */
	.dmaClrAct  = false,				/* Do not clear DMA requests when DMA channel is active. */
	.quadModeX4 = false,				/* Select X2 quadrature decode mode (if used). ----> check its use*/
	.oneShot    = true,					/* Enable one shot. */
	.sync       = false,				/* Not started/stopped/reloaded by other timers. */
	};

	/* Configure TIMER */
	TIMER_Init(TIMER0,&timerInit);
}

void TIMER0_setup()
{
	TIMER0_init();

	/* Enable overflow interrupt */
	TIMER_IntEnable(TIMER0, TIMER_IF_UF);

	TIMER_IntClear(TIMER0, 0xFFFF);
}

void TIMER0_startwithCount(uint32_t timerCount)
{
//	blockSleepMode(ENERGY_MODE_TIMER);

	/* Set TIMER Top value */
	TIMER_CounterSet(TIMER0, timerCount);

	NVIC_ClearPendingIRQ(TIMER0_IRQn);

	/* Enable TIMER0 interrupt vector in NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);

	TIMER_Enable(TIMER0,true);
}

void TIMER0_IRQHandler()
{
	__disable_irq();

	//as this is only a one shot timer, we can disable the NVIC level interrupts
	NVIC_DisableIRQ(TIMER0_IRQn);
	/* Clear flag for TIMER0 overflow interrupt */
	if(TIMER0->IF & TIMER_IF_UF)
	{
		TIMER_IntClear(TIMER0, TIMER_IF_UF);
		TIMER_INT_SERVED = true;
	}

	__enable_irq();

}
