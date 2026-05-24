#ifndef QC1602_H
#define QC1602_H

#include "main.h"

#define QC1602_EN_HIGH   (0x0C)
#define QC1602_EN_LOW    (0x08)
#define QC1602_SHIFT_CMD (0x80)

typedef struct QC1602_Handle {
	I2C_HandleTypeDef *display_bus;
	uint8_t extender_addr;
	uint8_t row_max;
	uint8_t column_max;
} QC1602_HandleTypeDef;

HAL_StatusTypeDef QC1602_WriteCMD(
		QC1602_HandleTypeDef *handle,
		uint8_t cmd
);

HAL_StatusTypeDef QC1602_WriteChar(
		QC1602_HandleTypeDef *handle,
		char character,
		uint8_t row,
		uint8_t column
);

HAL_StatusTypeDef QC1602_Init(
		QC1602_HandleTypeDef *handle,
		I2C_HandleTypeDef *bus,
		uint8_t extender_addr,
		uint8_t row_max,
		uint8_t column_max
);

HAL_StatusTypeDef QC1602_WriteString(
		QC1602_HandleTypeDef *handle,
		const char *string,
		const size_t string_len,
		uint8_t start_row,
		uint8_t start_column
);

HAL_StatusTypeDef QC1602_ClearDisplay(QC1602_HandleTypeDef *handle);

#endif
