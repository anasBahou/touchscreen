/*
 * DmTft24363Display.cpp
 *
 *  Created on: 15 avr. 2019
 *      Author: sixtron-training
 */

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

DmTft24_363_Display::DmTft24_363_Display() {

	printf("Creation of DmTft24_363_Display...\n");

	_pageID = HOMEPAGE;
	_batteryLevel = 0;
	_angle = 0;
	_previousAngle = 0;

	for (int i = 0 ; i < NUM_SETTINGS_VARIABLES ; i++) {
		_settingsVariables[i] = 0;
	}

	_speedChanged  = false;
	_micSensChanged = false;

	_myflash = new FlashIAP;
	_tft = new DmTftIli9341(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);
	_touch = new DmTouch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_SCK);
	_touchItr = new InterruptIn(DIO4);
	_i2c = new I2C(I2C1_SDA, I2C1_SCL);
	_battery = new MAX17201(_i2c);

	printf("... completed ! \n\n");

}

DmTft24_363_Display::~DmTft24_363_Display() {

	printf("DmTft24_363_Dsiplay destroyed");

}

void DmTft24_363_Display::itrFunc() {

}

void DmTft24_363_Display::init() {

	printf("Initialization...\n");

	/*
	 * OBJECT INITILIZATION
	 */
	_tft->init(); // screen initialization
	_touch->init(); // touch controller initialization
	_myflash->init(); // flash memory initialization
	_touchItr->fall(callback(this, &DmTft24_363_Display::itrFunc)); // set up the function linked to the interruption
	_battery->configure(1, 800, 3.3, false, false);

	/*
	 * BUTTONS INITILIZATION
	 */
	_buttons[0] = new Button("settings", 0, 0, 100, 50); // settings button initialization
	_buttons[1] = new Button("back", 0, 0, 50, 50); // back button initialization
	_buttons[2] = new Button("-", 70, 55, 50, 50); // "-" speed button initialization
	_buttons[3] = new Button("+", 190, 55, 50, 50); // "+" speed button initialization
	_buttons[4] = new Button("-", 70, 110, 50, 50); // "-" mic_sens button initialization
	_buttons[5] = new Button("+", 190, 110, 50, 50); // // "+" mi_sens button initialization

	printf("... completed !\n\n");

}

void DmTft24_363_Display::setID(int pageID) {

	printf("Set pageID...\n");
	_pageID = pageID;
	printf("... completed !\n\n");

}

void DmTft24_363_Display::setBatteryLevel() {

	printf("Set batteryLevel...\n");
	_batteryLevel = _battery->state_of_charge();
	printf(" Battery Level : %.3f\n");
	printf("... completed !\n\n");

}

void DmTft24_363_Display::setAngle(int angle) {

}

void DmTft24_363_Display::refreshBatteryLevel() {

	printf("Refreshing batteryLevel...\n");

	// variables display battery
	uint16_t x0_rect = 200;
	uint16_t y0_rect = 20;
	uint16_t x1_rect = 230;
	uint16_t y1_rect = 30;
	uint16_t rect_width = x1_rect -1 - (x0_rect +1) ; // -/+ 1 to avoid the pixels of the white rectangle

	// screen size
	uint16_t w = _tft->width();
	uint16_t h = _tft->height();

	// battery
	//uint8_t battery_level_int = (uint8_t)_batteryLevel;
	uint16_t level_to_pixel = (uint16_t)((_batteryLevel/100)*rect_width); // conversion to an integer
	uint16_t new_level_to_pixel = (uint16_t)((_batteryLevel/100)*rect_width); // conversion to an integer

	// erase the existing value
	_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, BLACK);
	_tft->fillRectangle(200 , 0 , 260, 20, BLACK); // erase the text <=> battery level

	// level to string
	char battery_level_str[6];
	sprintf(battery_level_str, "%d%s", _batteryLevel, "%"); // to convert battery_level to a string of a format "70.0 %"

	// draw the string
	_tft->drawString(200, 0, battery_level_str);

	if (_batteryLevel > 66) {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + new_level_to_pixel, y1_rect-1, GREEN);
	}
	else if ( (33 < _batteryLevel) && (_batteryLevel < 67) ) {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + new_level_to_pixel, y1_rect-1, YELLOW);

	}else {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + new_level_to_pixel, y1_rect-1, RED);
	}

	printf("... completed !\n\n");

}

void DmTft24_363_Display::refreshAngle() {

}

void DmTft24_363_Display::refresh() {

}

void DmTft24_363_Display::readSettings() {

}

void DmTft24_363_Display::saveSettings() {

}

void DmTft24_363_Display::homePage() {

	printf("Home page...\n");

	// variables display battery
	uint16_t x0_rect = 200;
	uint16_t y0_rect = 20;
	uint16_t x1_rect = 230;
	uint16_t y1_rect = 30;
	uint16_t rect_width = x1_rect -1 - (x0_rect + 1) ; // -/+ 1 to avoid the pixels of the white rectangle

	// screen size
	uint16_t w = _tft->width();
	uint16_t h = _tft->height();

	// case the settings were changed ==> save the settings
	if ((_speedChanged== 1)||(_micSensChanged==1)){
	  saveSettings();
	}

	// display settings button
	_buttons[0]->draw(_tft);

	// get the sound direction
	float sound_angle_rad = (float)_angle * 3.14/180;
	uint16_t circle_radius = 80;
	uint16_t x_sound, y_sound;

	x_sound = w/2 + circle_radius*cos(sound_angle_rad);
	y_sound = h/2 - circle_radius*sin(sound_angle_rad);


	// battery
    uint8_t battery_level_int = (uint8_t)_batteryLevel;
	uint16_t level_to_pixel = (uint16_t)((_batteryLevel/100)*rect_width);
	char battery_level_str[6];
	sprintf(battery_level_str, "%d%s", _batteryLevel, "%"); // to convert battery_level to a string of a format "70.0 %"

	// display direction
	_tft->drawCircle(w/2, h/2, circle_radius, WHITE);
	_tft->drawPoint(w/2, h/2);
	_tft->drawLine(w/2, h/2, x_sound, y_sound, RED);

	// drawing the Battery level
	_tft->drawString(200, 0, battery_level_str);
	_tft->drawRectangle(x0_rect, y0_rect, x1_rect, y1_rect, WHITE);
	_tft->drawVerticalLine(x1_rect+1, y0_rect+4, 2, WHITE);

	if (_batteryLevel > 74) {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, GREEN);
	}
	else if ((20 < _batteryLevel)&&( _batteryLevel < 75)) {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, YELLOW);

	}
	else {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, RED);
	}

	printf("... completed ! \n\n");

}

void DmTft24_363_Display::settingsPage() {

	printf("Settings page...\n");

	// display
	_tft->drawString(0, 70, "Speed");
	_tft->drawNumber(135 , 70, _settingsVariables[0], 3, false); //  _settingsVariables[0] <=> speed variable
	_tft->drawString(0, 125, "Mic Sens");
	_tft->drawNumber(135 , 125, _settingsVariables[1], 3, false); //  _settingsVariables[1] <=> mic_sens variable

	// display buttons
	for (int i = 1; i < 6; i++) {
		_buttons[i]->draw(_tft);
	}

	printf("... completed ! \n\n");

}

void DmTft24_363_Display::changeSettings() {

}


