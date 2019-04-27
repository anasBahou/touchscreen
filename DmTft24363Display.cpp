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

	for (int i = 0 ; i < NUM_SETTINGS_VARIABLES ; i++) {
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

}

void DmTft24_363_Display::setID(int pageID) {

}

void DmTft24_363_Display::setBatteryLevel(int batteryLevel) {

}

void DmTft24_363_Display::setAngle(int angle) {

}

void DmTft24_363_Display::refreshBatteryLevel() {

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

}

void DmTft24_363_Display::settingsPage() {

}

void DmTft24_363_Display::changeSettings() {

}
