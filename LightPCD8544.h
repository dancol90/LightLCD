/*############################################################################################
 LightLCD
 Lightweight library for various LCD

 Author: Daniele Colanardi
 License: BSD, see LICENSE file

 Inspired by Adafruit_PCD8544 library.
############################################################################################*/

#ifndef _LIGHT_PCD8544_H
#define _LIGHT_PCD8544_H

#include "LightLCD.h"
#include <SPI.h>

#define BLACK 1
#define WHITE 0

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

class LightPCD8544 : public LightLCD {
    public:
        LightPCD8544(uint8_t DC, uint8_t CS) : dc(DC), cs(CS) {}

        void begin() {
            // set pin directions
            pinMode(dc, OUTPUT);
            pinMode(cs, OUTPUT);
            
            //pinMode(SS, OUTPUT);

            SPI.begin();
            SPI.setClockDivider(SPI_CLOCK_DIV16);

            // Enter extended instruction mode
            command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

            // LCD bias select (4 is optimal?)
            command(PCD8544_SETBIAS | 0x4);

            // Set contrast
            //if (contrast > 0x7f)
            //    contrast = 0x7f;

            //command(PCD8544_SETVOP | contrast);

            // Return to normal instruction mode
            command(PCD8544_FUNCTIONSET);

            // Set display to not Inverted
            command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

            // Clear the buffer to init it.
            clear();

            // Show a blank screen
            update();
        }

        void setContrast(uint8_t val) {
            if (val > 0x7f)
                val = 0x7f;
            
            command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
            command(PCD8544_SETVOP | val); 
            command(PCD8544_FUNCTIONSET);
        }

        void clear() {
            memset(buffer, 0, 84 * 48 / 8);
            resetLimits(true);
            
            cursor_y = cursor_x = 0;
        }

        void update() {
            if(limits.y0 == limits.y1)
                return;
            
            for(uint8_t y = limits.y0 / 8; y <= (limits.y1 - 1) / 8; y++) {
                command(PCD8544_SETYADDR | y);

                command(PCD8544_SETXADDR | limits.x0);

                digitalWrite(dc, HIGH);

                // Activate chip
                digitalWrite(cs, LOW);

                for(uint8_t x = limits.x0; x < limits.x1; x++)
                    SPI.transfer(buffer[(width() * y) + x]);
                    //SPI.transfer(0xFF);
                
                // Disable chip
                digitalWrite(cs, HIGH);

            }

            command(PCD8544_SETYADDR);  // no idea why this is necessary but it is to finish the last byte?

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

        void invertDisplay(uint8_t i) {
            command(PCD8544_DISPLAYCONTROL | (i ? PCD8544_DISPLAYINVERTED : PCD8544_DISPLAYNORMAL));
        }

        int width()  { return 84; }
        int height() { return 48; }

    protected:
        uint8_t dc, cs;

        uint8_t buffer[84 * 48 / 8];

        void command(uint8_t c) {
            // Signal DATA mode
            digitalWrite(dc, LOW);
            // Enable chip
            digitalWrite(cs, LOW);

            SPI.transfer(c);

            // Disable chip
            digitalWrite(cs, HIGH);
        }
};

#endif
