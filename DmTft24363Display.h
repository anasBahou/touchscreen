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
#define NUM_BUTTONS  6

class DmTft24_363_Display {
public:
    enum class pages : char {
        HomePage            = 0,
        SettingsPage        = 1
    };

	DmTft24_363_Display(DmTftIli9341* display_controler, DmTouch* touch_controler, InterruptIn* interrupt, FlashIAP* flash);

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

	void homePage();

	void settingsPage();

	void changeSettings();


	int _pageID;
	int _batteryLevel;
	int _angle;
	int _previousAngle;
	uint16_t _settingsVariables[NUM_SETTINGS_VARIABLES];
	bool _settingsChanged;
	Button* _buttons[NUM_BUTTONS];
	FlashIAP* _myflash;
	DmTftIli9341* tft;
	DmTouch* touch;
	InterruptIn* _touchItr;

};

#endif /* DMTFT24363DISPLAY_H_ */
