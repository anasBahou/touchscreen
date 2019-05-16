/*
 * Button.cpp
 *
 *  Created on: 9 mai 2019
 *      Author: sixtron-training
 */

#include "mbed.h"
#include "Button.h"


//If DISPLAY_INVERT is defined as 1 then the display will be inverted from its native orientation
#define DISPLAY_INVERT      1

#define MAX_GO_STRING       30
#define SPACE_ASCII         0x20
#define FONT_WIDTH          8

//MenuSettings_t MenuSettings;
char GoTmpString[MAX_GO_STRING];


static int BmpWidth;
static int BmpHeight;
static uint8_t BmpImageoffset;

static bool BmpReadHeader( uint8_t *thisBmp );
static uint16_t Read16( uint8_t *src );
static uint32_t Read32( uint8_t *src );
static void DrawBmpFromFlash( uint8_t *thisBmp, int x, int y, DmTftIli9341* Tft );


GraphObjectStatus_t GraphObjectDraw( GraphObject_t* goObject, uint8_t* source, \
                                     bool doFill, bool activeTouch, DmTftIli9341* Tft, DmTouch* Touch )
{
    GraphObjectStatus_t status = GO_STATUS_NOERR;
    uint8_t maxChar;

    if( goObject == NULL )
    {
        return GO_STATUS_BAD_ARG;
    }
    if( goObject->Xpos + goObject->Width > SCREEN_WIDTH )
    {
        return GO_STATUS_BAD_COORD;
    }
    if( goObject->Ypos + goObject->Height > SCREEN_HEIGHT )
    {
        return GO_STATUS_BAD_COORD;
    }
    switch( goObject->Type )
    {
        case GO_TEXT:
            if( source == NULL )
            {
                status = GO_STATUS_BAD_ARG;
            }
            else
            {
                uint8_t i = 0;
                uint8_t j = 0;
                // max character in the object string
                maxChar = goObject->Width / FONT_WIDTH;
                Tft->setTextColor( goObject->BackColor, goObject->FrontColor );
                for( i = 0; i < maxChar; i++)
                {
                    if( *source != 0 )
                    {
                        Tft->drawChar( goObject->Xpos + j, goObject->Ypos, \
                                      ( char )*( source++ ), false );
                        j += FONT_WIDTH;
                    }
                    else
                    {
                        Tft->drawChar( goObject->Xpos + ( FONT_WIDTH * i ), \
                                      goObject->Ypos, SPACE_ASCII, false);
                    }
                }
                goObject->TouchActive = activeTouch;
            }
            break;

        case GO_RECTANGLE:
            Tft->drawRectangle( goObject->Xpos, goObject->Ypos, goObject->Xpos + \
                               goObject->Width - 1, goObject->Ypos + \
                               goObject->Height - 1, goObject->FrontColor );
            if( doFill )
            {
                Tft->fillRectangle( goObject->Xpos + 1, goObject->Ypos + 1, \
                                   goObject->Xpos + goObject->Width - 2, \
                                   goObject->Ypos + goObject->Height - 2, \
                                   goObject->FillColor );
            }
            goObject->TouchActive = activeTouch;
            break;

        case GO_CIRCLE:
            Tft->drawCircle( goObject->Xpos, goObject->Ypos, \
                            ( goObject->Height < goObject->Width ) ? \
                              ( goObject->Height / 2 ) : ( goObject->Width / 2 ), \
                            goObject->FrontColor );
            if( doFill )
            {
                Tft->fillCircle( goObject->Xpos, goObject->Ypos, \
                                ( goObject->Height < goObject->Width ) ? \
                                  ( goObject->Height / 2 ) - 1 : ( goObject->Width / 2 ) - 1, \
                                goObject->FillColor );
            }
            goObject->TouchActive = activeTouch;
            break;

        case GO_TRIANGLE:
            status = GO_STATUS_BAD_ARG;
            break;

        case GO_IMAGE:
            if( source == NULL )
            {
                status = GO_STATUS_BAD_ARG;
            }
            else
            {
                if( BmpReadHeader( goObject->Source ) )
                {
                    DrawBmpFromFlash( goObject->Source, goObject->Xpos, \
                                      goObject->Ypos, Tft);
                }
                else
                {
                    // draw a red rectangle with a line through, to show error
                    Tft->drawRectangle( goObject->Xpos, goObject->Ypos, \
                                       goObject->Xpos + goObject->Width - 1, \
                                       goObject->Ypos + goObject->Height - 1, \
                                       OBJECT_ERROR );
                    Tft->drawLine( goObject->Xpos, goObject->Ypos, goObject->Xpos + \
                                  goObject->Width - 1, goObject->Ypos + \
                                  goObject->Height - 1, OBJECT_ERROR );
                }
                goObject->TouchActive = activeTouch;
            }
            break;

        case GO_LINE:
            Tft->drawLine( goObject->Xpos, goObject->Ypos, goObject->Xpos + \
                                  goObject->Width - 1, goObject->Ypos + \
                                  goObject->Height - 1, goObject->FrontColor );
            break;

        default:
            status = GO_STATUS_BAD_ARG;
    }
    return status;
}

GraphObjectStatus_t GraphObjectClear( GraphObject_t* goObject, bool doFill, DmTftIli9341* Tft, DmTouch* Touch )
{
    GraphObjectStatus_t status = GO_STATUS_NOERR;
    uint8_t maxChar;

    if( goObject == NULL )
    {
        return GO_STATUS_BAD_ARG;
    }
    if( goObject->Xpos + goObject->Width > SCREEN_WIDTH )
    {
        return GO_STATUS_BAD_COORD;
    }
    if( goObject->Ypos + goObject->Height > SCREEN_HEIGHT )
    {
        return GO_STATUS_BAD_COORD;
    }
    switch( goObject->Type )
    {
        case GO_TEXT:
            // max character in the object string
            maxChar = goObject->Width / FONT_WIDTH;
            GoTmpString[maxChar] = NULL;
            Tft->setTextColor( goObject->BackColor, goObject->FrontColor );
            Tft->drawString( goObject->Xpos, goObject->Ypos, GoTmpString );
            GoTmpString[maxChar] = SPACE_ASCII;
            goObject->TouchActive = false;
            break;

        case GO_RECTANGLE:
        case GO_IMAGE:
            if( doFill )
            {
                Tft->fillRectangle( goObject->Xpos, goObject->Ypos, \
                                   goObject->Xpos + goObject->Width - 1, \
                                   goObject->Ypos + goObject->Height - 1, \
                                   goObject->BackColor );
            }
            else
            {
                Tft->drawRectangle( goObject->Xpos, goObject->Ypos, goObject->Xpos + \
                                   goObject->Width - 1, goObject->Ypos + \
                                   goObject->Height - 1, goObject->BackColor );
            }
            goObject->TouchActive = false;
            break;

        case GO_CIRCLE:
            if( doFill )
            {
                Tft->fillCircle( goObject->Xpos, goObject->Ypos, \
                                ( goObject->Height < goObject->Width ) ? \
                                  ( goObject->Height / 2 ) : ( goObject->Width / 2 ), \
                                goObject->BackColor );
            }
            else
            {
                Tft->drawCircle( goObject->Xpos, goObject->Ypos, \
                                ( goObject->Height < goObject->Width ) ? \
                                  ( goObject->Height / 2 ) : ( goObject->Width / 2 ), \
                                goObject->BackColor );
            }
            goObject->TouchActive = false;
            break;

        case GO_TRIANGLE:
            status = GO_STATUS_BAD_ARG;
            goObject->TouchActive = false;
            break;

        case GO_LINE:
            Tft->drawLine( goObject->Xpos, goObject->Ypos, goObject->Xpos + \
                                  goObject->Width - 1, goObject->Ypos + \
                                  goObject->Height - 1, goObject->BackColor );
            goObject->TouchActive = false;
            break;

        default:
            status = GO_STATUS_BAD_ARG;
    }
    return status;
}

void DisplayDriverDrawLogo( uint8_t *thisBmp, uint8_t xPos, uint8_t yPos, DmTftIli9341* Tft )
{
    if( BmpReadHeader( thisBmp ) )
    {
        DrawBmpFromFlash( thisBmp, xPos, yPos, Tft );
    }
}

GraphObjectStatus_t GraphObjectTouched( GraphObject_t* objects, \
                                        uint8_t objectsCount, \
                                        uint8_t* touchedObject, DmTftIli9341* Tft, DmTouch* Touch)
{
    uint8_t objScan;
    uint16_t x, y = 0;
    bool touched = false;
    GraphObjectStatus_t status = GO_STATUS_BAD_COORD;

    if( Touch->isTouched( ) )
    {
        Touch->readTouchData( x, y, touched );

        if( touched == true )
        {
            for( objScan = 0; objScan < objectsCount; objScan++)
            {
                if( objects[objScan].TouchActive == true )
                {
                    if( ( y >= objects[objScan].Ypos ) && ( y <= ( objects[objScan].Ypos + objects[objScan].Height - 1 ) ) )
                    {
                        if( ( x >= objects[objScan].Xpos ) && ( x <= ( objects[objScan].Xpos + objects[objScan].Width - 1 ) ) )
                        {
                            *touchedObject = objects[objScan].Id;
                            status = GO_STATUS_NOERR;
                            break;      // return the first object match and no scan of other following objects
                        }
                    }
                }
            }
        }
    }

    return status;
}

static bool BmpReadHeader( uint8_t *thisBmp )
{
    uint16_t pos = 0;

    Read16( thisBmp );
    if( Read16( thisBmp ) != 0x4D42 )
    { // read magic byte
        return false;
    }
    pos += 2;

    // read file size
    pos += 4;
    pos += 4; // Skip creator bytes
    BmpImageoffset = Read32( thisBmp + pos );
    pos += 4;
    // read DIB header
    pos +=4;
    BmpWidth = Read32( thisBmp + pos );
    pos += 4;
    BmpHeight = Read32( thisBmp + pos );
    pos += 4;
    if( Read16( thisBmp + pos ) != 1 )
    {
        // number of color planes must be 1
        return false;
    }
    pos += 2;
    pos += 2;
    if( Read16( thisBmp + pos ) != 0 )
    {
        return false; // compression not supported!
    }
    pos += 2; // Should really be 2??
    return true;
}

// LITTLE ENDIAN!
static uint16_t Read16( uint8_t *src )
{
    uint16_t d;
    uint8_t b;
    b = *src;
    d = *( src + 1 );
    d <<= 8;
    d |= b;
    return d;
}

// LITTLE ENDIAN!
static uint32_t Read32( uint8_t *src )
{
    uint32_t d;
    uint16_t b;

    b = Read16( src );
    d = Read16( src + 2 );
    d <<= 16;
    d |= b;
    return d;
}

static void DrawBmpFromFlash( uint8_t *thisBmp, int xPos, int yPos, DmTftIli9341* Tft )
{
    uint16_t pos = BmpImageoffset;
    uint16_t p;  // pixel
    uint8_t g;
    uint8_t b;
    int i, j; // line, column

    for( i = BmpHeight; i > 0; i-- )
    {
        for( j = 0; j < BmpWidth; j++ )
        {
            b = *( thisBmp + pos++ );
            g = *( thisBmp + pos++ );
            p = *( thisBmp + pos++ );

            p >>= 3;
            p <<= 6;

            g >>= 2;
            p |= g;
            p <<= 5;

            b >>= 3;
            p |= b;

            // write out the 16 bits of color
            Tft->setPixel( j + xPos, i + yPos, p );
        }
        pos += 1;
    }
}

