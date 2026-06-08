#include "QC1602.h"

static uint8_t __QC1602_CheckHandleTypeDef(QC1602_HandleTypeDef *handle)
{
	if (!handle->display_bus || handle->extender_addr == 0)
    	return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef QC1602_WriteCMD(QC1602_HandleTypeDef *handle ,uint8_t cmd)
{
	if (__QC1602_CheckHandleTypeDef(handle) != HAL_OK)
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

    HAL_Delay(50); // >40 ms power-on delay
    res = QC1602_WriteCMD(handle, 0x30);
    HAL_Delay(5);

    res = QC1602_WriteCMD(handle, 0x30);
    HAL_Delay(2);

    res = QC1602_WriteCMD(handle, 0x30);
    HAL_Delay(10);

    res = QC1602_WriteCMD(handle, 0x20); // switch to 4-bit mode
    HAL_Delay(2);

    res = QC1602_WriteCMD(handle, 0x28); // 4-bit, 2-line, 5×8 font
    HAL_Delay(2);

    res = QC1602_WriteCMD(handle, 0x08); // display OFF
    HAL_Delay(2);

    res = QC1602_WriteCMD(handle, 0x01); // clear display
    HAL_Delay(3);

    res = QC1602_WriteCMD(handle, 0x06); // entry mode: cursor increment, no shift
    HAL_Delay(2);

    res = QC1602_WriteCMD(handle, 0x0C); // display ON, cursor OFF, blink OFF
    HAL_Delay(5);

    return res;
}

HAL_StatusTypeDef QC1602_WriteChar(
		QC1602_HandleTypeDef *handle,
		char character,
		uint8_t row,
		uint8_t column
)
{
	if (__QC1602_CheckHandleTypeDef(handle) != HAL_OK
			|| row > handle->row_max
			|| column > handle->column_max)
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

HAL_StatusTypeDef QC1602_WriteString(
		QC1602_HandleTypeDef *handle,
		const char *string,
		const size_t string_len,
		uint8_t start_row,
		uint8_t start_column
)
{
	if (__QC1602_CheckHandleTypeDef(handle) != HAL_OK
			|| start_row > handle->row_max
			|| start_column > handle->column_max)
		return HAL_ERROR;

	HAL_StatusTypeDef res = HAL_OK;
	size_t row = start_row;

	for (size_t i = 0; i < string_len; i++) {
		if (string[i] == '\n')
			row++;
		else
			res = QC1602_WriteChar(handle, string[i], row, start_column + i);

		HAL_Delay(2);
	}

	return res;
}

HAL_StatusTypeDef QC1602_ClearDisplay(QC1602_HandleTypeDef *handle)
{
	if (__QC1602_CheckHandleTypeDef(handle) != HAL_OK)
		return HAL_ERROR;

    HAL_StatusTypeDef res = QC1602_WriteCMD(handle, 0x01);
    HAL_Delay(5);
    return res;
}
