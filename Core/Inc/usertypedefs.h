#include "main.h"
#include "FreeRTOS.h"

typedef struct {
	float x;
    float y;
    float z;
    TickType_t timestamp;
} dataset;


