#include "mbed.h"
#include "DmTftIli9341.h"
#include "DmTouch.h"
#include "Button.h"
#include "PinNames.h"

#include <stdio.h>
#include <math.h>


/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/
#define MARGIN  5
#define RESULT_MARGIN_X  (MARGIN*3)
#define RESULT_MARGIN_Y  (MARGIN*3)

const char* menu_Items[] = {"BATTERY STATUS", "SOUND DIRECTION", "SETTINGS"};
#define NUM_BUTTONS  (sizeof(menu_Items)/sizeof(menu_Items[0]))
#define BUTTONS_PER_LINE   1
#define BUTTONS_NUM_LINES  3


/******************************************************************************
 * Local variables
 *****************************************************************************/


/******************************************************************************
 * Global variables
 *****************************************************************************/

/*********  TFT DISPLAY INIT *********/
DmTftIli9341 tft(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);

/*********   TOUCH PANEL INIT  *********/
DmTouch touch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_MISO, SPI1_SCK);

/*********   BOUTONS INIT  *********/
Button* buttons[NUM_BUTTONS];

int speed = 50; // Speed value


/******************************************************************************
 * Local functions
 *****************************************************************************/
void home_page(uint32_t arg=0) {

	/*Display the home page*/

	// Variables
	bool down = false; // touch test
	uint16_t x = 0;
	uint16_t y = 0;

	// Initialisation
	tft.init();
	touch.init();

	// Display buttons
	for (int i = 0; i < NUM_BUTTONS; i++) {
		buttons[i]->draw(&tft);
	}

	// Examine if user touch the screen
	while(true) {
		touch.readTouchData(x, y, down);
		for (int i = 0; i < NUM_BUTTONS; i++) {
			if (buttons[i]->handle(x, y, down)) {
				buttons[i]->draw(&tft);
			}
		}
		wait(0.02);
	}
}


void handleClick_Battery(uint32_t arg=0) {

	/*Display state of the battery*/

	// Variables touch test
	bool down = false;
	uint16_t x = 0;
	uint16_t y = 0;

	// Variables back boutton
	uint16_t x_back = 0;
	uint16_t y_back = 0;
	uint16_t size_back = 50;

	// Variables display battery
	uint16_t x0_rect = 60;
	uint16_t y0_rect = 80;
	uint16_t x1_rect = tft.width() - x0_rect;
	uint16_t y1_rect = tft.height() - y0_rect;
	uint16_t rect_height = y1_rect - y0_rect;

	// Initialisation
	tft.init();
	touch.init();

	float battery_level = 100; // to be able to do the division by 100
	uint16_t level_to_pixel = (uint16_t)(((100-battery_level)/100)*rect_height); // conversion to an integer
	char battery_level_str[6];
	const char percent[] = "%";
	sprintf(battery_level_str, "%.1f %s", battery_level, percent); // to convert battery_level to a string of a format "70.0 %"

	// drawing the Battery level
	tft.drawString(x0_rect+40, y0_rect-20, battery_level_str);
	tft.drawRectangle(x0_rect, y0_rect, x1_rect, y1_rect, BLUE);
	tft.fillRectangle(x0_rect+1, y0_rect+1 + level_to_pixel, x1_rect-1, y1_rect-1, RED); // +1 and -1 to avoid the rectangle borders

	// back boutton
	const char* back_menu_Items[] = {"back"};
	Button* back_button[1];
	back_button[0]= new Button(back_menu_Items[0], x_back, y1, size_back, size_back);
	back_button[0]->setAction(home_page, back_menu_Items[0][0]); // back to the home page

	// Display buttons
	for (int i = 0; i < NUM_BUTTONS; i++) {
		back_button[i]->draw(&tft);
	}

	// Examine if user touch the screen
	while(true) {
		touch.readTouchData(x, y, down);
		for (int i = 0; i < 1; i++) {
			if (back_button[i]->handle(x, y, down)) {
				back_button[i]->draw(&tft);
		}
	}
	wait(0.02);
  }
}


void handleClick_Sound(uint32_t arg=0) {

	/*Display the direction of the sound*/

	// Varaibles touch test
	bool down = false;
	uint16_t x = 0;
	uint16_t y = 0;

	// Variables back boutton
	uint16_t x_back = 0;
	uint16_t y_back = 0;
	uint16_t size_back = 50;

	// Size screen
	uint16_t w = tft.width();
	uint16_t h = tft.height();

	// Initialisation
	  tft.init();
	  touch.init();

	// Create the back button
	const char* back_menu_Items[] = {"back"};
	Button* back_button[1];
	back_button[0]= new Button(back_menu_Items[0], x_back, y1, size_back, size_back);
	back_button[0]->setAction(home_page, back_menu_Items[0][0]); // back to the home page

	// Get the sound direction
	int sound_angle = 160;
	float sound_angle_rad = (float)sound_angle * 3.14/180;
	uint16_t circle_radius = 80;
	uint16_t x_sound, y_sound;

	x_sound = w/2 + circle_radius*cos(sound_angle_rad);
	y_sound = h/2 - circle_radius*sin(sound_angle_rad);

	char sound_angle_str[4];
	const char degre[] = "degree";
	sprintf(sound_angle_str, "%d %s", sound_angle, degre);

	// What to show
	tft.drawCircle(w/2, h/2, circle_radius, GREEN);
	tft.drawPoint(w/2, h/2);
	tft.drawString(w/2 - 40, h/2 - circle_radius - 20, sound_angle_str);
	tft.drawLine(w/2, h/2, x_sound, y_sound, BLUE);

	// Display buttons
	for (int i = 0; i < NUM_BUTTONS; i++) {
		back_button[i]->draw(&tft);
	}

	//Examine if user touch the screen
	while(true) {
		touch.readTouchData(x, y, down);
		for (int i = 0; i < 1; i++) {
			if (back_button[i]->handle(x, y, down)) {
				back_button[i]->draw(&tft);
			}
		}
		wait(0.02);
	}
}


void change_speed(uint32_t arg) {

	/*Modify the speed of the robot*/

	// Variables
	uint16_t x1 = tft.width()/2 - 10;
    uint16_t y1 = 50 + MARGIN;
    uint16_t size1 = 50;
    char speed_str[4];

	if (arg==0){ // case "-" pressed
		tft.fillRectangle(x1, y1, x1+size1, y1+size1, BLACK);
		speed--;
		sprintf(speed_str, "%d", speed);
		tft.drawString(x1, y1, speed_str);
	}
	if (arg==1){ // case "+" pressed
		tft.fillRectangle(x1, y1, x1+size1, y1+size1, BLACK);
		speed++;
		sprintf(speed_str, "%d", speed);
		tft.drawString(x1, y1, speed_str);
	}


}


void handleClick_Settings(uint32_t arg=0) {

	/*Go to Setting menu*/

	// Variables
	bool down = false;
	uint16_t x = 0;
	uint16_t y = 0;

	// Parameters for back button
	uint16_t x_back = 0;
	uint16_t y_back = 0;
	uint16_t size_back = 50;


	// Create a new set of buttons
	const int NUM_SETTINGS_BUTTONS = 3;
	const char* settings_menu_Items[] = {"back", "-", "+"};
	Button* settings_buttons[NUM_SETTINGS_BUTTONS];

	// config the back button
	settings_buttons[0]= new Button(settings_menu_Items[0], x_back, y1, size_back, size_back);
	settings_buttons[0]->setAction(home_page, settings_menu_Items[0][0]); // back to the home page

	// config the speed button "-"
	settings_buttons[1]= new Button(settings_menu_Items[1], x_back+size_back+MARGIN, y1+size_back+MARGIN, size_back, size_back);
	settings_buttons[1]->setAction(change_speed, 0); // "0" refers to "-"

	// config the speed button "-"
	settings_buttons[2]= new Button(settings_menu_Items[2], tft.width()-size_back-MARGIN, y1+size_back+MARGIN, size_back, size_back);
	settings_buttons[2]->setAction(change_speed, 1); // "1" refers to "+"

	// Initialisation
	tft.init();
	touch.init();

	// Display "Speed"
	tft.drawString(0, size1 + MARGIN, "Speed :");
	tft.drawNumber(tft.width()/2 - 10, 50 + MARGIN, speed, 3, false);

	// Display butons
	for (int i = 0; i < NUM_SETTINGS_BUTTONS; i++) {
	  settings_buttons[i]->draw(&tft);
	}

	// Examine if user touche the screen
	while(true) {
		touch.readTouchData(x, y, down);
		for (int i = 0; i < NUM_SETTINGS_BUTTONS; i++) {
			if (settings_buttons[i]->handle(x, y, down)) {
				settings_buttons[i]->draw(&tft);
			}
		}
		wait(0.02);
	}
}


/******************************************************************************
 * Main
 *****************************************************************************/

int main() {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t h = tft.height();
  uint16_t size = (h - MARGIN*BUTTONS_NUM_LINES) / BUTTONS_NUM_LINES;

  for (int i = 0; i < NUM_BUTTONS;i++) {
    x = MARGIN + (size + MARGIN) * (i % BUTTONS_PER_LINE);
    y = size*i + MARGIN;
    buttons[i] = new Button(menu_Items[i], x, y, tft.width(), size);
  }

  // setting the action for each button
  buttons[0]->setAction(handleClick_Battery, menu_Items[0][0]);
  buttons[1]->setAction(handleClick_Sound, menu_Items[1][0]);
  buttons[2]->setAction(handleClick_Settings, menu_Items[2][0]);

  // show the home screen
  home_page();

}


