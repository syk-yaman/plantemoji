/* Jordan Ebel */

#define _IO_SPA_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef enum
{
	IO_SPA_OK    = 0,
	IO_SPA_ERROR = 1
} IO_SPA_RC;


/******** I2C ********/
typedef struct
{
	uint8_t dev_addr;
	const char *bus;  // Linux only
	int bus_fd;       // Linux only
} IO_SPA_I2C_PARAMS;

IO_SPA_RC io_spa_i2c_init(IO_SPA_I2C_PARAMS *params);
IO_SPA_RC io_spa_i2c_read(IO_SPA_I2C_PARAMS *params, uint8_t addr, uint8_t num_bytes, uint8_t *data);
IO_SPA_RC io_spa_i2c_read_short(IO_SPA_I2C_PARAMS *params, uint8_t num_bytes, uint8_t *data);
IO_SPA_RC io_spa_i2c_write(IO_SPA_I2C_PARAMS *params, uint8_t addr, uint8_t num_bytes, uint8_t *data);
IO_SPA_RC io_spa_i2c_write_short(IO_SPA_I2C_PARAMS *params, uint8_t data);
IO_SPA_RC io_spa_i2c_close(IO_SPA_I2C_PARAMS *params);
/*********************/


