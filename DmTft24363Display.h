/*
 * DmTft24363Display.h
 *
 *  Created on: 15 avr. 2019
 *      Author: sixtron-training
 */

#include "mbed.h"
#include "DmTftIli9341.h"
#include "DmTouch.h"
#include "PinNames.h"
#include "max17201.h"
#include "DisplayDriver.h"

#include <math.h>
#include <stdio.h>

#ifndef DMTFT24363DISPLAY_H_
#define DMTFT24363DISPLAY_H_

#define NUM_SETTINGS_VARIABLES  2
#define NUM_BUTTONS  6
#define HOMEPAGE 0
#define SETTINGSPAGE 1
#define SPEED 0
#define MIC_SENS 1
#define SETTINGS 0
#define BACK 1
#define SPEED_MINUS 2
#define SPEED_PLUS 3
#define MIC_SENS_MINUS 4
#define MIC_SENS_PLUS 5

class DmTft24_363_Display {
public:

	DmTft24_363_Display();

	DmTft24_363_Display(FlashIAP* flash, DmTftIli9341* tft, DmTouch* touch, InterruptIn* touchItr, I2C* i2c);

	~DmTft24_363_Display();

	void init();

	void setID(int pageID);

	void setAngle(int angle);

	void refreshBatteryLevel();

	void refreshAngle();

	void refresh();

	void homePage();

	void settingsPage();

	void handleTouchEvent();

	void readSettings();

	void saveSettings();

private:
	//void readSettings();

	//void saveSettings();

	void itrFunc();


	int _pageID;
	bool _changePage;
	float _batteryLevel;
	int _angle;
	int _previousAngle;
	uint16_t _settingsVariables[NUM_SETTINGS_VARIABLES];
	uint32_t _settingsAddress;
	bool _speedChanged;
	bool _micSensChanged;
	GraphObject_t _buttons[NUM_BUTTONS];
	FlashIAP* _myflash;
	DmTftIli9341* _tft;
	DmTouch* _touch;
	InterruptIn* _touchItr;
	I2C* _i2c;
	sixtron::MAX17201* _battery;
	EventQueue* _queue;


};

#endif /* DMTFT24363DISPLAY_H_ */
