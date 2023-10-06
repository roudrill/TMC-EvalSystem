#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include "hal/HAL.h"
#include "hal/USB.h"

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

//--> extern uint8_t USB_DCI_DeInit(void);
//--> extern uint8_t USB_Class_CDC_DeInit(uint8_t controller_ID);
//--> extern uint8_t USB_Class_DeInit(uint8_t controller_ID);

static void init();
static void deInit();
static void tx(uint8_t ch);
static uint8_t rx(uint8_t *ch);
static void txN(uint8_t *str, uint8_t number);
static uint8_t rxN(uint8_t *ch, uint8_t number);
static void clearBuffers(void);
static uint32_t bytesAvailable();

typedef struct read_status {
	uint8_t status;
};

RXTXTypeDef USB =
{
	.init            = init,
	.deInit          = deInit,
	.rx              = rx,
	.tx              = tx,
	.rxN             = rxN,
	.txN             = txN,
	.clearBuffers    = clearBuffers,
	.baudRate        = 115200,
	.bytesAvailable  = bytesAvailable
};

#define MSG_MAX_SIZE 32

K_MSGQ_DEFINE(uart_msg, 1, MSG_MAX_SIZE, 4); 

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

static char rx_buf[MSG_MAX_SIZE];

/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	// printk("serial_cb\n");

    if (!uart_irq_update(uart_dev)) 
	{
        return;
    }

    if (!uart_irq_rx_ready(uart_dev)) 
	{
        return;
    }

    while (uart_fifo_read(uart_dev, &c, 1) == 1) 
	{
        int r = k_msgq_put(&uart_msg, &c, K_NO_WAIT);
		// printk("Return: %d Caracter: %c Número de caracters: %d\n", r, c, k_msgq_num_used_get(&uart_msg));
    }
}

void init()
{
    char tx_buf[MSG_MAX_SIZE];

    if (!device_is_ready(uart_dev)) 
	{
        //--> printk("UART device not found!");
        return;
    }   

    /* configure interrupt and callback to receive data */
    int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

    if (ret < 0) {
        if (ret == -ENOTSUP) {
            //--> printk("Interrupt-driven UART API support not enabled\n");
        } else if (ret == -ENOSYS) {
            //--> printk("UART device does not support interrupt-driven API\n");
        } else {
            //--> printk("Error setting UART callback: %d\n", ret);
        }
        return;
    }   

    uart_irq_rx_enable(uart_dev);
}

uint8_t rxN(uint8_t *str, uint8_t number)
{
	uint8_t c;

	if (k_msgq_num_used_get(&uart_msg) < number)
	{
		// printk("number of caracters %d\n", k_msgq_num_used_get(&uart_msg));
		return false;
	}

	printk("number of caracters %d\n", k_msgq_num_used_get(&uart_msg));

	for(int32_t i = 0; i < number; i++) 
	{
	 	if(k_msgq_get(&uart_msg, &c, K_FOREVER) == 0) 
		{
			str[i] = c;
		}
    }

	k_msgq_purge(&uart_msg);

	return true;
}

void tx(uint8_t ch)
{
	uart_poll_out(uart_dev, ch);
}

void txN(uint8_t *str, uint8_t number)
{
	for(int32_t i = 0; i < number; i++)
	{
		tx(str[i]);
	}
}

static void clearBuffers(void)
{
	k_msgq_purge(&uart_msg);
}

static uint32_t bytesAvailable()
{
	return k_msgq_num_used_get(&uart_msg);
}

static void deInit(void)
{
	k_msgq_purge(&uart_msg);
}
