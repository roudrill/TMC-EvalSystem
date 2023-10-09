#include "hal/HAL.h"
#include "hal/SysTick.h"

#include <zephyr/kernel.h>

volatile uint32_t systick = 0;

void timer_handler(struct k_timer *dummy)
{
     systick++;
}

K_TIMER_DEFINE(timer, timer_handler, NULL);

void systick_init()
{
	k_timer_start(&timer, K_MSEC(1), K_MSEC(1));
}

uint32_t systick_getTick()
{
	return systick;
}

uint32_t systick_getMicrosecondTick() // It wasn't implemeted becouse only use by TMC6140_eval.c
{
	return 1;
}

void wait(uint32_t delay)	// wait for [delay] ms/systicks
{
	uint32_t startTick = systick;
	while((systick-startTick) <= delay) {}
}

uint32_t timeSince(uint32_t tick)	// time difference since the [tick] timestamp in ms/systicks
{
	return timeDiff(systick, tick);
}

uint32_t timeDiff(uint32_t newTick, uint32_t oldTick) // Time difference between newTick and oldTick timestamps
{
	uint32_t tickDiff = newTick - oldTick;

	// Prevent subtraction underflow - saturate to 0 instead
	if(tickDiff != 0)
		return tickDiff - 1;
	else
		return 0;
}
