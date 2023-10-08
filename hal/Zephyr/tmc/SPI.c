#include "hal/HAL.h"
#include "hal/RS232.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>

void init();
void reset_ch1();
void reset_ch2();

typedef uint32_t SPI_MemMapPtr;

static uint8_t readWrite(SPIChannelTypeDef *SPIChannel, uint8_t data, uint8_t lastTransfer);
static uint8_t spi_ch1_readWrite(uint8_t data, uint8_t lastTransfer);
static uint8_t spi_ch2_readWrite(uint8_t data, uint8_t lastTransfer);
static void spi_ch1_readWriteArray(uint8_t *data, size_t length);
static void spi_ch2_readWriteArray(uint8_t *data, size_t length);

SPIChannelTypeDef *SPIChannel_1_default;
SPIChannelTypeDef *SPIChannel_2_default;

static IOPinTypeDef IODummy = { .bitWeight = DUMMY_BITWEIGHT };

static const struct device *spi1 = DEVICE_DT_GET(DT_NODELABEL(spi1));
//--> static const struct device *spi2 = DEVICE_DT_GET(DT_NODELABEL(spi2));

static struct spi_config spi1_cfg = {
	.operation = SPI_WORD_SET(8),
    .frequency = 256000U
};

static struct spi_config spi2_cfg = {
	.operation = SPI_WORD_SET(8),
    .frequency = 256000U
};

SPITypeDef SPI=
{
	.ch1 =
	{
		.periphery       = 1,
		.CSN             = &IODummy,
		.readWrite       = spi_ch1_readWrite,
		.readWriteArray  = spi_ch1_readWriteArray,
		.reset           = reset_ch1
	},

	.ch2 =
	{
		.periphery       = 2,
		.CSN             = &IODummy,
		.readWrite       = spi_ch2_readWrite,
		.readWriteArray  = spi_ch2_readWriteArray,
		.reset           = reset_ch2
	},

	.init = init
};


void init()
{
	if (!device_is_ready(spi1)) {
        //--> printk("SPI device %s is not ready\n", spi->name);
        return;
    }

	//--> if (!device_is_ready(spi2)) {
        //--> printk("SPI device %s is not ready\n", spi->name);
        //--> return;
    //--> }

	// SPI0 -> EEPROM
	// -------------------------------------------------------------------------------

	// SPI1 -> ch1
	// -------------------------------------------------------------------------------

	// SPI2 -> ch2
	// -------------------------------------------------------------------------------
}

void reset_ch1()
{

}

void reset_ch2()
{

}

// Helper lookup tables
static uint8_t PBR_values[4] = { 2, 3, 5, 7 };
static uint16_t BR_values[16] = { 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

uint32_t spi_getFrequency(SPIChannelTypeDef *SPIChannel)
{
	return spi1_cfg.frequency;
}

// Set the SPI frequency to the next-best available frequency (rounding down).
// Returns the actual frequency set or 0 if no suitable frequency was found.
uint32_t spi_setFrequency(SPIChannelTypeDef *SPIChannel, uint32_t desiredFrequency)
{
	spi1_cfg.frequency = desiredFrequency;

	return spi1_cfg.frequency;
}

int32_t spi_readInt(SPIChannelTypeDef *SPIChannel, uint8_t address)
{
	// clear write bit
	address &= 0x7F;

	SPIChannel->readWrite(address, false);
	int value = SPIChannel->readWrite(0, false);
	value <<= 8;
	value |= SPIChannel->readWrite(0, false);
	value <<= 8;
	value |= SPIChannel->readWrite(0, false);
	value <<= 8;
	value |= SPIChannel->readWrite(0, true);

	return value;
}

int32_t spi_ch1_readInt(uint8_t address)
{
	return spi_readInt(SPIChannel_1_default, address);
}

int32_t spi_ch2_readInt(uint8_t address)
{
	return spi_readInt(SPIChannel_2_default, address);
}

void spi_writeInt(SPIChannelTypeDef *SPIChannel, uint8_t address, int32_t value)
{
	SPIChannel->readWrite(address|0x80, false);
	SPIChannel->readWrite(0xFF & (value>>24), false);
	SPIChannel->readWrite(0xFF & (value>>16), false);
	SPIChannel->readWrite(0xFF & (value>>8), false);
	SPIChannel->readWrite(0xFF & (value>>0), true);
}

void spi_ch1_writeInt(uint8_t address, int32_t value)
{
	spi_writeInt(SPIChannel_1_default, address, value);
}

void spi_ch2_writeInt(uint8_t address, int32_t value)
{
	spi_writeInt(SPIChannel_2_default, address, value);
}

uint8_t spi_ch1_readWrite(uint8_t data, uint8_t lastTransfer)
{
	return readWrite(&SPI.ch1, data, lastTransfer);
}

uint8_t spi_ch2_readWrite(uint8_t data, uint8_t lastTransfer)
{
	return readWrite(&SPI.ch2, data, lastTransfer);
}

static void spi_ch1_readWriteArray(uint8_t *data, size_t length)
{
	for(size_t i = 0; i < length; i++)
	{
		data[i] = readWrite(&SPI.ch1, data[i], (i == (length - 1))? true:false);
	}
}

static void spi_ch2_readWriteArray(uint8_t *data, size_t length)
{
	for(size_t i = 0; i < length; i++)
	{
		data[i] = readWrite(&SPI.ch2, data[i], (i == (length - 1))? true:false);
	}
}

uint8_t spi_ch1_readWriteByte(uint8_t data, uint8_t lastTransfer)
{
	return readWrite(SPIChannel_1_default, data, lastTransfer);
}

uint8_t spi_ch2_readWriteByte(SPIChannelTypeDef *SPIChannel, uint8_t data, uint8_t lastTransfer)
{
	return readWrite(SPIChannel, data, lastTransfer);
}

uint8_t readWrite(SPIChannelTypeDef *SPIChannel, uint8_t writeData, uint8_t lastTransfer)
{	
	uint8_t readData;

    struct spi_buf writeBuf[] = {
		{
        	.buf = &writeData,
			.len = 1
		}
	};

	struct spi_buf readBuf[] = {
		{
        	.buf = &readData,
			.len = 1
		}
	};
    
	struct spi_buf_set tx = {
          .buffers = writeBuf
    };

	struct spi_buf_set rx = {
          .buffers = readBuf
    };

	spi_transceive(spi1, &spi1_cfg, &tx, &rx);

	return *(uint8_t *)(rx.buffers->buf);
}
