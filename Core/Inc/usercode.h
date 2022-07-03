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
#include "queue.h"
#include "lookuptable.h"

#define TELA1 1
#define TELA2 2
#define TELA3 3

int tela_anterior = TELA1;
int tela_atual = TELA1;

TaskHandle_t displayHandler = NULL;
TaskHandle_t IRQHandlerScreen1 = NULL;
TaskHandle_t IRQHandlerScreen2 = NULL;
TaskHandle_t IRQHandlerScreen3 = NULL;
QueueHandle_t queueCorrentes;
QueueHandle_t queueVelAngular;
QueueHandle_t queuePosicaoGPS;

void vDisplayManager(void *p);
void vTaskScreenIRQ1(void *p);
void vTaskScreenIRQ2(void *p);
void vTaskScreenIRQ3(void *p);
void vTaskGeradorCorrente(void *p);
void vTaskGeradorVelAngular(void *p);
void vTaskGeradorPosicao(void *p);
void vTaskTestDataReader(void *p);
void baseTela(uint16_t);
void dadosTela(uint16_t);

struct Dataset
    {
        float x;
        float y;
        float z;
        TickType_t timestamp;
    } datasetFormat;

// Task Creations e Inicialização do RTOS
void userRTOS(void){

	queueCorrentes = xQueueCreate(300, sizeof(datasetFormat));
	queueVelAngular = xQueueCreate(30, sizeof(datasetFormat));
	queuePosicaoGPS = xQueueCreate(3, sizeof(datasetFormat));

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
				1024,
				(void*) 0,
				1,
				&displayHandler);

    xTaskCreate(vTaskGeradorCorrente,
        		"geradorCorrente",
    			512,
    			(void*) 0,
    			4,
    			NULL);

    xTaskCreate(vTaskGeradorVelAngular,
           		"geradorVelAngular",
       			512,
       			(void*) 0,
       			3,
       			NULL);

    xTaskCreate(vTaskGeradorPosicao,
               	"geradorPosicao",
           		512,
           		(void*) 0,
           		2,
           		NULL);

    xTaskCreate(vTaskTestDataReader,
            	"testDataReader",
        		512,
        		(void*) 0,
        		2,
        		NULL);

    vTaskStartScheduler();

    while(1){

    }
}

// Task de gerenciamento da tela
void vDisplayManager(void *p){
	TickType_t xLastWakeTime;
	while(1){
		xLastWakeTime = xTaskGetTickCount();
		if(tela_atual != tela_anterior){
			tela_anterior = tela_atual;
			baseTela(tela_atual);
		}
		dadosTela(tela_atual);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(250));
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

// Task de test de geração de dados e envio pra queue
void vTaskGeradorCorrente(void *p) {
	TickType_t xLastWakeTime;
	struct Dataset correntes;
	int indice = 0;
	while(1) {
		xLastWakeTime = xTaskGetTickCount();
		correntes.x = vetorCorrenteX[indice];
		correntes.y = vetorCorrenteY[indice];
		correntes.z = vetorCorrenteZ[indice];
		correntes.timestamp = xLastWakeTime;
		if(indice >= LENGTH_LUT){
			indice = 0;
		}else{
			indice++;
		}
		if(xQueueSendToBack(queueCorrentes, &correntes, 0) == errQUEUE_FULL){
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
	}
}

// Task de test de geração de dados e envio pra queue
void vTaskGeradorVelAngular(void *p) {
	TickType_t xLastWakeTime;
	struct Dataset velAngular;
	int indice = 0;
	while(1) {
		xLastWakeTime = xTaskGetTickCount();
		velAngular.x = vetorVelAngX[indice];
		velAngular.y = vetorVelAngY[indice];
		velAngular.z = vetorVelAngZ[indice];
		velAngular.timestamp = xLastWakeTime;
		if(indice >= LENGTH_LUT){
			indice = 0;
		}else{
			indice++;
		}
		if(xQueueSendToBack(queueVelAngular, &velAngular, 0) == errQUEUE_FULL){
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}

// Task de test de geração de dados e envio pra queue
void vTaskGeradorPosicao(void *p) {
	TickType_t xLastWakeTime;
	struct Dataset posicao;
	int indice = 0;
	while(1) {
		xLastWakeTime = xTaskGetTickCount();
		posicao.x = vetorPosicaoX[indice];
		posicao.y = vetorPosicaoY[indice];
		posicao.z = vetorPosicaoZ[indice];
		posicao.timestamp = xLastWakeTime;
		if(indice >= LENGTH_LUT){
			indice = 0;
		}else{
			indice++;
		}
		if(xQueueSendToBack(queuePosicaoGPS, &posicao, 0) == errQUEUE_FULL){
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
	}
}

// Task de test de leitura de dados da queue
void vTaskTestDataReader(void *p) {
	TickType_t xLastWakeTime;
	struct Dataset corrente;
	struct Dataset vel;
	struct Dataset posicao;
	while (1) {
		xLastWakeTime = xTaskGetTickCount();
		while(xQueueReceive(queueCorrentes, &corrente, 0) != errQUEUE_EMPTY);
		while(xQueueReceive(queueVelAngular, &vel, 0) != errQUEUE_EMPTY);
		while(xQueueReceive(queuePosicaoGPS, &posicao, 0) != errQUEUE_EMPTY);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
	}
}

// Função de inicialização da tela executada antes da inicialização do RTOS
void inicializar(void){
	ILI9341_Init();
	ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
	baseTela(tela_atual);
}

// Funções de geração de vídeo

// Base para plotagem de gráfico
void funcGraph(void){
	ILI9341_DrawFilledRectangleCoord(35, 70, 39, 190, WHITE);
	ILI9341_DrawFilledRectangleCoord(35, 186, 285, 190, WHITE);
	ILI9341_DrawVLine(40, 186, 11, WHITE);
	ILI9341_DrawVLine(99, 186, 11, WHITE);
	ILI9341_DrawVLine(158, 186, 11, WHITE);
	ILI9341_DrawVLine(217, 186, 11, WHITE);
	ILI9341_DrawVLine(276, 186, 11, WHITE);
	ILI9341_DrawHLine(28, 77, 11, WHITE);
	ILI9341_DrawHLine(28, 104, 11, WHITE);
	ILI9341_DrawHLine(28, 131, 11, WHITE);
	ILI9341_DrawHLine(28, 158, 11, WHITE);
	ILI9341_DrawHLine(28, 185, 11, WHITE);
}

// Base da Tela 1
void funcTela1(void){
	ILI9341_DrawText("Tela 1 - Velocidades e Posicao", FONT4, 25, 9, WHITE, NAVY);
	ILI9341_DrawText("Vel. X (cm/s):", FONT3, 25, 60, LIGHTBLUE, BLACK);
	ILI9341_DrawText("Vel. Y (cm/s)", FONT3, 25, 120, MAGENTA, BLACK);
	ILI9341_DrawText("W (rad/s):", FONT3, 25, 180, YELLOW, BLACK);
	ILI9341_DrawText("Pos. X (cm):", FONT3, 165, 60, GREEN, BLACK);
	ILI9341_DrawText("Pos. Y (cm):", FONT3, 165, 120, DARKORANGE, BLACK);

	double teste = 12.3456789;
	char testecharcasa1[20], testecharcasa2[20], testecharcasarad[20];
	sprintf(testecharcasa1, "%0.1f   ", teste);
	sprintf(testecharcasarad, "%0.1f   ", teste);
	sprintf(testecharcasa2, "%0.2f   ", teste);
	ILI9341_DrawText(testecharcasa1, FONT4, 25, 80, LIGHTBLUE, BLACK);
	ILI9341_DrawText(testecharcasa1, FONT4, 25, 140, MAGENTA, BLACK);
	ILI9341_DrawText(testecharcasarad, FONT4, 25, 200, YELLOW, BLACK);
	ILI9341_DrawText(testecharcasa2, FONT4, 165, 80, GREEN, BLACK);
	ILI9341_DrawText(testecharcasa2, FONT4, 165, 140, DARKORANGE, BLACK);
}

// Base da Tela 2
void funcTela2(void){
	ILI9341_DrawText("Tela 2 - Graficos Vel. Angular", FONT4, 25, 9, WHITE, NAVY);
	ILI9341_DrawText("rad/s", FONT2, 24, 50, WHITE, BLACK);
	ILI9341_DrawText("s", FONT2, 295, 182, WHITE, BLACK);

	funcGraph();

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
	ILI9341_DrawText("A", FONT2, 34, 50, WHITE, BLACK);
	ILI9341_DrawText("s", FONT2, 295, 182, WHITE, BLACK);

	funcGraph();

	ILI9341_DrawFilledRectangleCoord(20, 220, 100, 240, LIGHTBLUE);
	ILI9341_DrawFilledRectangleCoord(120, 220, 200, 240, DARKORANGE);
	ILI9341_DrawFilledRectangleCoord(220, 220, 300, 240, GREENYELLOW);

	ILI9341_DrawText("Motor 1", FONT3, 32, 224, BLACK, LIGHTBLUE);
	ILI9341_DrawText("Motor 2", FONT3, 132, 224, BLACK, DARKORANGE);
	ILI9341_DrawText("Motor 3", FONT3, 232, 224, BLACK, GREENYELLOW);
}

// Seleção de base de tela
void baseTela(uint16_t numTela){
	ILI9341_DrawFilledRectangleCoord(0, 36, 320, 240, BLACK);
	ILI9341_DrawFilledRectangleCoord(0, 0, 320, 36, NAVY);
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
