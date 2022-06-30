#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"

#define TELA1 1
#define TELA2 2
#define TELA3 3

void moldeTela(void);
void baseTela(uint16_t);

void inicializar(void){
	ILI9341_Init();
	ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
	moldeTela();
}

void userRTOS(void){

	while(1){
		baseTela(TELA1);
		HAL_Delay(1000);
		baseTela(TELA2);
		HAL_Delay(1000);
		baseTela(TELA3);
		HAL_Delay(1000);
	}

}

void moldeTela(void){
	ILI9341_FillScreen(BLACK);
	baseTela(TELA1);
}

void baseTela(uint16_t numTela){
	ILI9341_DrawFilledRectangleCoord(0, 0, 320, 36, NAVY);
	switch(numTela){
		case TELA1:
			ILI9341_DrawText("Tela 1 - Velocidades e Posicao", FONT4, 25, 9, WHITE, NAVY);
			break;
		case TELA2:
			ILI9341_DrawText("Tela 2 - Graficos Vel. Angular", FONT4, 25, 9, WHITE, NAVY);
			break;
		case TELA3:
			ILI9341_DrawText("Tela 3 - Graficos Correntes", FONT4, 38, 9, WHITE, NAVY);
			break;
	}
	ILI9341_DrawFilledRectangleCoord(0, 36, 320, 240, BLACK);
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
