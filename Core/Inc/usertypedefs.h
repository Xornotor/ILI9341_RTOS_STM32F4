#include "main.h"
#include "FreeRTOS.h"


#define TAMANHO_BUFFER 40

typedef struct {
	float x;
    float y;
    float z;
    TickType_t timestamp;
} dataset;

typedef struct {
	dataset dados[TAMANHO_BUFFER];
	int startIndex;
	int posicoesPreenchidas;
} circle_buffer;
