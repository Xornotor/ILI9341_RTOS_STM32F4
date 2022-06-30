#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"

void moldeTela(void);

void userCode(void){

	ILI9341_Init();
	ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
	ILI9341_FillScreen(BLACK);
	moldeTela();

	while(1){

	}

}

void moldeTela(void){
	ILI9341_DrawFilledRectangleCoord(22, 192, 99, 219, LIGHTGREY);
	ILI9341_DrawHollowRectangleCoord(20, 190, 100, 220, DARKGREY);
	ILI9341_DrawText("Tela 1", FONT1, 43, 201, BLACK, LIGHTGREY);

	ILI9341_DrawFilledRectangleCoord(122, 192, 199, 219, LIGHTGREY);
	ILI9341_DrawHollowRectangleCoord(120, 190, 200, 220, DARKGREY);
	ILI9341_DrawText("Tela 2", FONT1, 143, 201, BLACK, LIGHTGREY);

	ILI9341_DrawFilledRectangleCoord(222, 192, 299, 219, LIGHTGREY);
	ILI9341_DrawHollowRectangleCoord(220, 190, 300, 220, DARKGREY);
	ILI9341_DrawText("Tela 3", FONT1, 243, 201, BLACK, LIGHTGREY);

	ILI9341_DrawFilledRectangleCoord(20, 20, 300, 170,  WHITE);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
		case BTN_1_Pin:
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
			break;
		case BTN_2_Pin:
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
			break;
		case BTN_3_Pin:
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			break;
		default:
			break;
	}
}
