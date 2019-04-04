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
 * Includes
 *****************************************************************************/

#include "mbed.h"
#include <stdio.h>
#include <math.h>
#include "DmTftHX8353C.h"
#include "DmTftS6D0164.h"
#include "DmTftIli9325.h"
#include "DmTftIli9341.h"
#include "DmTftSsd2119.h"
#include "DmTouch.h"

#include "Button.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/


#define MARGIN  5

#define RESULT_MARGIN_X  (MARGIN*3)
#define RESULT_MARGIN_Y  (MARGIN*3)

#define NUM_BUTTONS  (sizeof(menu_Items)/sizeof(menu_Items[0]))

#define NUM_SETTINGS_VARIABLES  (sizeof(menu_Items)/sizeof(menu_Items[0]))

#if 1
  /* Displays without adapter */
  #define DM_PIN_SPI_MOSI   D11
  #define DM_PIN_SPI_MISO   D12
  #define DM_PIN_SPI_SCLK   D13
  #define DM_PIN_CS_TOUCH   D4
  #define DM_PIN_CS_TFT     D10
  #define DM_PIN_CS_SDCARD  D8
  #define DM_PIN_CS_FLASH   D6
#else
  /* Displays with adapter */
  #define DM_PIN_SPI_MOSI   A0
  #define DM_PIN_SPI_MISO   D9
  #define DM_PIN_SPI_SCLK   A1
  #define DM_PIN_CS_TOUCH   D8
  #define DM_PIN_CS_TFT     A3
  #define DM_PIN_CS_SDCARD  D10
#endif

//#define LANDSCAPE

/******************************************************************************
 * Local variables
 *****************************************************************************/
FlashIAP my_flash;

//DmTftIli9325 tft;  /* DM_TFT28_103 and DM_TFT24_104 */
DmTftIli9341 tft(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);  /* DmTftIli9341(PinName cs, PinName dc, PinName mosi, PinName miso, PinName clk)  DM_TFT28_105 and DM_TFT28_116*/
 //DmTftSsd2119 tft;   /* DM_TFT35_107 */

//DmTouch touch(DmTouch::DM_TFT28_103, DmTouch::Software); /* For LPC4088 QuickStart Board */
//DmTouch touch(DmTouch::DM_TFT28_103);
//DmTouch touch(DmTouch::DM_TFT24_104, DmTouch::Software); /* For LPC4088 QuickStart Board */
//DmTouch touch(DmTouch::DM_TFT24_104);
DmTouch touch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_MISO, SPI1_SCK);
//DmTouch touch(DmTouch::DM_TFT35_107);

DigitalInOut csTouch(DM_PIN_CS_TOUCH, PIN_OUTPUT, PullUp, 1);
DigitalInOut csDisplay(DM_PIN_CS_TFT, PIN_OUTPUT, PullUp, 1);
DigitalInOut csSDCard(DM_PIN_CS_SDCARD, PIN_OUTPUT, PullUp, 1);
#ifdef DM_PIN_CS_FLASH
  DigitalInOut csFlash(DM_PIN_CS_FLASH, PIN_OUTPUT, PullUp, 1);
#endif

#ifdef LANDSCAPE
#define BUTTONS_PER_LINE   2
  #define BUTTONS_NUM_LINES  3
  /*
   *   1   2   3   4   5   +
   *   6   7   8   9   0   -
   *   *   /   =   clr
   */
  const char* menu_Items[] = {
            "BATTERY STATUS","SOUND DIRECTION",
            "SETTINGS"
  };
#else
  #define BUTTONS_PER_LINE   1
  #define BUTTONS_NUM_LINES  3

  const char* menu_Items[] = {
          "BATTERY STATUS","SOUND DIRECTION",
          "SETTINGS"
  };
#endif

Button* buttons[NUM_BUTTONS];

static char buff[25] = {0};
static bool redrawResult = true;
static bool clearResult = true;

// Address
uint32_t settings_address = 0x080FF000;

uint32_t back_from_settings = 0;
uint16_t settings_variables[5];

/******************************************************************************
 * Global variables
 *****************************************************************************/


/******************************************************************************
 * Local functions
 *****************************************************************************/

void read_variables(){

	  my_flash.read(settings_variables , settings_address, my_flash.get_page_size());
//	  my_flash.read(settings_variables+1 , mic_sens_adress, 1);

	  printf("speed : %d\n", settings_variables[0]);
	  printf("mic_sens : %d\n", settings_variables[1]);
//	  printf("mic_sens : %d\n", settings_variables[1]);

}

void save_variables(){


	  my_flash.erase(settings_address, my_flash.get_sector_size(settings_address));
	  my_flash.program(settings_variables , settings_address, my_flash.get_page_size());


//      uint32_t ret1 = my_flash.get_page_size();
//      uint32_t ret2 = my_flash.get_sector_size(settings_address);
//	  printf("program : %d\n", ret);
//	  printf("get_page_size : %d\n", ret1);
//	  printf("get_sector_size : %d\n", ret2);
//	  my_flash.program(settings_variables+1 , mic_sens_adress, 1);

	  printf("Settings saved \n");
	  printf("Saved speed : %d\n", settings_variables[0]);
	  printf("Saved mic_sens : %d\n", settings_variables[1]);


}



void home_page(uint32_t arg=0){
  bool down = false;
  uint16_t x = 0;
  uint16_t y = 0;
  tft.init();
  touch.init();

  // case back from the settings window ==> need to save the settings

	if (arg == back_from_settings){
	  save_variables();
	}

  for (int i = 0; i < NUM_BUTTONS; i++) {
	buttons[i]->draw(&tft);
  }

  while(true) {
	touch.readTouchData(x, y, down);


	for (int i = 0; i < NUM_BUTTONS; i++) {
	  if (buttons[i]->handle(x, y, down)) {
		buttons[i]->draw(&tft);
	  }
	}

	//wait(0.02);

  }

}


void handleClick_Battery(uint32_t arg=0)
{
	// parameteres for the readTouchData function
  bool down = false;
  uint16_t x = 0;
  uint16_t y = 0;

  uint16_t x1 = 0;
  uint16_t y1 = 0;
  uint16_t size1 = 50;

  uint16_t x0_rect = 60;
  uint16_t y0_rect = 80;
  uint16_t x1_rect = tft.width() - x0_rect;
  uint16_t y1_rect = tft.height() - y0_rect;
  uint16_t rect_height = y1_rect - y0_rect;

  float battery_level = 70; // to be able to do the division by 100
  uint16_t level_to_pixel = (uint16_t)(((100-battery_level)/100)*rect_height); // conversion to an integer
  char battery_level_str[6];
  const char percent[] = "%";
  sprintf(battery_level_str, "%.1f %s", battery_level, percent); // to convert battery_level to a string of a format "70.0 %"

  // create a new button
  const char* back_menu_Items[] = {
		  "back"};
  Button* back_button[1];
  back_button[0]= new Button(back_menu_Items[0], x1, y1, size1, size1);
  back_button[0]->setAction(home_page, back_menu_Items[0][0]); // back to the home page


  tft.init();
  touch.init();

  // drawing the Battery level
  tft.drawString(x0_rect+40, y0_rect-20, battery_level_str);
  tft.drawRectangle(x0_rect, y0_rect, x1_rect, y1_rect, BLUE);
  tft.fillRectangle(x0_rect+1, y0_rect+1 + level_to_pixel, x1_rect-1, y1_rect-1, RED); // +1 and -1 to avoid the rectangle borders

  for (int i = 0; i < NUM_BUTTONS; i++) {
	  back_button[i]->draw(&tft);
  }

  while(true) {
	touch.readTouchData(x, y, down);
	for (int i = 0; i < 1; i++) {
	  if (back_button[i]->handle(x, y, down)) {
		  back_button[i]->draw(&tft);
	  }
	}

	//wait(0.02);
  }

}


void handleClick_Sound(uint32_t arg=0)
{
  bool down = false;
  uint16_t x = 0;
  uint16_t y = 0;

  uint16_t x1 = 0;
  uint16_t y1 = 0;
  uint16_t size1 = 50;

  uint16_t w = tft.width();
  uint16_t h = tft.height();



  const char* back_menu_Items[] = {
		  "back"};
  Button* back_button[1];
  // create the back button
  back_button[0]= new Button(back_menu_Items[0], x1, y1, size1, size1);
  back_button[0]->setAction(home_page, back_menu_Items[0][0]); // back to the home page


  tft.init();
  touch.init();

  // get the sound direction
  int sound_angle = 160;
  float sound_angle_rad = (float)sound_angle * 3.14/180;
  uint16_t circle_radius = 80;
  uint16_t x_sound, y_sound;

  x_sound = w/2 + circle_radius*cos(sound_angle_rad);
  y_sound = h/2 - circle_radius*sin(sound_angle_rad);

  char sound_angle_str[4];
  const char degre[] = "degree";
  sprintf(sound_angle_str, "%d %s", sound_angle, degre);

  // what to show

  tft.drawCircle(w/2, h/2, circle_radius, GREEN);
  tft.drawPoint(w/2, h/2);
  tft.drawString(w/2 - 40, h/2 - circle_radius - 20, sound_angle_str);
  tft.drawLine(w/2, h/2, x_sound, y_sound, BLUE);

  for (int i = 0; i < NUM_BUTTONS; i++) {
	  back_button[i]->draw(&tft);
  }

  while(true) {
	touch.readTouchData(x, y, down);
	for (int i = 0; i < 1; i++) {
	  if (back_button[i]->handle(x, y, down)) {
		  back_button[i]->draw(&tft);
	  }
	}

	//wait(0.02);
  }

}



void change_setting(uint32_t arg){

	uint16_t x1 = tft.width()/2 ;
    uint16_t y1 = 50 + MARGIN;
    uint16_t size1 = 50;

    char speed_str[4];
    char mic_sens_str[4];

    // Handling the speed variation
	if (arg==0){ // case "-" pressed
		tft.fillRectangle(x1 + MARGIN, y1, x1+size1, y1+size1, BLACK);
		settings_variables[0]--; // settings_variables[0] <=> speed variable
		tft.drawNumber(x1 , y1, settings_variables[0], 3, false); //  settings_variables[0] <=> speed variable
	}
	if (arg==1){ // case "+" pressed
		tft.fillRectangle(x1+MARGIN, y1, x1+size1, y1+size1, BLACK);
		settings_variables[0]++;
		tft.drawNumber(x1, y1, settings_variables[0], 3, false); //  settings_variables[0] <=> speed variable
	}

	// Handling the mic_sens variation
	if (arg==2){ // case "-" pressed
		tft.fillRectangle(x1+ MARGIN, 2*y1, x1+size1, 2*y1+size1, BLACK);
		settings_variables[1]--; // settings_variables[0] <=> speed variable
		tft.drawNumber(x1 , 2*y1, settings_variables[1], 3, false);
	}
	if (arg==3){ // case "+" pressed
		tft.fillRectangle(x1+MARGIN, 2*y1, x1+size1, 2*y1+size1, BLACK);
		settings_variables[1]++;
		tft.drawNumber(x1 , 2*y1, settings_variables[1], 3, false);
	}


}

void handleClick_Settings(uint32_t arg=0)
{
  bool down = false;
  uint16_t x = 0;
  uint16_t y = 0;
  // parameters for back button
  uint16_t x1 = 0;
  uint16_t y1 = 0;
  uint16_t size1 = 50;


  // create a new set of buttons
  const int NUM_SETTINGS_BUTTONS = 5;
  const char* settings_menu_Items[] = {
		  "back", "-", "+", "-", "+"};
  Button* settings_buttons[NUM_SETTINGS_BUTTONS];

  // config the back button
  settings_buttons[0]= new Button(settings_menu_Items[0], x1, y1, size1, size1);
  settings_buttons[0]->setAction(home_page, back_from_settings); // back to the home page
  // config the speed button "-"
  settings_buttons[1]= new Button(settings_menu_Items[1], x1+size1+4*MARGIN, y1+size1+MARGIN, size1, size1);
  settings_buttons[1]->setAction(change_setting, 0); // "0" refers to "-" for the speed button
  // config the speed button "-"
  settings_buttons[2]= new Button(settings_menu_Items[2], tft.width()-size1-MARGIN, y1+size1+MARGIN, size1, size1);
  settings_buttons[2]->setAction(change_setting, 1); // "1" refers to "+" for the speed button
  // config the mic_sens button "-"
  settings_buttons[3]= new Button(settings_menu_Items[1], x1+size1+4*MARGIN, y1+2*(size1+MARGIN), size1, size1);
  settings_buttons[3]->setAction(change_setting, 2); // "0" refers to "-" for the mic_sens button
  // config the mic_sens button "-"
  settings_buttons[4]= new Button(settings_menu_Items[2], tft.width()-size1-MARGIN, y1+2*(size1+MARGIN), size1, size1);
  settings_buttons[4]->setAction(change_setting, 3); // "1" refers to "+" for the mic_sens button

  tft.init();
  touch.init();

  tft.drawString(0, size1 + MARGIN, "Speed :");
  tft.drawNumber(tft.width()/2 , size1 + MARGIN, settings_variables[0], 3, false); //  settings_variables[0] <=> speed variable
  tft.drawString(0, 2*(size1 + MARGIN), "Mic_Sens:");
  tft.drawNumber(tft.width()/2 , 2*(size1+MARGIN), settings_variables[1], 3, false); //  settings_variables[1] <=> mic_sens variable


  for (int i = 0; i < NUM_SETTINGS_BUTTONS; i++) {
	  settings_buttons[i]->draw(&tft);
  }

  while(true) {
	touch.readTouchData(x, y, down);
	for (int i = 0; i < NUM_SETTINGS_BUTTONS; i++) {
	  if (settings_buttons[i]->handle(x, y, down)) {
		  settings_buttons[i]->draw(&tft);
	  }
	}

	//wait(0.02);
  }

}


/******************************************************************************
 * Main
 *****************************************************************************/

int main() {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t w = tft.width();
  uint16_t h = tft.height();
  uint16_t size = (h - MARGIN*BUTTONS_NUM_LINES) / BUTTONS_NUM_LINES;
  uint16_t yoff = 5;

  if (my_flash.init() != 0) {
	  printf("Error initializing flash\n"); // init the flash memory
  }
  ///////////////////////////////////////////////
//  uint32_t test = 12;
//  int ret0 = my_flash.erase(settings_address, my_flash.get_sector_size(settings_address));
//  int ret = my_flash.program(&test , settings_address, my_flash.get_page_size());
//  uint32_t ret1 = my_flash.get_page_size();
//  uint32_t ret2 = my_flash.get_sector_size(settings_address);
//  uint32_t ret3 = my_flash.get_flash_size();
//  uint32_t ret4 = my_flash.get_flash_start();
//  uint32_t ret5 = my_flash.get_flash_start();
//  printf("erase : %d\n", ret0);
//  printf("program : %d\n", ret);
//  printf("get_page_size : %d\n", ret1);
//  printf("get_sector_size : %d\n", ret2);
//  printf("get_flash_size : %d\n", ret1);
//  printf("get_flash_start : %d\n", ret2);
////	  my_flash.program(settings_variables+1 , mic_sens_adress, 1);
//
//  printf("Settings saved \n");
//  printf("Saved speed : %d\n", settings_variables[0]);
//
//  my_flash.read(settings_variables, settings_address, my_flash.get_page_size());
//  printf("Speed read : %d\n", settings_variables[0]);

  ///////////////////////////////////////////
  read_variables(); // read the settings variables

  for (int i = 0; i < NUM_BUTTONS;i++) {
    x = MARGIN + (size + MARGIN) * (i % BUTTONS_PER_LINE);
    y = size*i + MARGIN;
    buttons[i] = new Button(menu_Items[i], x, y, w, size);
  }

  // setting the action for each button
  buttons[0]->setAction(handleClick_Battery, menu_Items[0][0]);
  buttons[1]->setAction(handleClick_Sound, menu_Items[1][0]);
  buttons[2]->setAction(handleClick_Settings, menu_Items[2][0]);

  // show the home screen
  home_page();

}
