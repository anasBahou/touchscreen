/******************************************************************************
 * Includes
 *****************************************************************************/
#include "DmTft24363Display.h"

using namespace sixtron;

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

/******************************************************************************
 * Global variables
 *****************************************************************************/

/******************************************************************************
 * Global functions
 *****************************************************************************/

/******************************************************************************
 * Main
 *****************************************************************************/
FlashIAP flash;
DmTftIli9341 tft(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);
DmTouch touch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_SCK);
InterruptIn touchItr(DIO4);
I2C i2c(I2C1_SDA, I2C1_SCL);
DmTft24_363_Display myTouchscreen(&flash, &tft, &touch, &touchItr, &i2c);


int main() {

	printf("ON...\n\n");

	myTouchscreen.init();

	myTouchscreen.homePage();

	while(1) {
		myTouchscreen.refresh();
		wait_ms(1000);
	}

	printf("\n...OFF\n\n");



}

