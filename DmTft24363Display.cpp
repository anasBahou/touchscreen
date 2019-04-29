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

#include <stdio.h>
#include <math.h>
#include <DmTft24363Display.h>

using namespace sixtron;

DmTft24_363_Display::DmTft24_363_Display() {

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
	_battery = new MAX17201(&_i2c);

	printf("DmTft24_363_Display created");

}

DmTft24_363_Display::~DmTft24_363_Display() {

	printf("DmTft24_363_Dsiplay destroyed");

}

void DmTft24_363_Display::init() {

	// BUTTONS

	// settings
	_buttons[0] = new Button("settings", 0, 0, 100, 50);
	// back
	_buttons[1] = new Button("back", 0, 0, 50, 50);
	// "-" speed
	_buttons[2] = new Button("-", 70, 55, 50, 50);
	// "+" speed
	_buttons[3] = new Button("+", 285, 55, 50, 50);
	// "-" mic_sens
	_buttons[4] = new Button("-", 70, 110, 50, 50);
	// "+" mi_sens7
	_buttons[5] = new Button("+", 285, 110, 50, 50);

	// set up the interruption
	_touchItr->fall(DmTft24_363_Display::itrFunc);

}

void DmTft24_363_Display::setID(int pageID) {
	_pageID = pageID;
}

void DmTft24_363_Display::setBatteryLevel(int batteryLevel) {
	_batteryLevel = batteryLevel;
}

void DmTft24_363_Display::setAngle(int angle) {
	_previousAngle = _angle;
	_angle = angle;
}

void DmTft24_363_Display::refreshBatteryLevel() {

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
	uint8_t battery_level_int = (uint8_t)_batteryLevel;
	uint16_t level_to_pixel = (uint16_t)((_batteryLevel/100)*rect_width); // conversion to an integer
	uint16_t new_level_to_pixel = (uint16_t)((_battery->state_of_charge()/100)*rect_width); // conversion to an integer

	// erase the existing value
	_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, BLACK);
	_tft->fillRectangle(200 , 0 , 260, 20, BLACK); // erase the text <=> battery level

	// level to string
	char battery_level_str[6];
	sprintf(battery_level_str, "%d%s", _battery->state_of_charge(), "%"); // to convert battery_level to a string of a format "70.0 %"

	// draw the string
	_tft->drawString(200, 0, battery_level_str);

	if (_batteryLevel > 74) {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + new_level_to_pixel, y1_rect-1, GREEN);
	}
	else if ((20 < _batteryLevel)&&( _batteryLevel < 75)) {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + new_level_to_pixel, y1_rect-1, YELLOW);

	}else {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + new_level_to_pixel, y1_rect-1, RED);
	}
}

void DmTft24_363_Display::refreshAngle() {
	if (_pageID==0){ // in case the user is in the home_page

		float sound_angle_rad = (float)_previousAngle * 3.14/180;
		uint16_t circle_radius = 80;
		uint16_t x_sound1, y_sound1;
		// screen size
		uint16_t w = _tft->width();
		uint16_t h = _tft->height();

		x_sound1 = w/2 + circle_radius*cos(sound_angle_rad);
		y_sound1 = h/2 - circle_radius*sin(sound_angle_rad);

		float sound_angle_rad2 = (float)_angle * 3.14/180;
		uint16_t x_sound2, y_sound2;
		// screen size


		x_sound2 = w/2 + circle_radius*cos(sound_angle_rad2);
		y_sound2 = h/2 - circle_radius*sin(sound_angle_rad2);

		_tft->drawLine(w/2, h/2, x_sound1, y_sound1, BLACK); // erase the drawn line
		_tft->drawLine(w/2, h/2, x_sound2, y_sound2, RED);
		printf("direction updated\n");
	}
}

void DmTft24_363_Display::refresh() {
	if (_pageID==HOMEPAGE){ // in case the user is in the home_page
		refreshAngle();
		refreshBatteryLevel();
	}else if(_pageID==SETTINGSPAGE){
		// varibales
		uint16_t x1 = _tft->width()/2 ;
		uint16_t y1 = 50 + MARGIN;
		uint16_t size1 = 50;
		if (_speedChanged==1){ // redraw the speed value
			_tft->fillRectangle(x1 + MARGIN, y1, x1+size1, y1+size1, BLACK);
			_tft->drawNumber(x1 , y1, _settingsVariables[0], 3, false); //  _settingsVariables[0] <=> speed variable
		}
		if (_micSensChanged==1){ // redraw the mic_sens value
			_tft->fillRectangle(x1+MARGIN, 2*y1, x1+size1, 2*y1+size1, BLACK);
			_tft->drawNumber(x1 , 2*y1, _settingsVariables[1], 3, false); // _settingsVariables[1] <=> Mic_sense variable
		}
	}
}

void DmTft24_363_Display::readSettings() {
	// read blocs from flash memory
	_myflash->read(_settingsVariables , _settingsAddress, _myflash->get_page_size());

	printf("speed : %d\n", _settingsVariables[0]);
	printf("mic_sens : %d\n", _settingsVariables[1]);
}

void DmTft24_363_Display::saveSettings() {
	// erase blocks on flash memory
	_myflash->erase(_settingsAddress, _myflash->get_sector_size(_settingsAddress));
	// program blocks
	_myflash->program(_settingsVariables , _settingsAddress, _myflash->get_page_size());


	printf("Settings saved \n");
	printf("Saved speed : %d\n", _settingsVariables[0]);
	printf("Saved mic_sens : %d\n", _settingsVariables[1]);
}

void DmTft24_363_Display::itrFunc(){
	//variables
	bool down = false;
	uint16_t x = 0;
	uint16_t y = 0;
	// read touch data
	_touch->readTouchData(x, y, down);
	for (int i = 0; i < 6; i++) {
		if (_buttons[i]->handle(x, y, down)) {
			_buttons[i]->draw(_tft);
			switch(i){
				case 0: // settings button
					_pageID = SETTINGSPAGE;
					break;
				case 1: // back button
					_pageID = HOMEPAGE;
					break;
				case 2: // case "-" pressed
					_speedChanged = 1; // the settings were changed
					_settingsVariables[0]--; // _settingsVariables[0] <=> speed variable
					break;
				case 3: // case "+" pressed
					_speedChanged = 1; // the settings were changed
					_settingsVariables[0]++;
					break;
				case 4: // case "-" pressed
					_micSensChanged = 1; // the settings were changed
					_settingsVariables[1]--; // _settingsVariables[1] <=> Mic_sense variable
					break;
				case 5: // case "+" pressed
					_micSensChanged = 1; // the settings were changed
					_settingsVariables[1]++;
					break;

			}
		}
	}
}

void DmTft24_363_Display::homePage() {
	// set page ID
	_pageID = HOMEPAGE;

	// variables display battery
	uint16_t x0_rect = 200;
	uint16_t y0_rect = 20;
	uint16_t x1_rect = 230;
	uint16_t y1_rect = 30;
	uint16_t rect_width = x1_rect -1 - (x0_rect +1) ; // -/+ 1 to avoid the pixels of the white rectangle

	// screen size
	uint16_t w = _tft->width();
	uint16_t h = _tft->height();

	// case the settings were changed ==> save the settings
	if ((_speedChanged== 1)||(_micSensChanged==1)){
	  saveSettings();
	}

	// display
	_buttons[0]->draw(_tft);

	// get the sound direction
	float sound_angle_rad = (float)_angle * 3.14/180;
	uint16_t circle_radius = 80;
	uint16_t x_sound, y_sound;

	x_sound = w/2 + circle_radius*cos(sound_angle_rad);
	y_sound = h/2 - circle_radius*sin(sound_angle_rad);


	// battery
	uint8_t battery_level_int = (uint8_t)_batteryLevel;
	uint16_t level_to_pixel = (uint16_t)((_batteryLevel/100)*rect_width); // conversion to an integer
	char battery_level_str[6];
	const char percent[] = "%";
	sprintf(battery_level_str, "%d%s", battery_level_int, percent); // to convert battery_level to a string of a format "70.0 %"

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

	}else {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, RED);
	}

}

void DmTft24_363_Display::settingsPage() {
	// set page ID
	_pageID = SETTINGSPAGE;

	// parameters for back button
	uint16_t size1 = 50;

	// initialization
	_tft->init();
	_touch->init();

	// display
	_tft->drawString(0, size1 + MARGIN, "Speed :");
	_tft->drawNumber(_tft->width()/2 , size1 + MARGIN, _settingsVariables[0], 3, false); //  _settingsVariables[0] <=> speed variable
	_tft->drawString(0, 2*(size1 + MARGIN), "Mic_Sens:");
	_tft->drawNumber(_tft->width()/2 , 2*(size1+MARGIN), _settingsVariables[1], 3, false); //  _settingsVariables[1] <=> mic_sens variable

	// display buttons
	for (int i = 1; i < 6; i++) {
		_buttons[i]->draw(_tft);
	}

}

