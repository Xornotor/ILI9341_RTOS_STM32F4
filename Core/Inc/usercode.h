/* Gerenciamento de display de monitoração de base robótica
 * Alunos: Ana Clara Batista, André Paiva, Pedro Augusto Correia
 * UFBA - 2022.1 - Programação em Tempo Real para Sistemas Embarcados
 */


/* ===========================INCLUDES============================ */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "main.h"
#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "usertypedefs.h"
#include "lookuptable.h"


/* ================DEFINES E VARIÁVEIS DE CONTROLE================ */

#define TELA1 1
#define TELA2 2
#define TELA3 3

#define REFRESH_TELA 500

uint16_t sTelaAnterior = TELA1;
uint16_t sTelaAtual = TELA1;


/* =============ESTRUTURAS DE ARMAZENAMENTO DE DADOS============== */

dataset xVelLinearAtual;
dataset xPosicaoAtual;

/* ===========================HANDLERS============================ */

TaskHandle_t xHandlerDisplayManager = NULL;
TaskHandle_t xHandlerIRQScreen1 = NULL;
TaskHandle_t xHandlerIRQScreen2 = NULL;
TaskHandle_t xHandlerIRQScreen3 = NULL;

QueueHandle_t xQueueCorrente = NULL;
QueueHandle_t xQueueVelW = NULL;
QueueHandle_t xQueuePosicao = NULL;

SemaphoreHandle_t xMutexBufferCorrente = NULL;
SemaphoreHandle_t xMutexBufferVelW = NULL;
SemaphoreHandle_t xMutexVelLinearAtual = NULL;
SemaphoreHandle_t xMutexPosicaoAtual = NULL;


/* ==========================PROTÓTIPOS========================== */

void userRTOS(void);
void vDisplayManager(void *p);
void vTaskGeradorCorrente(void *p);
void vTaskGeradorVelW(void *p);
void vTaskGeradorPosicao(void *p);
void vTaskQueueCorrenteReader(void *p);
void vTaskQueueVelWReader(void *p);
void vTaskQueuePosicaoReader(void *p);
void vTaskScreenIRQ1(void *p);
void vTaskScreenIRQ2(void *p);
void vTaskScreenIRQ3(void *p);
void HAL_GPIO_EXTI_Callback(uint16_t);
void inicializar(void);
void funcBaseGraph(void);
void funcBaseTela1(void);
void funcBaseTela2(void);
void funcBaseTela3(void);
void baseTela(uint16_t);
void funcDadosTela1(void);
void funcDadosTela2(void);
void funcDadosTela3(void);
void dadosTela(uint16_t);


/* =====================INICIALIZAÇÃO DO RTOS===================== */

// Função de inicialização
void userRTOS(void){

	xMutexBufferCorrente = xSemaphoreCreateMutex();
	xMutexBufferVelW = xSemaphoreCreateMutex();
	xMutexVelLinearAtual = xSemaphoreCreateMutex();
	xMutexPosicaoAtual = xSemaphoreCreateMutex();

	xQueueCorrente = xQueueCreate(30, sizeof(dataset));
	xQueueVelW = xQueueCreate(30, sizeof(dataset));
	xQueuePosicao = xQueueCreate(3, sizeof(dataset));

	xTaskCreate(vTaskScreenIRQ1,
				"irq1",
				128,
				(void*) 0,
				tskIDLE_PRIORITY,
				&xHandlerIRQScreen1);

	xTaskCreate(vTaskScreenIRQ2,
				"irq2",
				128,
				(void*) 0,
				tskIDLE_PRIORITY,
				&xHandlerIRQScreen2);

	xTaskCreate(vTaskScreenIRQ3,
				"irq3",
				128,
				(void*) 0,
				tskIDLE_PRIORITY,
				&xHandlerIRQScreen3);

	xTaskCreate(vDisplayManager,
				"displayManager",
				1024,
				(void*) 0,
				1,
				&xHandlerDisplayManager);

	xTaskCreate(vTaskGeradorCorrente,
				"geradorCorrente",
				512,
				(void*) 0,
				10,
				NULL);

	xTaskCreate(vTaskGeradorVelW,
				"geradorVelW",
				512,
				(void*) 0,
				9,
				NULL);

	xTaskCreate(vTaskGeradorPosicao,
				"geradorPosicao",
				512,
				(void*) 0,
				8,
				NULL);

	xTaskCreate(vTaskQueueCorrenteReader,
				"queueCorrenteReader",
				512,
				(void*) 0,
				5,
				NULL);

	xTaskCreate(vTaskQueueVelWReader,
				"queueVelWReader",
				512,
				(void*) 0,
				5,
				NULL);

	xTaskCreate(vTaskQueuePosicaoReader,
				"queuePosicaoReader",
				512,
				(void*) 0,
				5,
				NULL);

	vTaskStartScheduler();

    while(1);
}


/* =========================TASKS DO RTOS========================= */

// Gerenciamento da tela
void vDisplayManager(void *p){
	TickType_t xLastWakeTime;;
	while(1){
		xLastWakeTime = xTaskGetTickCount();
		if(sTelaAtual != sTelaAnterior){
			sTelaAnterior = sTelaAtual;
			baseTela(sTelaAtual);
		}
		dadosTela(sTelaAtual);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(REFRESH_TELA));
	}
}

// Geração de dados de corrente e envio para queue
void vTaskGeradorCorrente(void *p) {
	TickType_t xLastWakeTime;
	dataset correntes;
	uint16_t sIndice = 0;
	while(1) {
		xLastWakeTime = xTaskGetTickCount();
		correntes.x = vetorCorrenteX[sIndice];
		correntes.y = vetorCorrenteY[sIndice];
		correntes.z = vetorCorrenteZ[sIndice];
		correntes.timestamp = xLastWakeTime;
		if(sIndice >= LENGTH_LUT){
			sIndice = 0;
		}else{
			sIndice++;
		}
		if(xQueueSendToBack(xQueueCorrente, &correntes, 0) == errQUEUE_FULL){
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
	}
}

// Geração de dados de velocidade angular e envio para queue
void vTaskGeradorVelW(void *p) {
	TickType_t xLastWakeTime;
	dataset velAngular;
	uint16_t sIndice = 0;
	while(1) {
		xLastWakeTime = xTaskGetTickCount();
		velAngular.x = vetorVelAngX[sIndice];
		velAngular.y = vetorVelAngY[sIndice];
		velAngular.z = vetorVelAngZ[sIndice];
		velAngular.timestamp = xLastWakeTime;
		if(sIndice >= LENGTH_LUT){
			sIndice = 0;
		}else{
			sIndice++;
		}
		if(xQueueSendToBack(xQueueVelW, &velAngular, 0) == errQUEUE_FULL){
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}

// Geração de dados de GPS e envio para queue
void vTaskGeradorPosicao(void *p) {
	TickType_t xLastWakeTime;
	dataset posicao;
	uint16_t sIndice = 0;
	while(1) {
		xLastWakeTime = xTaskGetTickCount();
		posicao.x = vetorPosicaoX[sIndice];
		posicao.y = vetorPosicaoY[sIndice];
		posicao.z = vetorPosicaoZ[sIndice];
		posicao.timestamp = xLastWakeTime;
		if(sIndice >= LENGTH_LUT){
			sIndice = 0;
		}else{
			sIndice++;
		}
		if(xQueueSendToBack(xQueuePosicao, &posicao, 0) == errQUEUE_FULL){
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
	}
}

// Leitura de dados de corrente da queue
void vTaskQueueCorrenteReader(void *p) {
	const TickType_t xMaxMutexDelay = pdMS_TO_TICKS(1);
	TickType_t xLastWakeTime;
	dataset corrente;
	while (1) {
		xLastWakeTime = xTaskGetTickCount();
		while(xQueueReceive(xQueueCorrente, &corrente, 0) != errQUEUE_EMPTY){
			if(xSemaphoreTake(xMutexBufferCorrente, xMaxMutexDelay) == pdPASS){

			}
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}

// Leitura de dados de velocidade angular da queue
void vTaskQueueVelWReader(void *p) {
	const TickType_t xMaxMutexDelay = pdMS_TO_TICKS(1);
	const uint16_t L_cm = 20;
	const uint16_t r_cm = 15;
	const float alpha1 = 0;
	const float alpha2 = 2*M_PI/3;
	const float alpha3 = 4*M_PI/3;
	const float sin_alpha1 = sin(alpha1);
	const float sin_alpha2 = sin(alpha2);
	const float sin_alpha3 = sin(alpha3);
	const float cos_alpha1 = cos(alpha1);
	const float cos_alpha2 = cos(alpha2);
	const float cos_alpha3 = cos(alpha3);
	TickType_t xLastWakeTime;
	float linearVX, linearVY, linearW;
	dataset velW, velLinear;
	while (1) {
		xLastWakeTime = xTaskGetTickCount();
		while(xQueueReceive(xQueueVelW, &velW, 0) != errQUEUE_EMPTY){
			if(xSemaphoreTake(xMutexBufferVelW, xMaxMutexDelay) == pdPASS){

			}
		}
		linearVX = r_cm*(2.0/3.0)*(-(sin_alpha1*velW.x)-(sin_alpha2*velW.y)-(sin_alpha3*velW.z));
		linearVY = r_cm*(2.0/3.0)*((cos_alpha1*velW.x)+(cos_alpha2*velW.y)+(cos_alpha3*velW.z));
		linearW = (r_cm*(velW.x+velW.y+velW.z))/(3*L_cm);
		velLinear.x = linearVX;
		velLinear.y = linearVY;
		velLinear.z = linearW;
		if(xSemaphoreTake(xMutexVelLinearAtual, xMaxMutexDelay) == pdPASS){
			xVelLinearAtual = velLinear;
			xSemaphoreGive(xMutexVelLinearAtual);
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}

// Leitura de dados de GPS da queue
void vTaskQueuePosicaoReader(void *p) {
	const TickType_t xMaxMutexDelay = pdMS_TO_TICKS(1);
	TickType_t xLastWakeTime;
	dataset posicao;
	while (1) {
		xLastWakeTime = xTaskGetTickCount();
		while(xQueueReceive(xQueuePosicao, &posicao, 0) != errQUEUE_EMPTY){
			if(xSemaphoreTake(xMutexPosicaoAtual, xMaxMutexDelay) == pdPASS){
				xPosicaoAtual = posicao;
				xSemaphoreGive(xMutexPosicaoAtual);
			}
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
	}
}

// ISR - Botão da Tela 1
void vTaskScreenIRQ1(void *p) {
    while (1) {
        vTaskSuspend(NULL);
        sTelaAtual = TELA1;
    }
}

// ISR - Botão da Tela 2
void vTaskScreenIRQ2(void *p) {
    while (1) {
        vTaskSuspend(NULL);
        sTelaAtual = TELA2;
    }
}

// ISR - Botão da Tela 3
void vTaskScreenIRQ3(void *p) {
    while (1) {
        vTaskSuspend(NULL);
        sTelaAtual = TELA3;
    }
}


/* ==================TRATAMENTO DE INTERRUPÇÕES=================== */

// Callback das interrupções dos botões
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	BaseType_t checkIfYieldRequired;
    switch(GPIO_Pin) {
        case BTN_1_Pin:
            checkIfYieldRequired = xTaskResumeFromISR(xHandlerIRQScreen1);
            portYIELD_FROM_ISR(checkIfYieldRequired);
            break;
        case BTN_2_Pin:
            checkIfYieldRequired = xTaskResumeFromISR(xHandlerIRQScreen2);
            portYIELD_FROM_ISR(checkIfYieldRequired);
            break;
        case BTN_3_Pin:
            checkIfYieldRequired = xTaskResumeFromISR(xHandlerIRQScreen3);
            portYIELD_FROM_ISR(checkIfYieldRequired);
            break;
        default:
            break;
    }
}


/* ======================FUNÇÕES AUXILIARES======================= */

// Inicialização da tela executada antes da inicialização do RTOS
void inicializar(void){
	ILI9341_Init();
	ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
	baseTela(sTelaAtual);
}

// Base para plotagem de gráfico
void funcBaseGraph(void){
	ILI9341_DrawRectangle(35, 70, 4, 120, WHITE);
	ILI9341_DrawRectangle(35, 186, 250, 4, WHITE);
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

// Geração de escala do eixo X
void funcScaleXGraph(uint16_t tempoInicio, uint16_t tempoFim){
	uint16_t swap_value;
	if(tempoInicio > tempoFim){
		swap_value = tempoInicio;
		tempoInicio = tempoFim;
		tempoFim = swap_value;
	}
	float tmp_value = 0;
	char textBuffer[20];

	for(int i = 0; i < 5; i++){
		tmp_value = (tempoInicio+((tempoFim - tempoInicio)*i/4.0));
		sprintf(textBuffer, "%.0f  ", tmp_value);
		ILI9341_DrawText(textBuffer, FONT1, 38+(i*59), 202, WHITE, BLACK);
	}
}

// Base da Tela 1
void funcBaseTela1(void){
	ILI9341_DrawText("Tela 1 - Velocidades e Posicao", FONT4, 25, 11, WHITE, NAVY);
	ILI9341_DrawText("Vel. X (cm/s):", FONT3, 25, 60, LIGHTBLUE, BLACK);
	ILI9341_DrawText("Vel. Y (cm/s)", FONT3, 25, 120, MAGENTA, BLACK);
	ILI9341_DrawText("W (cm/s):", FONT3, 25, 180, YELLOW, BLACK);
	ILI9341_DrawText("Pos. X (cm):", FONT3, 165, 60, GREEN, BLACK);
	ILI9341_DrawText("Pos. Y (cm):", FONT3, 165, 120, DARKORANGE, BLACK);
}

// Base da Tela 2
void funcBaseTela2(void){
	ILI9341_DrawText("Tela 2 - Graficos Vel. Angular", FONT4, 25, 11, WHITE, NAVY);
	ILI9341_DrawText("rad/s", FONT2, 24, 50, WHITE, BLACK);
	ILI9341_DrawText("s", FONT2, 295, 182, WHITE, BLACK);
	ILI9341_DrawText("32", FONT1, 11, 72, WHITE, BLACK);
	ILI9341_DrawText("24", FONT1, 11, 99, WHITE, BLACK);
	ILI9341_DrawText("16", FONT1, 11, 126, WHITE, BLACK);
	ILI9341_DrawText("8", FONT1, 15, 153, WHITE, BLACK);
	ILI9341_DrawText("0", FONT1, 15, 180, WHITE, BLACK);

	funcBaseGraph();

	ILI9341_DrawRectangle(20, 220, 80, 20, CYAN);
	ILI9341_DrawRectangle(120, 220, 80, 20, MAGENTA);
	ILI9341_DrawRectangle(220, 220, 80, 20, YELLOW);

	ILI9341_DrawText("Motor 1", FONT3, 32, 224, BLACK, CYAN);
	ILI9341_DrawText("Motor 2", FONT3, 132, 224, BLACK, MAGENTA);
	ILI9341_DrawText("Motor 3", FONT3, 232, 224, BLACK, YELLOW);

}

// Base da Tela 3
void funcBaseTela3(void){
	ILI9341_DrawText("Tela 3 - Graficos Correntes", FONT4, 38, 11, WHITE, NAVY);
	ILI9341_DrawText("A", FONT2, 34, 50, WHITE, BLACK);
	ILI9341_DrawText("s", FONT2, 295, 182, WHITE, BLACK);
	ILI9341_DrawText("4", FONT1, 15, 72, WHITE, BLACK);
	ILI9341_DrawText("3", FONT1, 15, 99, WHITE, BLACK);
	ILI9341_DrawText("2", FONT1, 15, 126, WHITE, BLACK);
	ILI9341_DrawText("1", FONT1, 15, 153, WHITE, BLACK);
	ILI9341_DrawText("0", FONT1, 15, 180, WHITE, BLACK);

	funcBaseGraph();

	ILI9341_DrawRectangle(20, 220, 80, 20, LIGHTBLUE);
	ILI9341_DrawRectangle(120, 220, 80, 20, DARKORANGE);
	ILI9341_DrawRectangle(220, 220, 80, 20, GREENYELLOW);

	ILI9341_DrawText("Motor 1", FONT3, 32, 224, BLACK, LIGHTBLUE);
	ILI9341_DrawText("Motor 2", FONT3, 132, 224, BLACK, DARKORANGE);
	ILI9341_DrawText("Motor 3", FONT3, 232, 224, BLACK, GREENYELLOW);

}

// Seleção de base de tela
void baseTela(uint16_t sNumTela){
	ILI9341_DrawRectangle(0, 36, 320, 204, BLACK);
	ILI9341_DrawRectangle(0, 0, 320, 36, NAVY);
	switch(sNumTela){
		case TELA1:
			funcBaseTela1();
			break;
		case TELA2:
			funcBaseTela2();
			break;
		case TELA3:
			funcBaseTela3();
			break;
		default:
			break;
	}
}

void funcDadosTela1(void){
	const TickType_t xMaxMutexDelay = pdMS_TO_TICKS(1);
	dataset velLinear;
	dataset posicao;
	char textBuffer[20];

	if(xSemaphoreTake(xMutexVelLinearAtual, xMaxMutexDelay) == pdPASS){
		velLinear = xVelLinearAtual;
		xSemaphoreGive(xMutexVelLinearAtual);
	}
	if(xSemaphoreTake(xMutexPosicaoAtual, xMaxMutexDelay) == pdPASS){
		posicao = xPosicaoAtual;
		xSemaphoreGive(xMutexPosicaoAtual);
	}

	sprintf(textBuffer, "%.1f    ", velLinear.x);
	ILI9341_DrawText(textBuffer, FONT4, 25, 80, LIGHTBLUE, BLACK);
	sprintf(textBuffer, "%.1f    ", velLinear.y);
	ILI9341_DrawText(textBuffer, FONT4, 25, 140, MAGENTA, BLACK);
	sprintf(textBuffer, "%.1f    ", velLinear.z);
	ILI9341_DrawText(textBuffer, FONT4, 25, 200, YELLOW, BLACK);

	sprintf(textBuffer, "%.2f    ", posicao.x);
	ILI9341_DrawText(textBuffer, FONT4, 165, 80, GREEN, BLACK);
	sprintf(textBuffer, "%.2f    ", posicao.y);
	ILI9341_DrawText(textBuffer, FONT4, 165, 140, DARKORANGE, BLACK);
}

void funcDadosTela2(void){
	ILI9341_DrawRectangle(39, 70, 246, 116, BLACK);
	funcScaleXGraph(0, 20);
}

void funcDadosTela3(void){
	ILI9341_DrawRectangle(39, 70, 246, 116, BLACK);
	funcScaleXGraph(500, 1000);
}

// Exibição de dados na tela
void dadosTela(uint16_t sNumTela){
	switch(sNumTela){
		case TELA1:
			funcDadosTela1();
			break;
		case TELA2:
			funcDadosTela2();
			break;
		case TELA3:
			funcDadosTela3();
			break;
		default:
			break;
	}
}
