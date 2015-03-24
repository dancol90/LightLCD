/*############################################################################################
 LightPCD8544
 Lightweight library for Nokia LCD

 Author: Daniele Colanardi
 License: BSD, see LICENSE file

 Inspired by Adafruit_PCD8544 library.
############################################################################################*/

#include "LightPCD8544.h"
#include "glcdfont.c"


#define swap(a, b) { uint8_t t = a; a = b; b = t; }

// Variable width font table
extern const uint8_t PROGMEM font[];

// ############################################################################################

LightPCD8544::LightPCD8544(uint8_t DC, uint8_t CS) {
    dc = DC;
    cs = CS;

    cursor_y = 0;
    cursor_x = 0;
    
    text_prop = 1;
}

void LightPCD8544::begin(uint8_t contrast) {
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
    if (contrast > 0x7f)
        contrast = 0x7f;

    command(PCD8544_SETVOP | contrast);

    // Return to normal instruction mode
    command(PCD8544_FUNCTIONSET);

    // Set display to not Inverted
    command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

    // Clear the buffer to init it.
    clear();

    // Show a blank screen
    update();
}

// ############################################################################################

void LightPCD8544::command(uint8_t c) {
    // Signal DATA mode
    digitalWrite(dc, LOW);
    // Enable chip
    digitalWrite(cs, LOW);

    SPI.transfer(c);

    // Disable chip
    digitalWrite(cs, HIGH);
}

// ############################################################################################

void LightPCD8544::invertDisplay(uint8_t i) {
    command(PCD8544_DISPLAYCONTROL | (i ? PCD8544_DISPLAYINVERTED : PCD8544_DISPLAYNORMAL));
}

void LightPCD8544::setContrast(uint8_t val) {
    if (val > 0x7f)
        val = 0x7f;
    
    command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
    command(PCD8544_SETVOP | val); 
    command(PCD8544_FUNCTIONSET);
}

// ############################################################################################

void LightPCD8544::resetLimits(uint8_t whole = true) {
    limits.x0 = 0;
    limits.x1 = whole ? LCD_WIDTH : 0;
    limits.y0 = 0;
    limits.y1 = whole ? LCD_HEIGHT : 0;
}

void LightPCD8544::expandLimits(uint8_t x, uint8_t y) {
    if (x <  limits.x0) limits.x0 = x;
    if (x >= limits.x1) limits.x1 = x + 1;

    if (y <  limits.y0) limits.y0 = y;
    if (y >= limits.y1) limits.y1 = y + 1;
}

void LightPCD8544::update(void) {
    if(limits.y0 == limits.y1)
        return;
    
    for(uint8_t y = limits.y0 / 8; y <= (limits.y1 - 1) / 8; y++) {
        command(PCD8544_SETYADDR | y);

        command(PCD8544_SETXADDR | limits.x0);

        digitalWrite(dc, HIGH);

        // Activate chip
        digitalWrite(cs, LOW);

        for(uint8_t x = limits.x0; x < limits.x1; x++)
            SPI.transfer(buffer[(LCD_WIDTH * y) + x]);
            //SPI.transfer(0xFF);
        
        // Disable chip
        digitalWrite(cs, HIGH);

    }

    command(PCD8544_SETYADDR);  // no idea why this is necessary but it is to finish the last byte?

    resetLimits(false);
}

void LightPCD8544::clear(void) {
    memset(buffer, 0, BUFFER_SIZE);
    resetLimits(true);
    
    cursor_y = cursor_x = 0;
}

// ############################################################################################

void LightPCD8544::drawPixel(uint8_t x, uint8_t y, uint8_t color) {
    if ((x < 0) || (x >= LCD_WIDTH) || (y < 0) || (y >= LCD_HEIGHT))
        return;

    //bitWrite(buffer[x + (y / 8) * LCD_WIDTH], y % 8, color);
    if (color) 
        buffer[x + (y / 8)*LCD_WIDTH] |= _BV(y%8);
    else
        buffer[x + (y / 8)*LCD_WIDTH] &= ~_BV(y%8);

    expandLimits(x, y);
}

void LightPCD8544::drawVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color) {
    for(int i = 0; i < h; i++)
        drawPixel(x, y + i, color);
}
void LightPCD8544::drawHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color) {
    for(int i = 0; i < w; i++)
        drawPixel(x + i, y, color);
}

void LightPCD8544::drawLine(uint8_t x0, uint8_t y0,  uint8_t x1, uint8_t y1,  uint8_t color) {
    uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
    
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    uint8_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int8_t err   = dx / 2;
    int8_t ystep = y0 < y1 ? 1 : -1;

    for (; x0 <= x1; x0++) {
        if (steep)
            drawPixel(y0, x0, color);
        else
            drawPixel(x0, y0, color);
        
        err -= dy;
    
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void LightPCD8544::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    drawHLine(x, y, w, color);
    drawHLine(x, y+h-1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x+w-1, y, h, color);
}

void LightPCD8544::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    for (uint8_t i = x; i < x+w; i++) {
        drawVLine(i, y, h, color); 
    }
}

// ############################################################################################


uint8_t LightPCD8544::drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color, uint8_t transparentBg, uint8_t size) {
    uint8_t len;
    uint8_t line;
    
    int8_t col;
    
    len = pgm_read_byte(font_width + c);
    
    
    if (x >= LCD_WIDTH  || y >= LCD_HEIGHT ||
       (x + len * size - 1) < 0          ||
       (y + 8   * size - 1) < 0 )
        return 0;

    for (int8_t i = 0; i < len + 1; i++ ) {
        if(i == len)
            line = 0;
        else
            line = pgm_read_byte(font + 5*c + i);
    
        for (int8_t j = 1; j < 8; j++) {
            // Choose color: is bg==color, do not draw background pixels (the ones with bit 0)
            if (line & (1 << j))
                col = color;
            else
                col = transparentBg ? -1 : !color;
            

            // col = -1 means do not draw.
            if(col != -1) {
                if (size == 1) // default size
                    drawPixel(x + i, y + j - 1, col);
                else  // big size
                    fillRect(x + i*size, y + (j - 1)*size, size, size, col);
            }
            
        }
    }
    
    return len + 1;
}

size_t LightPCD8544::write(uint8_t c) {
    if (c == '\n') {
        cursor_y += 1 * 8;
        cursor_x = 0;
    } else if (c != '\r')  {
        uint8_t c_width = drawChar(cursor_x, cursor_y, c, bitRead(text_prop, 0), bitRead(text_prop, 1));
        
        cursor_x += 1 * c_width;
        
        if (cursor_x >= LCD_WIDTH) {
            cursor_y += 1 * 8;
            cursor_x = 0;
        }
    }

    return 1;
}

uint8_t LightPCD8544::getStringWidth(const char* str) {
    if(str == NULL) return 0;
    
    char* ptr = (char*)str;
    uint8_t w = 0;
    
    while(*ptr != 0) { 
        // Add the single char's width + 1 blank column
        w += pgm_read_byte(font_width + *ptr) + 1;
        
        ptr++;
    }
    
    return w;
}

uint8_t LightPCD8544::getStringWidth(const __FlashStringHelper* str) {
    if(str == NULL) return 0;
    
    PGM_P ptr = reinterpret_cast<PGM_P>(str);
    uint8_t w = 0;
    uint8_t c = pgm_read_byte(ptr);
    
    while(c != 0) { 
        // Add the single char's width + 1 blank column
        w += pgm_read_byte(font_width + c) + 1;
        
        c = pgm_read_byte(ptr++);
    }
    
    return w;
}

// ############################################################################################

void LightPCD8544::setCursor(uint8_t x, uint8_t y) {
    if ((x < 0) || (x >= LCD_WIDTH) || (y < 0) || (y >= LCD_HEIGHT))
        return;

    cursor_x = x;
    cursor_y = y;
}

void LightPCD8544::setTextColor(uint8_t color, uint8_t transparentBg) {
    if(transparentBg != 0xff)
        bitWrite(text_prop, 1, transparentBg);

    bitWrite(text_prop, 0, color);
}
void LightPCD8544::setTextSize(uint8_t size) {
    text_prop = (text_prop & 0x03) | (size << 2);
}
