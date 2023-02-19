/* Jordan Ebel */
#include <stdint.h>
#include "io_spa.h"

#ifndef _SI_1145_H
#define _SI_1145_H


#define SI1145_CONFIG_BIT_PS                                     0x1   /* Proximity sensor */
#define SI1145_CONFIG_BIT_ALS            (SI1145_CONFIG_BIT_PS << 1)   /* Ambient light sensor */
#define SI1145_CONFIG_BIT_UV             (SI1145_CONFIG_BIT_PS << 2)   /* UV calculation */
#define SI1145_CONFIG_BIT_MEAS_RATE_FAST (SI1145_CONFIG_BIT_PS << 3)   /* 100ms measurement rate */
#define SI1145_CONFIG_BIT_MEAS_RATE_SLOW (SI1145_CONFIG_BIT_PS << 4)   /* 500ms measurement rate */
#define SI1145_CONFIG_BIT_INDOORS        (SI1145_CONFIG_BIT_PS << 5)   /* Indoors operation */

typedef enum
{
	SI1145_OK          = 0,
	SI1145_FAILURE     = 1,
	SI1145_INVALID_CMD = 2,
	SI1145_OVRFLW_PS1  = 3,
	SI1145_OVRFLW_PS2  = 4,
	SI1145_OVRFLW_PS3  = 5,
	SI1145_OVRFLW_VIS  = 6,
	SI1145_OVRFLW_IR   = 7,
	SI1145_OVRFLW_AUX  = 8
} SI1145_RC;

typedef enum
{
	SI1145_MEASUREMENT_PS     = 1,
	SI1145_MEASUREMENT_ALS    = 2,
	SI1145_MEASUREMENT_PSALS  = 3
} SI1145_MEASUREMENT_QUANTITY;

typedef enum
{
	SI1145_REG_PART_ID    = 0x00,
	SI1145_REG_REV_ID     = 0x01,
	SI1145_REG_SEQ_ID     = 0x02,
	SI1145_REG_HW_KEY     = 0x07,
	SI1145_REG_MEAS_RATE0 = 0x08,
	SI1145_REG_MEAS_RATE1 = 0x09,
	SI1145_REG_UCOEF0     = 0x13,
	SI1145_REG_UCOEF1     = 0x14,
	SI1145_REG_UCOEF2     = 0x15,
	SI1145_REG_UCOEF3     = 0x16,
	SI1145_REG_PARAM_WR   = 0x17,
	SI1145_REG_COMMAND    = 0x18,
	SI1145_REG_RESPONSE   = 0x20,
	SI1145_REG_VIS_DATA0  = 0x22,
	SI1145_REG_VIS_DATA1  = 0x23,
	SI1145_REG_IR_DATA0   = 0x24,
	SI1145_REG_IR_DATA1   = 0x25,
	SI1145_REG_PS1_DATA0  = 0x26,
	SI1145_REG_PS1_DATA1  = 0x27,
	SI1145_REG_PS2_DATA0  = 0x28,
	SI1145_REG_PS2_DATA1  = 0x29,
	SI1145_REG_PS3_DATA0  = 0x2A,
	SI1145_REG_PS3_DATA1  = 0x2B,
	SI1145_REG_UV_DATA0   = 0x2C,
	SI1145_REG_UV_DATA1   = 0x2D,
	SI1145_REG_PARAM_RD   = 0x2E,
} SI1145_REG;

typedef enum
{
	SI1145_RAM_CHLIST           = 0x01,
	SI1145_RAM_ALS_VIS_ADC_MISC = 0x12,
	SI1145_RAM_ALS_IR_ADC_MISC  = 0x1F
} SI1145_RAM;

typedef enum
{
	SI1145_CMD_RESET       = 0x01,
	SI1145_CMD_PS_FORCE    = 0x05,
	SI1145_CMD_ALS_FORCE   = 0x06,
	SI1145_CMD_PSALS_FORCE = 0x07,
	SI1145_CMD_PS_PAUSE    = 0x09,
	SI1145_CMD_ALS_PAUSE   = 0x0A,
	SI1145_CMD_PSALS_PAUSE = 0x0B,
	SI1145_CMD_PS_AUTO     = 0x0D,
	SI1145_CMD_ALS_AUTO    = 0x0E,
	SI1145_CMD_PSALS_AUTO  = 0x0F,
	SI1145_CMD_PARAM_GET   = 0x80,
	SI1145_CMD_PARAM_SET   = 0xA0
} SI1145_CMD;

#define SI1145_CONST_REV_ID          0x00
#define SI1145_CONST_SEQ_ID          0x08
#define SI1145_CONST_HW_KEY          0x17
#define SI1145_CONST_PART_ID         0x45
#define SI1145_CONST_VIS_RANGE       0x20
#define SI1145_CONST_IR_RANGE        0x20
#define SI1145_CONST_UCOEF0          0x7B
#define SI1145_CONST_UCOEF1          0x6B
#define SI1145_CONST_UCOEF2          0x01
#define SI1145_CONST_UCOEF3          0x00
#define SI1145_CONST_MAX_CMD_RETRIES 0x10
#define SI1145_CONST_MEAS_RATE0_SLOW 0x80
#define SI1145_CONST_MEAS_RATE1_SLOW 0x3E /* 0x3E80 -> 500 ms / 0.03125 ms */
#define SI1145_CONST_MEAS_RATE0_FAST 0x80
#define SI1145_CONST_MEAS_RATE1_FAST 0x0C /* 0x0C80 -> 100 ms / 0.03125 ms */


SI1145_RC si1145_init(const char *bus, uint8_t addr, uint8_t config_bitmap);
SI1145_RC si1145_measurement_auto(SI1145_MEASUREMENT_QUANTITY quantity);
SI1145_RC si1145_measurement_pause(SI1145_MEASUREMENT_QUANTITY quantity);
SI1145_RC si1145_measurement_force(SI1145_MEASUREMENT_QUANTITY quantity);
SI1145_RC si1145_get_vis_data(uint16_t *vis_data);
SI1145_RC si1145_get_ir_data(uint16_t *ir_data);
SI1145_RC si1145_get_ps_data(uint16_t *ps1_data, uint16_t *ps2_data, uint16_t *ps3_data);
SI1145_RC si1145_get_uv_data(uint16_t *uv_data);
SI1145_RC si1145_close(void);

#endif /* _SI_1145_H */
