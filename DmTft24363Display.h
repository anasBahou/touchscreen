/*
 * DmTft24363Display.h
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

#ifndef DMTFT24363DISPLAY_H_
#define DMTFT24363DISPLAY_H_

#define NUM_SETTINGS_VARIABLES  2
#define NUM__settingsVariables  2
#define NUM_BUTTONS  6
#define HOMEPAGE 0
#define SETTINGSPAGE 1
#define MARGIN  5

class DmTft24_363_Display {
public:

	DmTft24_363_Display();

	~DmTft24_363_Display();

	void init();

	void setID(int pageID);

	void setBatteryLevel(int batteryLevel);

	void setAngle(int angle);

	void refreshBatteryLevel();

	void refreshAngle();

	void refresh();

private:
	void readSettings();

	void saveSettings();

	void homePage(uint32_t arg);

	void settingsPage();

	void changeSettings(uint32_t arg);

	void itrFunc();


	int _pageID;
	int _batteryLevel;
	int _angle;
	int _previousAngle;
	uint16_t _settingsVariables[NUM_SETTINGS_VARIABLES];
	uint32_t _settingsAddress = 0x080FF000;
	bool _settingsChanged;
	Button* _buttons[NUM_BUTTONS];
	FlashIAP* _myflash;
	DmTftIli9341* _tft;
	DmTouch* _touch;
	InterruptIn* _touchItr;

};

#endif /* DMTFT24363DISPLAY_H_ */
