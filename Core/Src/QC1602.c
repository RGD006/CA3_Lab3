#include "QC1602.h"

HAL_StatusTypeDef QC1602_WriteCMD(QC1602_HandleTypeDef *handle ,uint8_t cmd)
{
	if (!handle->display_bus || handle->extender_addr == 0)
		return HAL_ERROR;

    uint8_t data_u = 0, data_l = 0;
    uint8_t data_t[4];

	// High halfword
    data_l = ((cmd << 4) & 0xF0); // Low halfword // I2C
    data_u = (cmd & 0xF0);
    // 0x0C: backlight ON (BL=1), EN=1, RW=0, RS=0 command
    // 0x08: backlight ON (BL=1), EN=0, RW=0, RS=0 command

    data_t[0] = data_u | QC1602_EN_HIGH; // EN=1
    data_t[1] = data_u | QC1602_EN_LOW;  // EN=0 (Write high halfword)
    data_t[2] = data_l | QC1602_EN_HIGH; // EN=1
    data_t[3] = data_l | QC1602_EN_LOW;  // EN=0 (Write low halfword)

    return HAL_I2C_Master_Transmit(handle->display_bus, handle->extender_addr, data_t, 4, 100);
}

HAL_StatusTypeDef QC1602_Init(
		QC1602_HandleTypeDef *handle,
		I2C_HandleTypeDef *bus,
		uint8_t extender_addr,
		uint8_t row_max,
		uint8_t column_max
)
{
	HAL_StatusTypeDef res = HAL_OK;
	handle->display_bus = bus;
	handle->extender_addr = extender_addr;
	handle->row_max = row_max;
	handle->column_max = column_max;

    HAL_Delay(50);

    uint8_t init_data[2];

    // Command 1: 0x30
    init_data[0] = 0x30 | QC1602_EN_HIGH;  // EN=1
    init_data[1] = 0x38 | QC1602_EN_LOW;  // EN=0
    res = HAL_I2C_Master_Transmit(handle->display_bus, handle->extender_addr, init_data, 2, 100);
    HAL_Delay(5);

    // Command 2: 0x30
    res = HAL_I2C_Master_Transmit(handle->display_bus, handle->extender_addr, init_data, 2, 100);
    HAL_Delay(1);

    // Command 3: 0x30
    res = HAL_I2C_Master_Transmit(handle->display_bus, handle->extender_addr, init_data, 2, 100);
    HAL_Delay(1);

    // Command 4: 0x20 (transmit to 4 bit mode)
    init_data[0] = 0x20 | QC1602_EN_HIGH;  // EN=1
    init_data[1] = 0x20 | QC1602_EN_LOW;  // EN=0
    res = HAL_I2C_Master_Transmit(handle->display_bus, handle->extender_addr, init_data, 2, 100);
    HAL_Delay(1);

    res = QC1602_WriteCMD(handle, 0x28);
    HAL_Delay(1);

    // Turn on display. Turn on cursor
    res = QC1602_WriteCMD(handle, 0x0C);
    HAL_Delay(1);

    // Clear screen
    res = QC1602_WriteCMD(handle, 0x01);
    HAL_Delay(2);

    // Enter mode: cursor move right after write
    res = QC1602_WriteCMD(handle, 0x06);
    HAL_Delay(1);

    return res;
}

HAL_StatusTypeDef QC1602_WriteChar(
		QC1602_HandleTypeDef *handle,
		char character,
		uint8_t row,
		uint8_t column
)
{
    if (!handle->display_bus || handle->extender_addr == 0)
    	return HAL_ERROR;

    if (row > handle->row_max || column > handle->column_max)
    	return HAL_ERROR;

    uint8_t shift;

    if (row == 0) {
    	shift = 0x00 + column;
    } else {
    	shift = 0x40 + column;
    }

	QC1602_WriteCMD(handle, QC1602_SHIFT_CMD | shift);

    uint8_t data_u = (character & 0xF0);
    uint8_t data_l = ((character << 4) & 0xF0);
    uint8_t data_t[4];

    uint8_t rs = 0x01;
    uint8_t backlight = 0x08;

    data_t[0] = data_u | QC1602_EN_HIGH | rs | backlight;
    data_t[1] = data_u | QC1602_EN_LOW  | rs | backlight;
    data_t[2] = data_l | QC1602_EN_HIGH | rs | backlight;
    data_t[3] = data_l | QC1602_EN_LOW  | rs | backlight;

    return HAL_I2C_Master_Transmit(handle->display_bus, handle->extender_addr, data_t, 4, 100);
}
