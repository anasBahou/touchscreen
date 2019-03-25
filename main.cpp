/**********************************************************************************************
 Copyright (c) 2014 DisplayModule. All rights reserved.

 Redistribution and use of this source code, part of this source code or any compiled binary
 based on this source code is permitted as long as the above copyright notice and following
 disclaimer is retained.

 DISCLAIMER:
 THIS SOFTWARE IS SUPPLIED "AS IS" WITHOUT ANY WARRANTIES AND SUPPORT. DISPLAYMODULE ASSUMES
 NO RESPONSIBILITY OR LIABILITY FOR THE USE OF THE SOFTWARE.
 ********************************************************************************************/

/******************************************************************************
 * Tested on NUCLEO-F401RE, LPCXpresso11U68, LPCXpresso824-MAX platform.
 *****************************************************************************/

#include "mbed.h"
#include "DmTftIli9341.h"
#include "DmTouch.h"
#include "PinNames.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

/* Note that there are restrictions on which platforms that can use printf
   in combinations with the DmTftLibrary. Some platforms (e.g. LPC1549 LPCXpresso)
   use the same pins for USBRX/USBTX and display control. Printing will
   cause the display to not work. Read more about this on the display's notebook
   page. */


/******************************************************************************
 * Local variables
 *****************************************************************************/

/*********  TFT DISPLAY INIT *********/
DmTftIli9341 tft(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);  /* DmTftIli9341(PinName cs, PinName dc, PinName mosi, PinName miso, PinName clk)  DM_TFT28_105 and DM_TFT28_116*/


 /*********   TOUCH PANEL INIT  *********/
DmTouch touch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_MISO, SPI1_SCK);


//DigitalInOut csTouch(DIO3, PIN_OUTPUT, PullUp, 1);
//DigitalInOut csDisplay(SPI1_CS, PIN_OUTPUT, PullUp, 1);

/******************************************************************************
 * Global variables
 *****************************************************************************/

/******************************************************************************
 * Local functions
 *****************************************************************************/


/******************************************************************************
 * Main
 *****************************************************************************/

int main() {

  tft.init();

  uint16_t x0 = 157;
  uint16_t y0 = 218;
  uint16_t a0 = 10;
  tft.drawCircle(x0, y0, a0, BLUE);
  tft.fillCircle(x0, y0, a0, BLUE);

  uint16_t x1 = 100;
  uint16_t y1 = 124;
  uint16_t a1 = 50;
  tft.drawCircle(x1, y1, a1, RED);
  tft.fillCircle(x1, y1, a1, RED);

  uint16_t x2 = 25;
  uint16_t y2 = 226;
  uint16_t a2= 35;
  tft.drawCircle(x2, y2, a2, GREEN);
  tft.fillCircle(x2, y2, a2, GREEN);

}

