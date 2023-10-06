#ifndef TMC4671_EVAL_H_
#define TMC4671_EVAL_H_

#include <stdint.h>

typedef enum {
	DRIVER_DISABLE,
	DRIVER_ENABLE,
	DRIVER_USE_GLOBAL_ENABLE
} DriverState;


static void timer_overflow(void);
static uint32_t rotate(uint8_t motor, int32_t velocity);
static uint32_t right(uint8_t motor, int32_t velocity);
static uint32_t left(uint8_t motor, int32_t velocity);
static uint32_t stop(uint8_t motor);
static uint32_t moveTo(uint8_t motor, int32_t position);
static uint32_t moveBy(uint8_t motor, int32_t *ticks);
static uint32_t handleParameter(uint8_t readWrite, uint8_t motor, uint8_t type, int32_t *value);
static uint32_t getMeasuredSpeed(uint8_t motor, int32_t *value);
static void periodicJob(uint32_t actualSystick);
static void timer_overflow(void);
static void writeRegister(uint8_t motor, uint8_t address, int32_t value);
static void readRegister(uint8_t motor, uint8_t address, int32_t *value);
static uint32_t SAP(uint8_t type, uint8_t motor, int32_t value);
static uint32_t GAP(uint8_t type, uint8_t motor, int32_t *value);
static uint32_t userFunction(uint8_t type, uint8_t motor, int32_t *value);
static void enableDriver(DriverState state);
static void deInit(void);
static uint8_t reset();
static uint8_t restore();
static void checkErrors(uint32_t tick);

#endif // TMC4671_EVAL_H_