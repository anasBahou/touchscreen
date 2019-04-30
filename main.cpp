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

	printf("start main\n\n");
	DmTft24_363_Display* myTouchscreen;
	myTouchscreen = new DmTft24_363_Display();

	myTouchscreen->init();

	myTouchscreen->homePage();

	while(1) {
		myTouchscreen->refresh();
		wait_ms(1000);
	}

	printf("end main\n\n");



}

