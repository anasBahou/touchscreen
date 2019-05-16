/*
 * DmTft24363Display.cpp
 *
 *  Created on: 15 avr. 2019
 *      Author: sixtron-training
 */

#include "DmTft24363Display.h"

using namespace sixtron;

DmTft24_363_Display::DmTft24_363_Display() {

	printf("DmTft24_363_Display created \n\n");

}

DmTft24_363_Display::DmTft24_363_Display(FlashIAP* flash, DmTftIli9341* tft, DmTouch* touch, InterruptIn* touchItr, I2C* i2c, uint32_t settingsAddress) {

	printf("Creation of DmTft24_363_Display...");

	_pageID = HOMEPAGE;
	_changePage = false;
	_speedChanged  = false;
	_micSensChanged = false;
	_batteryLevel = 0;
	_previousBatteryLevel = 0;
	_angle = 0;
	_previousAngle = 0;
	_minMicSens = 0;
	_maxMicSens = 1;
	_minSpeed = 0;
	_maxSpeed = 1;
	_settingsAddress = settingsAddress;
	_myflash = flash;
	_tft = tft;
	_touch = touch;
	_touchItr = touchItr;
	_i2c = i2c;
	_battery = new MAX17201(_i2c);
	_queue = mbed_event_queue();

	printf("... completed \n\n");

}

DmTft24_363_Display::~DmTft24_363_Display() {

	printf("DmTft24_363_Dsiplay destroyed\n\n");

}

void  DmTft24_363_Display::handleTouchEvent()
{

	_queue->call_in(200, callback(_touchItr, &InterruptIn::enable_irq));

	uint8_t which;

	if ( _pageID == HOMEPAGE ) {
		if ( GraphObjectTouched(_buttons, NUM_BUTTONS, &which, _tft, _touch) == GO_STATUS_NOERR ) {
			_changePage = true;
			_pageID = SETTINGSPAGE;
			_buttons[SETTINGS].TouchActive = false;
			return;
		}
	}

	else if ( _pageID == SETTINGSPAGE ) {
		if ( GraphObjectTouched(_buttons, NUM_BUTTONS, &which, _tft, _touch) == GO_STATUS_NOERR) {
			if ( which == BACK ) {
				_changePage = true;
				_pageID = HOMEPAGE;
				_buttons[BACK].TouchActive = false;
				_buttons[SPEED_MINUS].TouchActive = false;
				_buttons[SPEED_PLUS].TouchActive = false;
				_buttons[MIC_SENS_MINUS].TouchActive = false;
				_buttons[MIC_SENS_PLUS].TouchActive = false;
				return;
			}

			else if ( which == SPEED_MINUS ) {
				if ( ( _minSpeed <= _settingsVariables[SPEED] ) && ( _settingsVariables[SPEED] <= _maxSpeed ) ) {
					_speedChanged = true;
					_settingsVariables[SPEED]-=0.05;
				}
				return;
			}

			else if ( which == SPEED_PLUS ) {
				if ( ( _minSpeed <= _settingsVariables[SPEED] ) && ( _settingsVariables[SPEED] <= _maxSpeed ) ) {
					_speedChanged = true;
					_settingsVariables[SPEED]+=0.05;
				}
				return;
			}

			else if ( which == MIC_SENS_MINUS ) {
				if ( ( _minMicSens <= _settingsVariables[MIC_SENS] ) && ( _settingsVariables[MIC_SENS] <= _maxMicSens ) ) {
					_micSensChanged = true;
					_settingsVariables[MIC_SENS]-=0.015;
				}
				return;
			}

			else if ( which == MIC_SENS_PLUS ) {
				if ( ( _minMicSens <= _settingsVariables[MIC_SENS] ) && ( _settingsVariables[MIC_SENS] <= _maxMicSens ) ) {
					_micSensChanged = true;
					_settingsVariables[MIC_SENS]+=0.015;
				}
				return;
			}

		}
	}

}


void DmTft24_363_Display::itrFunc() {
	_touchItr->disable_irq();
	_queue->call(callback(this, &DmTft24_363_Display::handleTouchEvent));
	printf("touch\n");
	return;
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
	_battery->configure(1, 800, 3.3, false, false); //battery configuration

	/*
	 * BUTTONS INITILIZATION
	 */
	// settings button initialization
	_buttons[SETTINGS].Id = SETTINGS;
	_buttons[SETTINGS].Type = GO_RECTANGLE;
	_buttons[SETTINGS].Xpos = 0;
	_buttons[SETTINGS].Ypos = 0;
	_buttons[SETTINGS].Width = 100;
	_buttons[SETTINGS].Height = 50;
	_buttons[SETTINGS].FrontColor = WHITE; //color of borders
	_buttons[SETTINGS].DoFill = true;
	_buttons[SETTINGS].FillColor = BLACK;
	_buttons[SETTINGS].TouchActive = false;

	// back button initialization
	_buttons[BACK].Id = BACK;
	_buttons[BACK].Type = GO_RECTANGLE;
	_buttons[BACK].Xpos = 0;
	_buttons[BACK].Ypos = 0;
	_buttons[BACK].Width = 50;
	_buttons[BACK].Height = 50;
	_buttons[BACK].FrontColor = WHITE; //color of borders
	_buttons[BACK].DoFill = true;
	_buttons[BACK].FillColor = BLACK;
	_buttons[BACK].TouchActive = false;

	// "-" speed button initialization
	_buttons[SPEED_MINUS].Id = SPEED_MINUS;
	_buttons[SPEED_MINUS].Type = GO_RECTANGLE;
	_buttons[SPEED_MINUS].Xpos = 70;
	_buttons[SPEED_MINUS].Ypos =55;
	_buttons[SPEED_MINUS].Width = 50;
	_buttons[SPEED_MINUS].Height = 50;
	_buttons[SPEED_MINUS].FrontColor = WHITE; //color of borders
	_buttons[SPEED_MINUS].DoFill = true;
	_buttons[SPEED_MINUS].FillColor = BLACK;
	_buttons[SPEED_MINUS].TouchActive = false;

	// "+" speed button initialization
	_buttons[SPEED_PLUS].Id = SPEED_PLUS,
	_buttons[SPEED_PLUS].Type = GO_RECTANGLE;
	_buttons[SPEED_PLUS].Xpos = 190;
	_buttons[SPEED_PLUS].Ypos =55;
	_buttons[SPEED_PLUS].Width = 50;
	_buttons[SPEED_PLUS].Height = 50;
	_buttons[SPEED_PLUS].FrontColor = WHITE; //color of borders
	_buttons[SPEED_PLUS].DoFill = true;
	_buttons[SPEED_PLUS].FillColor = BLACK;
	_buttons[SPEED_PLUS].TouchActive = false;

	// "-" mic_sens button initialization
	_buttons[MIC_SENS_MINUS].Id = MIC_SENS_MINUS;
	_buttons[MIC_SENS_MINUS].Type = GO_RECTANGLE;
	_buttons[MIC_SENS_MINUS].Xpos = 70;
	_buttons[MIC_SENS_MINUS].Ypos =110;
	_buttons[MIC_SENS_MINUS].Width = 50;
	_buttons[MIC_SENS_MINUS].Height = 50;
	_buttons[MIC_SENS_MINUS].FrontColor = WHITE; //color of borders
	_buttons[MIC_SENS_MINUS].DoFill = true;
	_buttons[MIC_SENS_MINUS].FillColor = BLACK;
	_buttons[MIC_SENS_MINUS].TouchActive = false;

	// "+" mi_sens button initialization
	_buttons[MIC_SENS_PLUS].Id = MIC_SENS_PLUS;
	_buttons[MIC_SENS_PLUS].Type = GO_RECTANGLE;
	_buttons[MIC_SENS_PLUS].Xpos = 190;
	_buttons[MIC_SENS_PLUS].Ypos =110;
	_buttons[MIC_SENS_PLUS].Width = 50;
	_buttons[MIC_SENS_PLUS].Height = 50;
	_buttons[MIC_SENS_PLUS].FrontColor = WHITE; //color of borders
	_buttons[MIC_SENS_PLUS].DoFill = true;
	_buttons[MIC_SENS_PLUS].FillColor = BLACK;
	_buttons[MIC_SENS_PLUS].TouchActive = false;

	/*
	 * LEVEL BATTERY INITIALIZATION
	 */
	 wait_ms(1000); // wait for battery configuration
	 _batteryLevel = _battery->state_of_charge();

	 /*
	  * SETTINGS VARIABLES AND MIC SENS INITIALIZATION
	  */
	 readSettings();

	printf("...completed\n\n");
	return;

}

void DmTft24_363_Display::setAngle(int angle) {

	_previousAngle = _angle;
	_angle = angle;
	return;

}

void DmTft24_363_Display::setBatteryLevel(float batteryLevel) {

	_previousBatteryLevel = _batteryLevel;
	_batteryLevel = batteryLevel;;
	return;

}

void DmTft24_363_Display::refreshBatteryLevel() {

	setBatteryLevel(_battery->state_of_charge());

	if ( _previousBatteryLevel != _batteryLevel ) {

		// variables display battery
		uint16_t x0_rect = 200;
		uint16_t y0_rect = 20;
		uint16_t x1_rect = 230;
		uint16_t y1_rect = 30;
		uint16_t rect_width = x1_rect -1 - (x0_rect +1) ; // -/+ 1 to avoid the pixels of the white rectangle

		// battery
		uint16_t level_to_pixel = (uint16_t)((_batteryLevel/100)*rect_width); // conversion to an integer

		// erase the existing value
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + rect_width, y1_rect-1, BLACK);
		_tft->fillRectangle(200 , 0 , 260, 16, BLACK); // erase the text <=> battery level

		// level to string
		char battery_level_str[6];
		sprintf(battery_level_str, "%.0f%s", _batteryLevel, "%"); // to convert battery_level to a string of a format "70.0 %"

		// draw the string
		_tft->drawString(200, 0, battery_level_str);

		if (_batteryLevel > 66) {
			_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, GREEN);
		}
		else if ( (33 < _batteryLevel) && (_batteryLevel < 67) ) {
			_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, YELLOW);

		}
		else {
			_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, RED);
		}
	}
	return;

}

void DmTft24_363_Display::refreshAngle() {

	if ( _previousAngle != _angle ) {

		// screen size
		uint16_t w = _tft->width();
		uint16_t h = _tft->height();

		// erase old direction line
		float sound_angle_rad = (float)_previousAngle * 3.14/180;
		uint16_t circle_radius = 80;
		uint16_t x_sound1, y_sound1;

		x_sound1 = w/2 + circle_radius*cos(sound_angle_rad);
		y_sound1 = h/2 - circle_radius*sin(sound_angle_rad);


		// display new direction line
		float sound_angle_rad2 = (float)_angle * 3.14/180;
		uint16_t x_sound2, y_sound2;

		x_sound2 = w/2 + circle_radius*cos(sound_angle_rad2);
		y_sound2 = h/2 - circle_radius*sin(sound_angle_rad2);

		// draw
		_tft->drawLine(w/2, h/2, x_sound1, y_sound1, BLACK); // erase the drawn line
		_tft->drawPoint(x_sound1, y_sound1);
		_tft->drawLine(w/2, h/2, x_sound2, y_sound2, RED);
	}

	return;
}

void DmTft24_363_Display::refresh() {

	if (_pageID==HOMEPAGE){ // in case the user is in the home_page

		if (_changePage == 1) {
			_changePage = 0;
			_tft->clearScreen(BLACK);
			homePage();
		}

		refreshAngle();
		refreshBatteryLevel();

	}
	else if(_pageID==SETTINGSPAGE){

		char speed_c[5];
		char mic_sens_c[5];
		sprintf(speed_c, "%.1f", _settingsVariables[SPEED]*100);
		sprintf(mic_sens_c, "%.1f", _settingsVariables[MIC_SENS]*100);
		uint16_t x1 = _tft->width()/2 ;
		uint16_t y1 = 55;
		uint16_t size1 = 50;
		if (_changePage == 1) {
			_changePage = 0;
			_tft->clearScreen(BLACK);
			settingsPage();
		}
		if (_speedChanged==1){ // redraw the speed value
			_tft->fillRectangle(x1 + 5, y1, x1+size1+15, y1+size1, BLACK);
			_tft->drawString(135 , 70, speed_c);
		}
		if (_micSensChanged==1){ // redraw the mic_sens value
			_tft->fillRectangle(x1+5, 2*y1, x1+size1+15, 2*y1+size1, BLACK);
			_tft->drawString(135 , 125, mic_sens_c);
		}
	}

	return;

}

void DmTft24_363_Display::readSettings() {

	// read blocks from flash memory
	_myflash->read(_settingsVariables, _settingsAddress, sizeof(_settingsVariables));//_myflash->get_page_size());

	if (isnan(_settingsVariables[SPEED]) != 0) {
		_settingsVariables[SPEED] = 0.95;
	}

	if (isnan(_settingsVariables[MIC_SENS]) != 0) {
		_settingsVariables[MIC_SENS] = 0.30;
	}

	printf("speed : %f\n", _settingsVariables[SPEED]);
	printf("mic_sens : %f\n", _settingsVariables[MIC_SENS]);
	return;

}

void DmTft24_363_Display::saveSettings() {

	// erase blocks on flash memory
	_myflash->erase(_settingsAddress, _myflash->get_sector_size(_settingsAddress));
	// program blocks
	_myflash->program(_settingsVariables , _settingsAddress, sizeof(_settingsVariables));//_myflash->get_page_size());

	printf("Settings saved \n");
	printf("Saved speed : %f\n", _settingsVariables[SPEED]);
	printf("Saved mic_sens : %f\n", _settingsVariables[MIC_SENS]);
	return;

}

void DmTft24_363_Display::homePage() {

	printf("Home page...");

	// case the settings were changed ==> save the settings
	if ((_speedChanged== 1)||(_micSensChanged==1)){
		_speedChanged = false;
		_micSensChanged = false;
		saveSettings();
	}

	// screen size
	uint16_t w = _tft->width();
	uint16_t h = _tft->height();

	/*
	 * BATTERY
	 */
	uint16_t x0_rect = 200;
	uint16_t y0_rect = 20;
	uint16_t x1_rect = 230;
	uint16_t y1_rect = 30;
	uint16_t rect_width = x1_rect -1 - (x0_rect + 1) ; // -/+ 1 to avoid the pixels of the white rectangle
	uint16_t level_to_pixel = (uint16_t)((_batteryLevel/100)*rect_width);
	char battery_level_str[6];
	sprintf(battery_level_str, "%.0f%s", _batteryLevel, "%"); // to convert battery_level to a string of a format "70.0 %"

	_tft->drawString(200, 0, battery_level_str);
	_tft->drawRectangle(x0_rect, y0_rect, x1_rect, y1_rect, WHITE);
	_tft->drawVerticalLine(x1_rect+1, y0_rect+4, 2, WHITE);

	if (_batteryLevel > 66) {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, GREEN);
	}
	else if ( (33 < _batteryLevel) && (_batteryLevel < 67) ) {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, YELLOW);

	}
	else {
		_tft->fillRectangle(x0_rect+1 , y0_rect+1 , x0_rect+1 + level_to_pixel, y1_rect-1, RED);
	}

	/*
	 * ANGLE
	 */
	float sound_angle_rad = (float)_angle * 3.14/180;
	uint16_t circle_radius = 80;
	uint16_t x_sound, y_sound;

	x_sound = w/2 + circle_radius*cos(sound_angle_rad);
	y_sound = h/2 - circle_radius*sin(sound_angle_rad);

	_tft->drawCircle(w/2, h/2, circle_radius, WHITE);
	_tft->drawPoint(w/2, h/2);
	_tft->drawLine(w/2, h/2, x_sound, y_sound, RED);

	/*
	 * BUTTON
	 */
	GraphObjectDraw(&_buttons[SETTINGS], 0, true, true, _tft, _touch);
	_tft->drawStringCentered(_buttons[SETTINGS].Xpos, _buttons[SETTINGS].Ypos, _buttons[SETTINGS].Width, _buttons[SETTINGS].Height, "settings");

	printf("...completed\n\n");
	return;



}

void DmTft24_363_Display::settingsPage() {

	printf("Settings page...");

	char speed_c[5];
	char mic_sens_c[5];
	sprintf(speed_c, "%.1f", _settingsVariables[SPEED]*100);
	sprintf(mic_sens_c, "%.1f", _settingsVariables[MIC_SENS]*100);


	_tft->drawString(0, 70, "Speed");
	_tft->drawString(135 , 70, speed_c); //  _settingsVariables[0] <=> speed variable
	_tft->drawString(0, 125, "Mic Sens");
	_tft->drawString(135 , 125, mic_sens_c); //  _settingsVariables[1] <=> mic_sens variable

	/*
	 * BUTTONS
	 */
	GraphObjectDraw(&_buttons[BACK], 0, true, true, _tft, _touch);
	_tft->drawStringCentered(_buttons[BACK].Xpos, _buttons[BACK].Ypos, _buttons[BACK].Width, _buttons[BACK].Height, "back");
	GraphObjectDraw(&_buttons[SPEED_MINUS], 0, true, true, _tft, _touch);
	_tft->drawStringCentered(_buttons[SPEED_MINUS].Xpos, _buttons[SPEED_MINUS].Ypos, _buttons[SPEED_MINUS].Width, _buttons[SPEED_MINUS].Height, "-");
	GraphObjectDraw(&_buttons[SPEED_PLUS], 0, true, true, _tft, _touch);
	_tft->drawStringCentered(_buttons[SPEED_PLUS].Xpos, _buttons[SPEED_PLUS].Ypos, _buttons[SPEED_PLUS].Width, _buttons[SPEED_PLUS].Height, "+");
	GraphObjectDraw(&_buttons[MIC_SENS_MINUS], 0, true, true, _tft, _touch);
	_tft->drawStringCentered(_buttons[MIC_SENS_MINUS].Xpos, _buttons[MIC_SENS_MINUS].Ypos, _buttons[MIC_SENS_MINUS].Width, _buttons[MIC_SENS_MINUS].Height, "-");
	GraphObjectDraw(&_buttons[MIC_SENS_PLUS], 0, true, true, _tft, _touch);
	_tft->drawStringCentered(_buttons[MIC_SENS_PLUS].Xpos, _buttons[MIC_SENS_PLUS].Ypos, _buttons[MIC_SENS_PLUS].Width, _buttons[MIC_SENS_PLUS].Height, "+");

	printf("...completed\n\n");
	return;

}

float DmTft24_363_Display::getSpeed() {

	return _settingsVariables[SPEED];

}

float DmTft24_363_Display::getMicSens() {

	return _settingsVariables[MIC_SENS];

}
