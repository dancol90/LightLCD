/*############################################################################################
 LightLCD
 Lightweight library for various LCD

 Author: Daniele Colanardi
 License: BSD, see LICENSE file

 Inspired by Adafruit_PCD8544 library.
############################################################################################*/

#ifndef _LIGHT_SSD1306_H
#define _LIGHT_SSD1306_H

#include "LightLCD.h"
#include <Wire.h>

#define SSD1306
 
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

class LightSSD1306 : public LightLCD {
    public:
        LightSSD1306() {}

        void begin() {
            Wire.begin();

            Wire.setClock(400000);

            byte command_sequence[] = {
                SSD1306_DISPLAYOFF,
                SSD1306_SETDISPLAYCLOCKDIV, 0x80,
                SSD1306_SETMULTIPLEX,       0x3F,
                SSD1306_SETDISPLAYOFFSET,   0x00, // No offset
                SSD1306_SETSTARTLINE | 0x0, 
                SSD1306_CHARGEPUMP,         0x14, // Force using internal high voltage
                SSD1306_MEMORYMODE,         0x00, // Horizontal Addressing Mode (same as PCD8544)
                SSD1306_SEGREMAP | 0x1,
                SSD1306_COMSCANDEC,                    
                SSD1306_SETCOMPINS,         0x12,
                SSD1306_SETCONTRAST,        0xCF,
                SSD1306_SETPRECHARGE,       0xF1,
                SSD1306_SETVCOMDETECT,      0x40,
                SSD1306_DISPLAYALLON_RESUME,
                SSD1306_NORMALDISPLAY,
                SSD1306_DISPLAYON
            };

            commandList(command_sequence, 25);

            clear();
            update();
        }

        void setContrast(uint8_t val) {};

        void clear() {
            memset(buffer, 0, width() * height() / 8);
            resetLimits(true);
            
            cursor_y = cursor_x = 0;
        }

        void update() {
            if(limits.y0 == limits.y1)
                return;
    
            byte command_list[] = {
                SSD1306_COLUMNADDR,
                    limits.x0,  // Which column to start from
                    limits.x1,  // To which
                SSD1306_PAGEADDR,
                    (byte)(limits.y0 / 8),
                    (byte)(limits.y1 / 8)
            };

            commandList(command_list, 6);

            byte i = 0;

            for(byte y = limits.y0 / 8; y <= limits.y1 / 8; y++) {   
                for(byte x = limits.x0; x <= limits.x1; x++) {
                    if (i == 0) {
                        Wire.endTransmission();

                        Wire.beginTransmission(0x3C);
                        Wire.write(0x40);
                    }

                    Wire.write(buffer[(width() * y) + x]);

                    i = ++i % 16;
                }
            }

            Wire.endTransmission();

            resetLimits(false);
        }

        void drawPixel(uint8_t x, uint8_t y, uint8_t color) {
            if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
                return;

            //bitWrite(buffer[x + (y / 8) * width()], y % 8, color);
            if (color) 
                buffer[x + (y / 8)*width()] |= _BV(y%8);
            else
                buffer[x + (y / 8)*width()] &= ~_BV(y%8);

            expandLimits(x, y);
        }

        void invertDisplay(bool invert) {
            command(invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
        }

        int width()  { return 128; }
        int height() { return 64; }

    private:
        uint8_t buffer[128 * 64 / 8];

        void command(uint8_t cmd) {
            Wire.beginTransmission(0x3C);
            Wire.write(0x00);
            Wire.write(cmd);
            Wire.endTransmission();
        }

        void commandList(byte* commands, int count) {
           for (byte i = 0; i < count; i++)
                command(commands[i]);
        }
};

#endif

