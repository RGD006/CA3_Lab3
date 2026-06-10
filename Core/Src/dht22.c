#include "dht22.h"

static void __DHT22_SetPinMode(GPIO_TypeDef *port, uint16_t pin, uint32_t mode)
{
	if (mode == GPIO_MODE_INPUT) {
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		// Configure as Input with Pull-up (for example)
		GPIO_InitStruct.Pin = pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;

		HAL_GPIO_Init(port, &GPIO_InitStruct);
	} else if (mode == GPIO_MODE_OUTPUT_PP) {
	    GPIO_InitTypeDef GPIO_InitStruct = {0};

	    // Configure as Push-Pull Output
	    GPIO_InitStruct.Pin = pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	    HAL_GPIO_Init(port, &GPIO_InitStruct);
	} else {
		// error handler
	}
}

void DHT22_UsDelay(uint32_t time)
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim2)) < time);
}

void DHT22_Start(void)
{
	__DHT22_SetPinMode(DHT_GPIO_Port, DHT_Pin, GPIO_MODE_OUTPUT_PP);
	HAL_GPIO_WritePin(DHT_GPIO_Port, DHT_Pin, 0);   // pull the pin low
	HAL_Delay(18);   // wait for > 1ms
	__DHT22_SetPinMode(DHT_GPIO_Port, DHT_Pin, GPIO_MODE_INPUT);
}

uint8_t DHT22_Check_Response(void)
{
	uint8_t response = 0;
	__DHT22_SetPinMode(DHT_GPIO_Port, DHT_Pin, GPIO_MODE_INPUT);

	DHT22_UsDelay (40);  // wait for 40us
	// if the pin is low
	if (!(HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin))) {
		DHT22_UsDelay(80);   // wait for 80us

		if ((HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)))
			response = 1;  // if the pin is high, response is ok
		else
			response = -1;
	}

	while ((HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)));   // wait for the pin to go low

	return response;
}

uint8_t DHT22_Read(void)
{
	uint8_t i,j;
	__DHT22_SetPinMode(DHT_GPIO_Port, DHT_Pin, GPIO_MODE_INPUT);

	for (j=0;j<8;j++) {
		while (!(HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)));   // wait for the pin to go high
		DHT22_UsDelay (40);   // wait for 40 us

		if (!(HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)))   // if the pin is low
			i&= ~(1<<(7-j));   // write 0
		else
			i|= (1<<(7-j));  // if the pin is high, write 1

		while ((HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)));  // wait for the pin to go low
	}

	return i;
}
