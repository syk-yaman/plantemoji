/* Jordan Ebel */

#include "si1145.h"
#include <util/delay.h>


#define SI1145_ISSET(map, bit) ((map & bit) != 0)

static IO_SPA_I2C_PARAMS params;

static SI1145_RC si1145_check_reg(SI1145_REG reg, uint8_t expected);
static SI1145_RC si1145_read_reg(SI1145_REG reg, uint8_t *data);
static SI1145_RC si1145_write_reg(SI1145_REG reg, uint8_t data);
static SI1145_RC si1145_write_check_reg(SI1145_REG reg, uint8_t data);
static SI1145_RC si1145_check_ram(SI1145_RAM ram, uint8_t expected);
static SI1145_RC si1145_read_ram(SI1145_RAM ram, uint8_t *data);
static SI1145_RC si1145_write_ram(SI1145_RAM ram, uint8_t data);
static SI1145_RC si1145_check_status(void);
static SI1145_RC si1145_send_cmd(SI1145_CMD cmd, uint8_t cmd_low_bits);


static SI1145_RC si1145_check_reg(SI1145_REG reg, uint8_t expected)
{
	uint8_t data;

	if (si1145_read_reg(reg, &data) != IO_SPA_OK)
	{
		return SI1145_FAILURE;
	}

	return data == expected ? SI1145_OK : SI1145_FAILURE;
}

static SI1145_RC si1145_read_reg(SI1145_REG reg, uint8_t *data)
{
	return io_spa_i2c_read(&params, reg, 1, data);
}

static SI1145_RC si1145_write_reg(SI1145_REG reg, uint8_t data)
{
	uint8_t read_data;

	if (io_spa_i2c_write(&params, reg, 1, &data) != IO_SPA_OK)
	{
		return SI1145_FAILURE;
	}

	return SI1145_OK;
}

static SI1145_RC si1145_write_check_reg(SI1145_REG reg, uint8_t data)
{
	if (si1145_write_reg(reg, data) != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	return si1145_check_reg(reg, data);
}

static SI1145_RC si1145_check_ram(SI1145_RAM ram, uint8_t expected)
{
	uint8_t data;

	if (si1145_read_ram(ram, &data) != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	return data == expected ? SI1145_OK : SI1145_FAILURE;
}

static SI1145_RC si1145_read_ram(SI1145_RAM ram, uint8_t *data)
{
	SI1145_RC rc = SI1145_OK;

	if (si1145_send_cmd(SI1145_CMD_PARAM_GET, ram) != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	if ((rc = si1145_check_status()) != SI1145_OK)
	{
		return rc;
	}

	if (si1145_read_reg(SI1145_REG_PARAM_RD, data) != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	return SI1145_OK;
}

static SI1145_RC si1145_write_ram(SI1145_RAM ram, uint8_t data)
{
	SI1145_RC rc = SI1145_OK;

	if (si1145_write_check_reg(SI1145_REG_PARAM_WR, data) != SI1145_OK ||
	si1145_send_cmd(SI1145_CMD_PARAM_SET, ram) != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	if ((rc = si1145_check_status()) != SI1145_OK)
	{
		return rc;
	}

	return si1145_check_ram(ram, data);
}

static SI1145_RC si1145_check_status(void)
{
	uint8_t data;
	if (si1145_read_reg(SI1145_REG_RESPONSE, &data) != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	if ((data & 0xF0) == 0x00)
	{
		return SI1145_OK;
	}

	switch (data)
	{
		case 0x80: return SI1145_INVALID_CMD;
		case 0x88: return SI1145_OVRFLW_PS1;
		case 0x89: return SI1145_OVRFLW_PS2;
		case 0x8A: return SI1145_OVRFLW_PS3;
		case 0x8C: return SI1145_OVRFLW_VIS;
		case 0x8D: return SI1145_OVRFLW_IR;
		case 0x8E: return SI1145_OVRFLW_AUX;
		default:   return SI1145_FAILURE;
	}
}

static SI1145_RC si1145_send_cmd(SI1145_CMD cmd, uint8_t cmd_low_bits)
{
	uint8_t command_reg_val = (cmd | cmd_low_bits);
	uint8_t data = 0x0;
	uint8_t command_retries = 0;

	/* Reset command register, then set command register. */
	if (si1145_write_check_reg(SI1145_REG_COMMAND, 0x00) != SI1145_OK ||
	si1145_write_reg(SI1145_REG_COMMAND, command_reg_val) != SI1145_OK )
	{
		//printf("Command error code: 0x%x\n", si1145_check_status());
		return SI1145_FAILURE;
	}

	/* Wait for non-zero response register contents. */
	if (cmd != SI1145_CMD_RESET)
	{
		do
		{
			if (si1145_read_reg(SI1145_REG_RESPONSE, &data) != SI1145_OK)
			{
				return SI1145_FAILURE;
			}
			if (++command_retries >= SI1145_CONST_MAX_CMD_RETRIES)
			{
				//printf("Timed out completing command: 0x%x\n", cmd);
				return SI1145_FAILURE;
			}
		}
		while (data == 0x0);
	}

	return SI1145_OK;
}

SI1145_RC si1145_init(const char *bus, uint8_t addr, uint8_t config_bitmap)
{
	uint8_t chlist = 0x0;
	params.dev_addr = addr;
	#ifdef PLATFORM_LINUX
	params.bus = bus;
	#endif

	if (io_spa_i2c_init(&params))
	{
		return SI1145_FAILURE;
	}

	/* Reset device */
	if (si1145_send_cmd(SI1145_CMD_RESET, 0x0) != SI1145_OK)
	{
		//printf("Failed to initialize SI1145 (%s)\n", "Reset");
		return SI1145_FAILURE;
	}

	/* TODO: add sleep to platform spa library */
	_delay_ms(1000);

	/* Check device */
	if (si1145_check_reg(SI1145_REG_PART_ID, SI1145_CONST_PART_ID) != SI1145_OK ||
	si1145_check_reg(SI1145_REG_REV_ID, SI1145_CONST_REV_ID) != SI1145_OK ||
	si1145_check_reg(SI1145_REG_SEQ_ID, SI1145_CONST_SEQ_ID) != SI1145_OK)
	{
		//printf("Failed to initialize SI1145 (%s)\n", "Read Only checks");
		return SI1145_FAILURE;
	}

	/* HW key */
	if (si1145_write_check_reg(SI1145_REG_HW_KEY, SI1145_CONST_HW_KEY) != SI1145_OK)
	{
		//printf("Failed to initialize SI1145 (%s)\n", "HW Key");
		return SI1145_FAILURE;
	}

	/* Proximity sensor */
	if (SI1145_ISSET(config_bitmap, SI1145_CONFIG_BIT_PS))
	{
		chlist |= 0x07;
	}

	/* Ambient light sensor */
	if (SI1145_ISSET(config_bitmap, SI1145_CONFIG_BIT_ALS))
	{
		chlist |= 0x30;
	}

	/* UV calculation */
	if (SI1145_ISSET(config_bitmap, SI1145_CONFIG_BIT_UV))
	{
		chlist |= 0x80;

		/* Outdoors operation */
		if (!SI1145_ISSET(config_bitmap, SI1145_CONFIG_BIT_INDOORS))
		{
			/* Set VIS_RANGE */
			if (si1145_write_ram(SI1145_RAM_ALS_VIS_ADC_MISC, SI1145_CONST_VIS_RANGE) != SI1145_OK)
			{
				//printf("Failed to initialize SI1145 (%s)\n", "ALS VIS Range");
				return SI1145_FAILURE;
			}

			/* Set IR_RANGE */
			if (si1145_write_ram(SI1145_RAM_ALS_IR_ADC_MISC, SI1145_CONST_IR_RANGE) != SI1145_OK)
			{
				//printf("Failed to initialize SI1145 (%s)\n", "ALS IR Range");
				return SI1145_FAILURE;
			}
		}

		/* Default UV calibration coefficients */
		if (si1145_write_check_reg(SI1145_REG_UCOEF0, SI1145_CONST_UCOEF0) != SI1145_OK ||
		si1145_write_check_reg(SI1145_REG_UCOEF1, SI1145_CONST_UCOEF1) != SI1145_OK ||
		si1145_write_check_reg(SI1145_REG_UCOEF2, SI1145_CONST_UCOEF2) != SI1145_OK ||
		si1145_write_check_reg(SI1145_REG_UCOEF3, SI1145_CONST_UCOEF3) != SI1145_OK)
		{
			//printf("Failed to initialize SI1145 (%s)\n", "UCOEF");
			return SI1145_FAILURE;
		}
	}

	/* Slow measurement rate */
	if (SI1145_ISSET(config_bitmap, SI1145_CONFIG_BIT_MEAS_RATE_SLOW))
	{
		if (si1145_write_check_reg(SI1145_REG_MEAS_RATE0, SI1145_CONST_MEAS_RATE0_SLOW) != SI1145_OK ||
		si1145_write_check_reg(SI1145_REG_MEAS_RATE1, SI1145_CONST_MEAS_RATE1_SLOW) != SI1145_OK)
		{
			//printf("Failed to initialize SI1145 (%s)\n", "MEAS_RATE slow");
			return SI1145_FAILURE;
		}
	}

	/* Fast measurement rate */
	if (SI1145_ISSET(config_bitmap, SI1145_CONFIG_BIT_MEAS_RATE_FAST))
	{
		if (si1145_write_check_reg(SI1145_REG_MEAS_RATE0, SI1145_CONST_MEAS_RATE0_FAST) != SI1145_OK ||
		si1145_write_check_reg(SI1145_REG_MEAS_RATE1, SI1145_CONST_MEAS_RATE1_FAST) != SI1145_OK)
		{
			//printf("Failed to initialize SI1145 (%s)\n", "MEAS_RATE fast");
			return SI1145_FAILURE;
		}
	}

	/* CHLIST */
	if (si1145_write_ram(SI1145_RAM_CHLIST, chlist) != SI1145_OK)
	{
		//printf("Failed to initialize SI1145 (%s)\n", "CHLIST");
		return SI1145_FAILURE;
	}

	return SI1145_OK;
}

SI1145_RC si1145_measurement_auto(SI1145_MEASUREMENT_QUANTITY quantity)
{
	SI1145_CMD cmd;
	switch (quantity)
	{
		case SI1145_MEASUREMENT_PS:
		cmd = SI1145_CMD_PS_AUTO;
		break;
		case SI1145_MEASUREMENT_ALS:
		cmd = SI1145_CMD_ALS_AUTO;
		break;
		case SI1145_MEASUREMENT_PSALS:
		cmd = SI1145_CMD_PSALS_AUTO;
		break;
		default:
		return SI1145_FAILURE;
	};

	if (si1145_send_cmd(cmd, 0x0) != SI1145_OK ||
	si1145_check_status() != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	return SI1145_OK;
}

SI1145_RC si1145_measurement_pause(SI1145_MEASUREMENT_QUANTITY quantity)
{
	SI1145_CMD cmd;
	switch (quantity)
	{
		case SI1145_MEASUREMENT_PS:
		cmd = SI1145_CMD_PS_PAUSE;
		break;
		case SI1145_MEASUREMENT_ALS:
		cmd = SI1145_CMD_ALS_PAUSE;
		break;
		case SI1145_MEASUREMENT_PSALS:
		cmd = SI1145_CMD_PSALS_PAUSE;
		break;
		default:
		return SI1145_FAILURE;
	};

	if (si1145_send_cmd(cmd, 0x0) != SI1145_OK ||
	si1145_check_status() != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	return SI1145_OK;
}

SI1145_RC si1145_measurement_force(SI1145_MEASUREMENT_QUANTITY quantity)
{
	SI1145_CMD cmd;
	switch (quantity)
	{
		case SI1145_MEASUREMENT_PS:
		cmd = SI1145_CMD_PS_FORCE;
		break;
		case SI1145_MEASUREMENT_ALS:
		cmd = SI1145_CMD_ALS_FORCE;
		break;
		case SI1145_MEASUREMENT_PSALS:
		cmd = SI1145_CMD_PSALS_FORCE;
		break;
		default:
		return SI1145_FAILURE;
	};

	if (si1145_send_cmd(cmd, 0x0) != SI1145_OK ||
	si1145_check_status() != SI1145_OK)
	{
		return SI1145_FAILURE;
	}

	return SI1145_OK;
}

SI1145_RC si1145_get_vis_data(uint16_t *vis_data)
{
	uint8_t data[2];
	if (si1145_read_reg(SI1145_REG_VIS_DATA0, &data[0]) ||
	si1145_read_reg(SI1145_REG_VIS_DATA1, &data[1]))
	{
		return SI1145_FAILURE;
	}

	*vis_data = ((uint16_t)data[1] << 8) | data[0];
	return SI1145_OK;
}

SI1145_RC si1145_get_ir_data(uint16_t *ir_data)
{
	uint8_t data[2];
	if (si1145_read_reg(SI1145_REG_IR_DATA0, &data[0]) ||
	si1145_read_reg(SI1145_REG_IR_DATA1, &data[1]))
	{
		return SI1145_FAILURE;
	}

	*ir_data = ((uint16_t)data[1] << 8) | data[0];
	return SI1145_OK;
}

SI1145_RC si1145_get_ps_data(uint16_t *ps1_data, uint16_t *ps2_data, uint16_t *ps3_data)
{
	uint8_t data[6];
	if (si1145_read_reg(SI1145_REG_PS1_DATA0, &data[0]) ||
	si1145_read_reg(SI1145_REG_PS1_DATA1, &data[1]) ||
	si1145_read_reg(SI1145_REG_PS2_DATA0, &data[2]) ||
	si1145_read_reg(SI1145_REG_PS2_DATA1, &data[3]) ||
	si1145_read_reg(SI1145_REG_PS3_DATA0, &data[4]) ||
	si1145_read_reg(SI1145_REG_PS3_DATA1, &data[5]))
	{
		return SI1145_FAILURE;
	}

	*ps1_data = ((uint16_t)data[1] << 8) | data[0];
	*ps2_data = ((uint16_t)data[3] << 8) | data[2];
	*ps3_data = ((uint16_t)data[5] << 8) | data[4];
	return SI1145_OK;
}

SI1145_RC si1145_get_uv_data(uint16_t *uv_data)
{
	uint8_t data[2];
	if (si1145_read_reg(SI1145_REG_UV_DATA0, &data[0]) ||
	si1145_read_reg(SI1145_REG_UV_DATA1, &data[1]))
	{
		return SI1145_FAILURE;
	}

	*uv_data = ((uint16_t)data[1] << 8) | data[0];
	return SI1145_OK;
}

SI1145_RC si1145_close()
{
	io_spa_i2c_close(&params);
	return SI1145_OK;
}
