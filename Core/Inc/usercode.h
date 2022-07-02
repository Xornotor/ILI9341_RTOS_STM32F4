/* Gerenciamento de display de monitoração de base robótica
 * Alunos: Ana Clara Batista, André Paiva, Pedro Augusto Correia
 * UFBA - 2022.1 - Programação em Tempo Real para Sistemas Embarcados
 */

#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"
#include "FreeRTOS.h"

#define TELA1 1
#define TELA2 2
#define TELA3 3

int tela_anterior = TELA1;
int tela_atual = TELA1;

TaskHandle_t displayHandler = NULL;
TaskHandle_t IRQHandlerScreen1 = NULL;
TaskHandle_t IRQHandlerScreen2 = NULL;
TaskHandle_t IRQHandlerScreen3 = NULL;

void vDisplayManager(void *p);
void vTaskScreenIRQ1(void *p);
void vTaskScreenIRQ2(void *p);
void vTaskScreenIRQ3(void *p);
void baseTela(uint16_t);
void dadosTela(uint16_t);

// Task Creations e Inicialização do RTOS
void userRTOS(void){

    xTaskCreate(vTaskScreenIRQ1,
    			"irq1",
				128,
				(void*) 0,
				tskIDLE_PRIORITY,
				&IRQHandlerScreen1);

    xTaskCreate(vTaskScreenIRQ2,
    			"irq2",
				128,
				(void*) 0,
				tskIDLE_PRIORITY,
				&IRQHandlerScreen2);

    xTaskCreate(vTaskScreenIRQ3,
    			"irq3",
				128,
				(void*) 0,
				tskIDLE_PRIORITY,
				&IRQHandlerScreen3);

    xTaskCreate(vDisplayManager,
    			"display",
				512,
				(void*) 0,
				1,
				&displayHandler);

    vTaskStartScheduler();

    while(1){

    }
}

// Task de gerenciamento da tela
void vDisplayManager(void *p){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while(1){
		if(tela_atual != tela_anterior){
			tela_anterior = tela_atual;
			baseTela(tela_atual);
		}
		dadosTela(tela_atual);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(333));
	}
}

// ISR - Botão da Tela 1
void vTaskScreenIRQ1(void *p) {
    while (1) {
        vTaskSuspend(NULL);
        tela_atual = TELA1;
    }
}

// ISR - Botão da Tela 2
void vTaskScreenIRQ2(void *p) {
    while (1) {
        vTaskSuspend(NULL);
        tela_atual = TELA2;
    }
}

// ISR - Botão da Tela 3
void vTaskScreenIRQ3(void *p) {
    while (1) {
        vTaskSuspend(NULL);
        tela_atual = TELA3;
    }
}

// Função de inicialização da tela executada antes da inicialização do RTOS
void inicializar(void){
	ILI9341_Init();
	ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
	baseTela(tela_atual);
}

// Funções de geração de vídeo

// Base da Tela 1
void funcTela1(void){
	ILI9341_DrawText("Tela 1 - Velocidades e Posicao", FONT4, 25, 9, WHITE, NAVY);
	ILI9341_DrawText("Veloc. X:", FONT3, 25, 60, CYAN, BLACK);
	ILI9341_DrawText("Veloc. Y:", FONT3, 25, 120, MAGENTA, BLACK);
	ILI9341_DrawText("w:", FONT3, 25, 180, YELLOW, BLACK);
	ILI9341_DrawText("Pos. X:", FONT3, 165, 60, GREEN, BLACK);
	ILI9341_DrawText("Pos. Y:", FONT3, 165, 120, RED, BLACK);

	double teste = 12.3456789;
	char testecharcasa1[10];
	char testecharcasa2[10];
	sprintf(testecharcasa1, "%3.1f", teste);
	sprintf(testecharcasa2, "%4.2f", teste);
	ILI9341_DrawText(testecharcasa1, FONT4, 25, 80, CYAN, BLACK);
	ILI9341_DrawText(testecharcasa1, FONT4, 25, 140, MAGENTA, BLACK);
	ILI9341_DrawText(testecharcasa1, FONT4, 25, 200, YELLOW, BLACK);
	ILI9341_DrawText(testecharcasa2, FONT4, 165, 80, GREEN, BLACK);
	ILI9341_DrawText(testecharcasa2, FONT4, 165, 140, RED, BLACK);
}

// Base da Tela 2
void funcTela2(void){
	ILI9341_DrawText("Tela 2 - Graficos Vel. Angular", FONT4, 25, 9, WHITE, NAVY);
	ILI9341_DrawFilledRectangleCoord(35, 70, 39, 190, WHITE);
	ILI9341_DrawFilledRectangleCoord(35, 186, 285, 190, WHITE);
	ILI9341_DrawVLine(97, 186, 11, WHITE);
	ILI9341_DrawVLine(157, 186, 11, WHITE);
	ILI9341_DrawVLine(217, 186, 11, WHITE);
	ILI9341_DrawVLine(277, 186, 11, WHITE);
	ILI9341_DrawText("rad/s", FONT2, 24, 50, WHITE, BLACK);
	ILI9341_DrawText("s", FONT2, 295, 182, WHITE, BLACK);
	ILI9341_DrawFilledRectangleCoord(20, 220, 100, 240, CYAN);
	ILI9341_DrawFilledRectangleCoord(120, 220, 200, 240, MAGENTA);
	ILI9341_DrawFilledRectangleCoord(220, 220, 300, 240, YELLOW);
	ILI9341_DrawText("Motor 1", FONT3, 32, 224, BLACK, CYAN);
	ILI9341_DrawText("Motor 2", FONT3, 132, 224, BLACK, MAGENTA);
	ILI9341_DrawText("Motor 3", FONT3, 232, 224, BLACK, YELLOW);
}

// Base da Tela 3
void funcTela3(void){
	ILI9341_DrawText("Tela 3 - Graficos Correntes", FONT4, 38, 9, WHITE, NAVY);
	ILI9341_DrawFilledRectangleCoord(35, 70, 39, 190, WHITE);
	ILI9341_DrawFilledRectangleCoord(35, 186, 285, 190, WHITE);
	ILI9341_DrawVLine(97, 186, 11, WHITE);
	ILI9341_DrawVLine(157, 186, 11, WHITE);
	ILI9341_DrawVLine(217, 186, 11, WHITE);
	ILI9341_DrawVLine(277, 186, 11, WHITE);
	ILI9341_DrawText("A", FONT2, 33, 50, WHITE, BLACK);
	ILI9341_DrawText("s", FONT2, 295, 182, WHITE, BLACK);
	ILI9341_DrawFilledRectangleCoord(20, 220, 100, 240, LIGHTBLUE);
	ILI9341_DrawFilledRectangleCoord(120, 220, 200, 240, DARKORANGE);
	ILI9341_DrawFilledRectangleCoord(220, 220, 300, 240, GREENYELLOW);
	ILI9341_DrawText("Motor 1", FONT3, 32, 224, BLACK, LIGHTBLUE);
	ILI9341_DrawText("Motor 2", FONT3, 132, 224, BLACK, DARKORANGE);
	ILI9341_DrawText("Motor 3", FONT3, 232, 224, BLACK, GREENYELLOW);
}

// Seleção de base de tela
void baseTela(uint16_t numTela){
	ILI9341_DrawFilledRectangleCoord(0, 0, 320, 36, NAVY);
	ILI9341_DrawFilledRectangleCoord(0, 36, 320, 240, BLACK);
	switch(numTela){
		case TELA1:
			funcTela1();
			break;
		case TELA2:
			funcTela2();
			break;
		case TELA3:
			funcTela3();
			break;
		default:
			break;
	}
}

void dadosTela(uint16_t numTela){
	switch(numTela){
		case TELA1:
			break;
		case TELA2:
			break;
		case TELA3:
			break;
		default:
			break;
	}
}

// Callback das interrupções dos botões
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	BaseType_t checkIfYieldRequired;
    switch(GPIO_Pin) {
        case BTN_1_Pin:
            checkIfYieldRequired = xTaskResumeFromISR(IRQHandlerScreen1);
            portYIELD_FROM_ISR(checkIfYieldRequired);
            break;
        case BTN_2_Pin:
            checkIfYieldRequired = xTaskResumeFromISR(IRQHandlerScreen2);
            portYIELD_FROM_ISR(checkIfYieldRequired);
            break;
        case BTN_3_Pin:
            checkIfYieldRequired = xTaskResumeFromISR(IRQHandlerScreen3);
            portYIELD_FROM_ISR(checkIfYieldRequired);
            break;
        default:
            break;
    }
}
