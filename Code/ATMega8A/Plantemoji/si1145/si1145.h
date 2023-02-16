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
