/*############################################################################################
 LightPCD8544
 Lightweight library for Nokia LCD

 Author: Daniele Colanardi
 License: BSD, see LICENSE file

 Inspired by Adafruit_PCD8544 library.
############################################################################################*/

#ifndef _LIGHT_PCD8544_H
#define _LIGHT_PCD8544_H

#include "Arduino.h"
#include <SPI.h>

#define BLACK 1
#define WHITE 0

#define LCD_WIDTH 84
#define LCD_HEIGHT 48

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

#define BUFFER_SIZE LCD_WIDTH * LCD_HEIGHT / 8

class LightPCD8544 : public Print {
    public:
        LightPCD8544(uint8_t DC, uint8_t CS);

        void    begin(uint8_t contrast = 40);

        void    setContrast(uint8_t val);
        void    clear();
        void    update();

        void    drawPixel(uint8_t x, uint8_t y, uint8_t color);

        void    invertDisplay(uint8_t i);

        void    drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
        void    drawVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color);
        void    drawHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);
        void    drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
        void    fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);


        uint8_t drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color = 1, uint8_t transparentBg = 1, uint8_t size = 1);

        void    setCursor(uint8_t x, uint8_t y);
        void    setTextColor(uint8_t color, uint8_t transparentBg = 0xff);
        void    setTextSize(uint8_t size);

        uint8_t getCharWidth(char c);
        uint8_t getStringWidth(const char* str);
        uint8_t getStringWidth(const __FlashStringHelper* str);

        virtual size_t write(uint8_t);


    private:
        uint8_t dc, cs;
        struct {
            uint8_t x0, y0;
            uint8_t x1, y1;
        } limits;

        uint8_t cursor_x, cursor_y;
        uint8_t text_prop;

        uint8_t buffer[BUFFER_SIZE];

        void command(uint8_t c);

        void resetLimits(uint8_t whole);
        void expandLimits(uint8_t x, uint8_t y);

};

#endif

