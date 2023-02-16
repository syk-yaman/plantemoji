/* Jordan Ebel */

#include "io_spa.h"
#include "../i2c/i2c.h"

IO_SPA_RC io_spa_i2c_init(IO_SPA_I2C_PARAMS *params)
{
	I2C_Init();                             // Initialize the I2c module.
	I2C_Start();                            // Start I2C communication
	I2C_Stop();                             // Stop I2C communication after initializing 

	return IO_SPA_OK;
}

IO_SPA_RC io_spa_i2c_read(IO_SPA_I2C_PARAMS *params, uint8_t addr, uint8_t num_bytes, uint8_t *data)
{
	if (io_spa_i2c_write_short(params, addr) != IO_SPA_OK)
	{
		return IO_SPA_ERROR;
	}

	I2C_Start();
	uint8_t temp_data = I2C_Read(1);
	data = &temp_data;
	I2C_Stop();   

	return IO_SPA_OK;
}

IO_SPA_RC io_spa_i2c_read_short(IO_SPA_I2C_PARAMS *params, uint8_t num_bytes, uint8_t *data)
{
	I2C_Start();   
	uint8_t temp_data = I2C_Read(1);   
	data = &temp_data;
	I2C_Stop();
	
	return IO_SPA_OK;
}

IO_SPA_RC io_spa_i2c_write(IO_SPA_I2C_PARAMS *params, uint8_t addr, uint8_t num_bytes, uint8_t *data)
{
	uint8_t temp_data = *data;
	I2C_Start();                           
	I2C_Write(addr);
	I2C_Write(temp_data);
	I2C_Stop();    
	
	return IO_SPA_OK;
}

IO_SPA_RC io_spa_i2c_write_short(IO_SPA_I2C_PARAMS *params, uint8_t data)
{
	I2C_Start();                          
	I2C_Write(data);
	I2C_Stop();
	
	return IO_SPA_OK;
}

IO_SPA_RC io_spa_i2c_close(IO_SPA_I2C_PARAMS *params)
{
	I2C_Stop();
	return IO_SPA_OK;
}

