#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include "hal/HAL.h"
#include "hal/IOs.h"

static void init();
static void setPinConfiguration(IOPinTypeDef *pin);
static void copyPinConfiguration(IOPinInitTypeDef *from, IOPinTypeDef*to);
static void resetPinConfiguration(IOPinTypeDef *pin);
static void setPin2Output(IOPinTypeDef *pin);
static void setPin2Input(IOPinTypeDef *pin);
static void setPinHigh(IOPinTypeDef *pin);
static void setPinLow(IOPinTypeDef *pin);
static void setPinState(IOPinTypeDef *pin, IO_States state);
static IO_States getPinState(IOPinTypeDef *pin);
static uint8_t isPinHigh(IOPinTypeDef *pin);

#define DE1_ALIAS DT_ALIAS(de1)

static const struct gpio_dt_spec gpio = GPIO_DT_SPEC_GET_OR(DE1_ALIAS, gpios, {0});

IOsTypeDef IOs =
{
	.init        = init,
	.set         = setPinConfiguration,
	.reset       = resetPinConfiguration,
	.copy        = copyPinConfiguration,
	.toOutput    = setPin2Output,
	.toInput     = setPin2Input,
	.setHigh     = setPinHigh,
	.setLow      = setPinLow,
	.setToState  = setPinState,
	.getState    = getPinState,
	.isHigh      = isPinHigh
};

static void init()
{
	int ret;

	if (!gpio_is_ready_dt(&gpio)) {
        return 0;
    }
}

static void setPinConfiguration(IOPinTypeDef *pin)
{

	if(IS_DUMMY_PIN(pin))
		return;

	uint32_t config = 0;
	switch(pin->configuration.GPIO_Mode)
	{
	case GPIO_Mode_IN:
	    gpio_pin_configure_dt(&gpio, GPIO_INPUT);
		break;
	case GPIO_Mode_OUT:
	    gpio_pin_configure_dt(&gpio, GPIO_OUTPUT);
		break;
	case GPIO_Mode_AN:   	
		break;
	}

	switch(pin->configuration.GPIO_OType)
	{
	case GPIO_OType_PP:
	    gpio_pin_configure_dt(&gpio, GPIO_OPEN_SOURCE);
		break;
	case GPIO_OType_OD:
	    gpio_pin_configure_dt(&gpio, GPIO_OPEN_DRAIN);
		break;
	}

	switch(pin->configuration.GPIO_PuPd)
	{
	case GPIO_PuPd_NOPULL:
		break;
	case GPIO_PuPd_UP:
		gpio_pin_configure_dt(&gpio, GPIO_PULL_UP);
		break;
	case GPIO_PuPd_DOWN:
		gpio_pin_configure_dt(&gpio, GPIO_PULL_DOWN);
		break;
	}
}

static void setPin2Output(IOPinTypeDef *pin)
{
	if(IS_DUMMY_PIN(pin))
		return;

	pin->configuration.GPIO_Mode = GPIO_Mode_OUT;
	setPinConfiguration(pin);
}

static void setPin2Input(IOPinTypeDef *pin)
{
	if(IS_DUMMY_PIN(pin))
		return;

	pin->configuration.GPIO_Mode = GPIO_Mode_IN;
	setPinConfiguration(pin);
}

static void setPinState(IOPinTypeDef *pin, IO_States state)
{
	if(IS_DUMMY_PIN(pin))
		return;

	switch(state)
	{
	case IOS_LOW:
		pin->configuration.GPIO_Mode   = GPIO_Mode_OUT;
		pin->configuration.GPIO_PuPd   = GPIO_PuPd_NOPULL;
		pin->configuration.GPIO_OType  = GPIO_OType_PP;
		setPinConfiguration(pin);
		gpio_pin_set_dt(&gpio, 0);
		break;
	case IOS_HIGH:
		pin->configuration.GPIO_Mode   = GPIO_Mode_OUT;
		pin->configuration.GPIO_PuPd   = GPIO_PuPd_NOPULL;
		pin->configuration.GPIO_OType  = GPIO_OType_PP;
		setPinConfiguration(pin);
		gpio_pin_set_dt(&gpio, 1);
		break;
	case IOS_OPEN:
		pin->configuration.GPIO_Mode  = GPIO_Mode_AN;
		setPinConfiguration(pin);
		break;
	case IOS_NOCHANGE:
		break;
	}

	pin->state = state;

	setPinConfiguration(pin);
}

static IO_States getPinState(IOPinTypeDef *pin)
{
	if(IS_DUMMY_PIN(pin))
		return IOS_OPEN;

	if(pin->configuration.GPIO_Mode == GPIO_Mode_AN)
		pin->state = IOS_OPEN;
	else if(gpio_pin_get_dt(&gpio))
		pin->state = IOS_HIGH;
	else
		pin->state = IOS_LOW;

	return pin->state;
}

static void setPinHigh(IOPinTypeDef *pin)
{
	if(IS_DUMMY_PIN(pin))
		return;

	pin->state = IOS_HIGH;
	return gpio_pin_set_dt(&gpio, 0);
}

static void setPinLow(IOPinTypeDef *pin)
{
	if(IS_DUMMY_PIN(pin))
		return;

	pin->state = IOS_LOW;
	return gpio_pin_set_dt(&gpio, 1);

}

static uint8_t isPinHigh(IOPinTypeDef *pin) // Die Abfrage eines Pins funktioniert nur, wenn der Pin AF1 ist
{
	if(IS_DUMMY_PIN(pin))
		return -1;

	if (pin->state)
		return 1;
	else
		return 0;
}

static void copyPinConfiguration(IOPinInitTypeDef *from, IOPinTypeDef *to)
{
	if(IS_DUMMY_PIN(to))
		return;

	to->configuration.GPIO_Mode   = from->GPIO_Mode;
	to->configuration.GPIO_OType  = from->GPIO_OType;
	to->configuration.GPIO_PuPd   = from->GPIO_PuPd;
	setPinConfiguration(to);
}

static void resetPinConfiguration(IOPinTypeDef *pin)
{

	if(IS_DUMMY_PIN(pin))
		return;

	copyPinConfiguration(&(pin->resetConfiguration), pin);
}

