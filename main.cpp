/******************************************************************************
 * Includes
 *****************************************************************************/
#include "mbed.h"
#include "DmTftIli9341.h"
#include "DmTouch.h"
#include "Button.h"
#include "PinNames.h"
#include "max17201.h"
#include "DmTft24363Display.h"

#include <stdio.h>
#include <math.h>


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

int main() {

	printf("ON...\n\n");

	FlashIAP* flash = new FlashIAP();
	DmTftIli9341* tft = new DmTftIli9341(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);
	DmTouch* touch = new DmTouch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_SCK);
	InterruptIn* touchItr = new InterruptIn(DIO4);
	I2C* i2c = new I2C(I2C1_SDA, I2C1_SCL);
	DmTft24_363_Display* myTouchscreen = new DmTft24_363_Display(flash, tft, touch, touchItr, i2c);


	while(1) {
		myTouchscreen->refresh();
		wait_ms(1000);
	}

	printf("\n...OFF\n\n");



}

