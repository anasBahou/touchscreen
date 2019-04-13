/******************************************************************************
 * Includes
 *****************************************************************************/
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
#define NUM_BUTTONS  (sizeof(menu_Items)/sizeof(menu_Items[0]))
#define NUM_SETTINGS_VARIABLES  2
#define BUTTONS_PER_LINE   1
#define BUTTONS_NUM_LINES  3


/******************************************************************************
 * Local variables
 *****************************************************************************/

/******************************************************************************
 * Global variables
 *****************************************************************************/
FlashIAP my_flash;
DmTftIli9341 tft(SPI1_CS, DIO2, SPI1_MOSI, SPI1_MISO, SPI1_SCK);
DmTouch touch(DmTouch::DM_TFT24_363, SPI1_MOSI, SPI1_MISO, SPI1_SCK);

const char* menu_Items[] = {"BATTERY STATUS", "SOUND DIRECTION", "SETTINGS"};
Button* buttons[NUM_BUTTONS];

float battery_level = 100;
int sound_angle = 160;

uint32_t settings_address = 0x080FF000;

uint32_t back_from_settings = 0;

uint16_t settings_variables[NUM_SETTINGS_VARIABLES+1];


/******************************************************************************
 * Global functions
 *****************************************************************************/

/*
 * Read variables on flash memory
 */
void read_variables() {

	// read blocs from flash memory
	my_flash.read(settings_variables , settings_address, my_flash.get_page_size());
//	my_flash.read(settings_variables+1 , mic_sens_adress, 1);

	printf("speed : %d\n", settings_variables[0]);
	printf("mic_sens : %d\n", settings_variables[1]);
}


/*
 * Save variables on flash memory
 */
void save_variables() {

	// erase blocks on flash memory
	my_flash.erase(settings_address, my_flash.get_sector_size(settings_address));
	// program blocks
	my_flash.program(settings_variables , settings_address, my_flash.get_page_size());


//  uint32_t ret1 = my_flash.get_page_size();
//  uint32_t ret2 = my_flash.get_sector_size(settings_address);
//	printf("program : %d\n", ret);
//	printf("get_page_size : %d\n", ret1);
//	printf("get_sector_size : %d\n", ret2);
//	my_flash.program(settings_variables+1 , mic_sens_adress, 1);

	  printf("Settings saved \n");
	  printf("Saved speed : %d\n", settings_variables[0]);
	  printf("Saved mic_sens : %d\n", settings_variables[1]);
}


/*
 * Prototype handleClick_Settings
 */
void handleClick_Settings(uint32_t arg=0);


/*
 * Display the home page
 */
void home_page(uint32_t arg=0) {

	// variables
    bool down = false;
    uint16_t x = 0;
    uint16_t y = 0;

	// variables settings button
	uint16_t x_set = 0;
	uint16_t y_set = 0;
	uint16_t height_set = 50;
	uint16_t width_set = 100;

	// variables display battery
	uint16_t x0_rect = 200;
	uint16_t y0_rect = 20;
	uint16_t x1_rect = 230;
	uint16_t y1_rect = 30;
	uint16_t rect_height = y1_rect - y0_rect;

	// screen size
	uint16_t w = tft.width();
	uint16_t h = tft.height();

    // initialization
    tft.init();
    touch.init();

	// create settings button
	const char* settings_menu_Items[] = {"settings"};
	Button* settings_button[1];
	settings_button[0]= new Button(settings_menu_Items[0], x_set, y_set, width_set, height_set);
	settings_button[0]->setAction(handleClick_Settings, settings_menu_Items[2][0]); // go to settings page


    // case back from the settings window ==> need to save the settings
	if (arg == back_from_settings){
	  save_variables();
	}

	// display
	settings_button[0]->draw(&tft);

    // get the sound direction
    float sound_angle_rad = (float)sound_angle * 3.14/180;
    uint16_t circle_radius = 80;
    uint16_t x_sound, y_sound;

    x_sound = w/2 + circle_radius*cos(sound_angle_rad);
    y_sound = h/2 - circle_radius*sin(sound_angle_rad);

    char sound_angle_str[4];

    // battery
    uint8_t battery_level_int = (uint8_t)battery_level;
	uint16_t level_to_pixel = (uint16_t)(((100-battery_level)/100)*rect_height); // conversion to an integer
	char battery_level_str[6];
	const char percent[] = "%";
	sprintf(battery_level_str, "%d%s", battery_level_int, percent); // to convert battery_level to a string of a format "70.0 %"

    // display direction
    tft.drawCircle(w/2, h/2, circle_radius, WHITE);
    tft.drawPoint(w/2, h/2);
    tft.drawLine(w/2, h/2, x_sound, y_sound, RED);

    // drawing the Battery level
	tft.drawString(200, 0, battery_level_str);
   	tft.drawRectangle(x0_rect, y0_rect, x1_rect, y1_rect, WHITE);
   	tft.drawVerticalLine(x0_rect-1, y0_rect+4, 2, WHITE);
   	if (battery_level > 74) {
   		tft.fillRectangle(x0_rect+1, y0_rect+1 + level_to_pixel, x1_rect-1, y1_rect-1, GREEN);
   	}
   	else if (49 < battery_level < 75) {
   		tft.fillRectangle(x0_rect+1, y0_rect+1 + level_to_pixel, x1_rect-1, y1_rect-1, YELLOW);

   	}
   	else if (24 < battery_level < 50) {
   		tft.fillRectangle(x0_rect+1, y0_rect+1 + level_to_pixel, x1_rect-1, y1_rect-1, ORANGE);
   	}
   	else if (10 < battery_level < 25) {
   		tft.fillRectangle(x0_rect+1, y0_rect+1 + level_to_pixel, x1_rect-1, y1_rect-1, RED);
   	}
   	else {
   		tft.fillRectangle(x0_rect+1, y0_rect+1 + level_to_pixel, x1_rect-1, y1_rect-1, BLACK);
   	}



	// examine if user touch the screen
	while(true) {
		touch.readTouchData(x, y, down);
		if (settings_button[0]->handle(x_set, y_set, down)) {
			settings_button[0]->draw(&tft);
		}
	}
}


/*
 *  Display state of the battery
 */
void handleClick_Battery(uint32_t arg=0) {

	// variables touch test
	bool down = false;
	uint16_t x = 0;
	uint16_t y = 0;

	// variables back button
	uint16_t x1 = 0;
	uint16_t y1 = 0;
	uint16_t size1 = 50;

	// variables display battery
	uint16_t x0_rect = 60;
	uint16_t y0_rect = 80;
	uint16_t x1_rect = tft.width() - x0_rect;
	uint16_t y1_rect = tft.height() - y0_rect;
	uint16_t rect_height = y1_rect - y0_rect;

	uint16_t level_to_pixel = (uint16_t)(((100-battery_level)/100)*rect_height); // conversion to an integer
	char battery_level_str[6];
	const char percent[] = "%";
	sprintf(battery_level_str, "%.1f %s", battery_level, percent); // to convert battery_level to a string of a format "70.0 %"

	// create back button
	const char* back_menu_Items[] = {"back"};
	Button* back_button[1];
	back_button[0]= new Button(back_menu_Items[0], x1, y1, size1, size1);
	back_button[0]->setAction(home_page, back_menu_Items[0][0]); // back to the home page

	// initialization
	tft.init();
	touch.init();

	// drawing the Battery level
	tft.drawString(x0_rect+40, y0_rect-20, battery_level_str);
	tft.drawRectangle(x0_rect, y0_rect, x1_rect, y1_rect, BLUE);
	tft.fillRectangle(x0_rect+1, y0_rect+1 + level_to_pixel, x1_rect-1, y1_rect-1, RED); // +1 and -1 to avoid the rectangle borders

	// display button
	for (uint i = 0; i < NUM_BUTTONS; i++) {
		back_button[i]->draw(&tft);
	}

	// examine if user touch the screen
	while(true) {
	touch.readTouchData(x, y, down);
	for (int i = 0; i < 1; i++) {
	  if (back_button[i]->handle(x, y, down)) {
		  back_button[i]->draw(&tft);
	  }
	}
  }
}


/*
 * Display the direction of the sound
 */
void handleClick_Sound(uint32_t arg=0) {

	// variables touch test
	bool down = false;
	uint16_t x = 0;
	uint16_t y = 0;

	// variables back button
	uint16_t x1 = 0;
	uint16_t y1 = 0;
	uint16_t size1 = 50;

	// screen size
	uint16_t w = tft.width();
	uint16_t h = tft.height();


	// create the back button
	const char* back_menu_Items[] = {"back"};
	Button* back_button[1];
    back_button[0]= new Button(back_menu_Items[0], x1, y1, size1, size1);
    back_button[0]->setAction(home_page, back_menu_Items[0][0]); // back to the home page

    // initialization
    tft.init();
    touch.init();

    // get the sound direction
    float sound_angle_rad = (float)sound_angle * 3.14/180;
    uint16_t circle_radius = 80;
    uint16_t x_sound, y_sound;

    x_sound = w/2 + circle_radius*cos(sound_angle_rad);
    y_sound = h/2 - circle_radius*sin(sound_angle_rad);

    char sound_angle_str[4];
    const char degre[] = "degree";
    sprintf(sound_angle_str, "%d %s", sound_angle, degre);

    // display information
    tft.drawCircle(w/2, h/2, circle_radius, GREEN);
    tft.drawPoint(w/2, h/2);
    tft.drawString(w/2 - 40, h/2 - circle_radius - 20, sound_angle_str);
    tft.drawLine(w/2, h/2, x_sound, y_sound, BLUE);

    // display buttons
    for (uint i = 0; i < NUM_BUTTONS; i++) {
    	back_button[i]->draw(&tft);
    }

    // examine if user touch the screen
    while(true) {
    	touch.readTouchData(x, y, down);
    	for (int i = 0; i < 1; i++) {
    		if (back_button[i]->handle(x, y, down)) {
    			back_button[i]->draw(&tft);
    		}
    	}
    }
}


/*
 * Change settings variables
 */
void change_setting(uint32_t arg) {

	// varibales
	uint16_t x1 = tft.width()/2 ;
    uint16_t y1 = 50 + MARGIN;
    uint16_t size1 = 50;
    //char speed_str[4];
    //char mic_sens_str[4];

    // handling the speed variation
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

	// handling the mic_sens variation
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


/*
 * Display setting menu
 */
void handleClick_Settings(uint32_t arg) {

	//variables
	bool down = false;
	uint16_t x = 0;
	uint16_t y = 0;

	// parameters for back button
	uint16_t x1 = 0;
	uint16_t y1 = 0;
	uint16_t size1 = 50;

	// create a new set of buttons
	const int NUM_SETTINGS_BUTTONS = 5;
	const char* settings_menu_Items[] = {"back", "-", "+", "-", "+"};
	Button* settings_buttons[NUM_SETTINGS_BUTTONS];

	// config the back button
	settings_buttons[0]= new Button(settings_menu_Items[0], x1, y1, size1, size1);
	settings_buttons[0]->setAction(home_page, back_from_settings); // back to the home page
	// config the speed button "-" for the speed
	settings_buttons[1]= new Button(settings_menu_Items[1], x1+size1+4*MARGIN, y1+size1+MARGIN, size1, size1);
	settings_buttons[1]->setAction(change_setting, 0); // "0" refers to "-" for the speed button
	// config the speed button "+" for the speed
	settings_buttons[2]= new Button(settings_menu_Items[2], tft.width()-size1-MARGIN, y1+size1+MARGIN, size1, size1);
	settings_buttons[2]->setAction(change_setting, 1); // "1" refers to "+" for the speed button
	// config the mic_sens button "-" for the mic_sens
	settings_buttons[3]= new Button(settings_menu_Items[1], x1+size1+4*MARGIN, y1+2*(size1+MARGIN), size1, size1);
	settings_buttons[3]->setAction(change_setting, 2); // "0" refers to "-" for the mic_sens button
	// config the mic_sens button "+" for the mic_sens
	settings_buttons[4]= new Button(settings_menu_Items[2], tft.width()-size1-MARGIN, y1+2*(size1+MARGIN), size1, size1);
	settings_buttons[4]->setAction(change_setting, 3); // "1" refers to "+" for the mic_sens button

	// initialization
	tft.init();
	touch.init();

	// display
	tft.drawString(0, size1 + MARGIN, "Speed :");
	tft.drawNumber(tft.width()/2 , size1 + MARGIN, settings_variables[0], 3, false); //  settings_variables[0] <=> speed variable
	tft.drawString(0, 2*(size1 + MARGIN), "Mic_Sens:");
	tft.drawNumber(tft.width()/2 , 2*(size1+MARGIN), settings_variables[1], 3, false); //  settings_variables[1] <=> mic_sens variable

	// display buttons
	for (int i = 0; i < NUM_SETTINGS_BUTTONS; i++) {
		settings_buttons[i]->draw(&tft);
	}

	// examine if user touch the screen
	while(true) {
		touch.readTouchData(x, y, down);
		for (int i = 0; i < NUM_SETTINGS_BUTTONS; i++) {
			if (settings_buttons[i]->handle(x, y, down)) {
				settings_buttons[i]->draw(&tft);
			}
		}
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
  //uint16_t yoff = 5;

  if (my_flash.init() != 0) {
	  printf("Error initializing flash\n"); // init the flash memory
  }

  read_variables(); // read the settings variables

  for (uint i = 0; i < NUM_BUTTONS;i++) {
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
