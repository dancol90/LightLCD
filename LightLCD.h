/*############################################################################################
 LightLCD
 Lightweight library for various LCD

 Author: Daniele Colanardi
 License: BSD, see LICENSE file

 Inspired by Adafruit_PCD8544 library.
############################################################################################*/

#ifndef _LIGHT_LCD_H
#define _LIGHT_LCD_H

#include "Arduino.h"

#define BLACK 1
#define WHITE 0

struct Limits {
    uint8_t x0, y0;
    uint8_t x1, y1;
};

class LightLCD : public Print {
    public:
        LightLCD();

        virtual void    begin() = 0;

        virtual void    clear() = 0;
        virtual void    update() = 0;

        virtual void    drawPixel(uint8_t x, uint8_t y, uint8_t color) = 0;

        void    drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
        void    drawVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color);
        void    drawHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);
        void    drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
        void    fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);

        uint8_t drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color = 1, uint8_t transparentBg = 1, uint8_t size = 1);
        void    drawXBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, uint8_t color, uint8_t transparentBg=1);

        uint8_t getCursorX();
        uint8_t getCursorY();

        void    setCursor(uint8_t x, uint8_t y);
        void    setTextColor(uint8_t color, uint8_t transparentBg = 0xff);
        void    setTextSize(uint8_t size);

        uint8_t getCharWidth(char c);
        uint8_t getStringWidth(const char* str);
        uint8_t getStringWidth(const __FlashStringHelper* str);

        size_t write(uint8_t);

        virtual int width() = 0;
        virtual int height() = 0;
        
    protected:
        Limits limits;

        uint8_t cursor_x, cursor_y;
        //uint8_t text_prop;

        struct TextProp {
            uint8_t color : 1;
            uint8_t transparent : 1;
            uint8_t size    : 6;
        } text_prop;

        void resetLimits(uint8_t whole);
        void expandLimits(uint8_t x, uint8_t y);
};

#endif
