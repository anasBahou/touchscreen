/*
 * Button.h
 *
 *  Created on: 9 mai 2019
 *      Author: sixtron-training
 */
#ifndef TFT_DISPLAY_DRIVER_H
#define TFT_DISPLAY_DRIVER_H


#include "DmTftBase.h"
#include "DmTftIli9341.h"
#include "DmTouch.h"


#define SCREEN_WIDTH                240
#define SCREEN_HEIGHT               320

#define TEXT_COLOR                  WHITE
#define BACK_COLOR                  BLACK
#define PAGE_COLOR                  CYAN
#define OBJECT_ERROR                RED
#define BUTTON_BORDER               BLUE
#define MENU_TEXT                   GREEN
#define TEXT_VALUE                  YELLOW
#define CIRCLE_FCOLOR               GRAY1


typedef enum
{
    GO_STATUS_NOERR,
    GO_STATUS_BAD_COORD,
    GO_STATUS_BAD_ARG,
    GO_STATUS_BAD_CONTEXT,
}GraphObjectStatus_t;

typedef enum
{
    GO_TEXT,
    GO_RECTANGLE,
    GO_CIRCLE,
    GO_TRIANGLE,
    GO_IMAGE,
    GO_LINE,
}GraphObjectType_t;

typedef struct
{
    uint8_t             Id;
    GraphObjectType_t   Type;
    uint16_t            Xpos;
    uint16_t            Ypos;
    uint16_t            Height;
    uint16_t            Width;
    uint16_t            LineWidth;
    uint16_t            BackColor;
    uint16_t            FrontColor;
    bool                DoFill;
    uint16_t            FillColor;
    uint8_t*            Source;
    bool                TouchActive;
}GraphObject_t;


/*!
 * \brief Initialses the hardware and variables associated with the display.
 */
 void DisplayDriverInit( void );

/*!
 * \brief Calibrates the touch screen.
 */
void DisplayDriverCalibrate( void );

/*!
 * \brief Draws a graphical object.
 *
 * \param [in]  *goObject     Object to draw.
 * \param [in]  *source       If object is a text : *source is the text to print
 *                            If object is a image: *source is the image
 *                            For the other type of goObject, *source is ignored
 * \param [in]  doFill        Indicate if the goObject (only for GO_RECTANGLE,
 *                            GO_CIRCLE & GO_TRIANGLE) should be filled or not.
 * \param [in]  activeTouch   Indicate if the coordinates of the goObject can be
 *                            used for touchscreen or not.
 * \retval      status        GO_STATUS_NOERR if ok or,
 *                            GO_STATUS_BAD_COORD if the object go out of screen
 */
GraphObjectStatus_t GraphObjectDraw( GraphObject_t* goObject, uint8_t* source, \
                                     bool doFill, bool activeTouch, DmTftIli9341* Tft, DmTouch* Touch );

/*!
 * \brief Clear a graphical object.
 *
 * \param [in]  *goObject     Object to clear.
 * \param [in]  doFill        Indicate if the goObject (only for GO_RECTANGLE,
 *                            GO_CIRCLE & GO_TRIANGLE) should be filled or not.
 * \retval      status        GO_STATUS_NOERR if ok or,
 *                            GO_STATUS_BAD_COORD if the object go out of screen
 */
GraphObjectStatus_t GraphObjectClear( GraphObject_t* goObject, bool doFill, DmTftIli9341* Tft, DmTouch* Touch );

/*!
 * \brief Get the first object that have the touched coordinate (if activateTouch
 *        of the object is TRUE).
 *
 * \param [in]  *objects      Tab of graphical objects.
 * \param [in]  objectsCount  Indicate the numbre of goObject in the tab.
 * \param [out] touchedObject Put the ID of the first touched and activated
 *                            object of the tab.
 * \retval      status        GO_STATUS_NOERR if ok.
 */
GraphObjectStatus_t GraphObjectTouched( GraphObject_t* objects, \
                                        uint8_t objectsCount, \
                                        uint8_t* touchedObject, DmTftIli9341* Tft, DmTouch* Touch);

/*!
 * \brief Draws a logo on the display.
 *
 * \param [in]  *thisBmp      The file to be printed onto the display.
 * \param [in]  xPos          Position across the display in pixels for the top
 *                            right corner of the logo starting at the left edge
 *                            of display.
 * \param [in]  yPos          Position across the display in pixels for the top
 *                            right corner of the logo starting at the top of
 *                            the display.
 */
void DisplayDriverDrawLogo( uint8_t *thisBmp, uint8_t xPos, uint8_t yPos, DmTftIli9341* Tft );

#endif //TFT_DISPLAY_DRIVER_H

