#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"

void testa(uint32_t letra, uint32_t fundo){
	ILI9341_DrawFilledRectangleCoord(20, 20, 300, 170, fundo);
	ILI9341_DrawText("XORNOTESTE", FONT4, 100, 89, letra, fundo);
	HAL_Delay(1000);
}


void userCode(void){

	ILI9341_Init();
	ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
	ILI9341_FillScreen(BLACK);
	bool escreveu = false;

	while(1){
		if(!escreveu){
			ILI9341_DrawFilledRectangleCoord(22, 192, 99, 219, LIGHTGREY);
			ILI9341_DrawHollowRectangleCoord(20, 190, 100, 220, DARKGREY);
			ILI9341_DrawText("Tela 1", FONT1, 43, 201, BLACK, LIGHTGREY);

			ILI9341_DrawFilledRectangleCoord(122, 192, 199, 219, LIGHTGREY);
			ILI9341_DrawHollowRectangleCoord(120, 190, 200, 220, DARKGREY);
			ILI9341_DrawText("Tela 2", FONT1, 143, 201, BLACK, LIGHTGREY);

			ILI9341_DrawFilledRectangleCoord(222, 192, 299, 219, LIGHTGREY);
			ILI9341_DrawHollowRectangleCoord(220, 190, 300, 220, DARKGREY);
			ILI9341_DrawText("Tela 3", FONT1, 243, 201, BLACK, LIGHTGREY);

			escreveu = true;
		}

		testa(BLACK, WHITE);
		testa(BLACK, LIGHTGREY);
		testa(WHITE, DARKGREY);
		testa(WHITE, NAVY);
		testa(WHITE, DARKGREEN);
		testa(WHITE, DARKCYAN);
		testa(WHITE, MAROON);
		testa(WHITE, PURPLE);
		testa(WHITE, OLIVE);

		testa(WHITE, BLUE);
		testa(BLACK, GREEN);
		testa(BLACK, CYAN);
		testa(WHITE, RED);
		testa(BLACK, MAGENTA);
		testa(BLACK, YELLOW);
		testa(BLACK, ORANGE);
		testa(BLACK, GREENYELLOW);
		testa(BLACK, PINK);


	}

}
