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

DmTft24_363_Display::DmTft24_363_Display() {

	_pageID = HOMEPAGE;
	_batteryLevel = 0;
	_angle = 0;
	_previousAngle = 0;

	for (int i = 0 ; i < NUM__settingsVariables ; i++) {
		_settingsVariables[i] = 0;
	}

	_settingsChanged  = false;

	_myflash = new FlashIAP;
	_tft = new DmTftIli9341(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);
	_touch = new DmTouch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_SCK);
	_touchItr = new InterruptIn(DIO4);

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
	if (_pageID==0){ // in case the user is in the home_page
		refreshAngle();
		refreshBatteryLevel();
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
	for (int i = 1; i < 6; i++) {
		if (_buttons[i]->handle(x, y, down)) {
			_buttons[i]->draw(_tft);
		}
	}
}

void DmTft24_363_Display::homePage(uint32_t arg=0) {

}

void DmTft24_363_Display::settingsPage() {
	// set page ID
	_pageID = 1;

	// reset the settings flag
	_settingsChanged = 0;

	// parameters for back button
	uint16_t x1 = 0;
	uint16_t y1 = 0;
	uint16_t size1 = 50;

	// config the back button
	_buttons[1]->setAction(DmTft24_363_Display::homePage, 0); // back to the home page
	// config the speed button "-" for the speed
	_buttons[2]->setAction(DmTft24_363_Display::changeSettings, 0); // "0" refers to "-" for the speed button
	// config the speed button "+" for the speed
	_buttons[3]->setAction(DmTft24_363_Display::changeSettings, 1); // "1" refers to "+" for the speed button
	// config the mic_sens button "-" for the mic_sens
	_buttons[4]->setAction(DmTft24_363_Display::changeSettings, 2); // "0" refers to "-" for the mic_sens button
	// config the mic_sens button "+" for the mic_sens
	_buttons[5]->setAction(DmTft24_363_Display::changeSettings, 3); // "1" refers to "+" for the mic_sens button

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

void DmTft24_363_Display::changeSettings(uint32_t arg) {
	// varibales
	uint16_t x1 = _tft->width()/2 ;
	uint16_t y1 = 50 + MARGIN;
	uint16_t size1 = 50;
	//char speed_str[4];
	//char mic_sens_str[4];

	// the settings were changed
	_settingsChanged = 1;
	// handling the speed variation
	if (arg==0){ // case "-" pressed
		_tft->fillRectangle(x1 + MARGIN, y1, x1+size1, y1+size1, BLACK);
		_settingsVariables[0]--; // _settingsVariables[0] <=> speed variable
		_tft->drawNumber(x1 , y1, _settingsVariables[0], 3, false); //  _settingsVariables[0] <=> speed variable
	}
	if (arg==1){ // case "+" pressed
		_tft->fillRectangle(x1+MARGIN, y1, x1+size1, y1+size1, BLACK);
		_settingsVariables[0]++;
		_tft->drawNumber(x1, y1, _settingsVariables[0], 3, false); //  _settingsVariables[0] <=> speed variable
	}

	// handling the mic_sens variation
	if (arg==2){ // case "-" pressed
		_tft->fillRectangle(x1+ MARGIN, 2*y1, x1+size1, 2*y1+size1, BLACK);
		_settingsVariables[1]--; // _settingsVariables[0] <=> speed variable
		_tft->drawNumber(x1 , 2*y1, _settingsVariables[1], 3, false);
	}
	if (arg==3){ // case "+" pressed
		_tft->fillRectangle(x1+MARGIN, 2*y1, x1+size1, 2*y1+size1, BLACK);
		_settingsVariables[1]++;
		_tft->drawNumber(x1 , 2*y1, _settingsVariables[1], 3, false);
	}
}
