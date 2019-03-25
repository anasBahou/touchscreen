#include "mbed.h"
#include "DmTftIli9341.h"
#include "DmTouch.h"
#include "PinNames.h"
#include "Button.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/
#define MARGIN  5
#define RESULT_MARGIN_X  (MARGIN*3)
#define RESULT_MARGIN_Y  (MARGIN*3)
#define BUTTONS_PER_LINE   1
#define BUTTONS_NUM_LINES  1
#define NUM_BUTTONS  (sizeof(captions)/sizeof(captions[0]))

const char* captions[] = {
      "Touch"
};

Button* buttons[NUM_BUTTONS];

/******************************************************************************
 * Local variables
 *****************************************************************************/

/*********  TFT DISPLAY INIT *********/
DmTftIli9341 tft(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);

 /*********   TOUCH PANEL INIT  *********/
DmTouch touch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_MISO, SPI1_SCK);

/******************************************************************************
 * Global variables
 *****************************************************************************/

/******************************************************************************
 * Local functions
 *****************************************************************************/
void handleClick(uint32_t arg)
{
	uint16_t xc = 50;
	uint16_t yc = 50;
	uint16_t rc = 10;
	tft.drawCircle(xc, yc, rc, BLACK);
	tft.fillCircle(xc, yc, rc, RED);
}

/******************************************************************************
 * Main
 *****************************************************************************/

int main() {

	// Initialisation
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t w = tft.width();
	uint16_t h = tft.height();
	uint16_t size = 50;//(w - (BUTTONS_PER_LINE + 1)*MARGIN)/BUTTONS_PER_LINE;
	bool down = false;

	tft.init();
	tft.clearScreen(BLACK);
	tft.fillRectangle(RESULT_MARGIN_X, RESULT_MARGIN_Y, w-RESULT_MARGIN_X, h-RESULT_MARGIN_Y, WHITE);
	touch.init();

	// Fonction

	x = 100; //MARGIN + (size + MARGIN) * (0 % BUTTONS_PER_LINE);
	y = 100; // h + (size + MARGIN) * (0 / BUTTONS_PER_LINE);
	buttons[0] = new Button(captions[0], x, y, size, size);
	buttons[0]->setAction(handleClick, captions[0][0]);
    buttons[0]->draw(&tft);

	while(true) {
	    touch.readTouchData(x, y, down);
	    if (buttons[0]->handle(x, y, down)) {
	        buttons[0]->draw(&tft);
	    }
	    wait(0.02);
	}
}




