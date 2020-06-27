/*############################################################################################
 LightLCD
 Lightweight library for various LCD

 Author: Daniele Colanardi
 License: BSD, see LICENSE file

 Inspired by Adafruit_PCD8544 library.
############################################################################################*/

#include "LightLCD.h"
#include "glcdfont.h"

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

// Variable width() font table
//extern const uint8_t font[];

// ############################################################################################

LightLCD::LightLCD() {
    cursor_y = 0;
    cursor_x = 0;
    
    text_prop = { 1, 1, 1 };
}

// ############################################################################################

void LightLCD::resetLimits(uint8_t whole = true) {
    limits.x0 = 0;
    limits.x1 = whole ? width() - 1 : 0;
    limits.y0 = 0;
    limits.y1 = whole ? height() - 1 : 0;
}

void LightLCD::expandLimits(uint8_t x, uint8_t y) {
         if (x < limits.x0) limits.x0 = x;
    else if (x > limits.x1) limits.x1 = x;

         if (y < limits.y0) limits.y0 = y;
    else if (y > limits.y1) limits.y1 = y;
}

// ############################################################################################

void LightLCD::drawVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color) {
    for(int i = 0; i < h; i++)
        drawPixel(x, y + i, color);
}
void LightLCD::drawHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color) {
    for(int i = 0; i < w; i++)
        drawPixel(x + i, y, color);
}

void LightLCD::drawLine(uint8_t x0, uint8_t y0,  uint8_t x1, uint8_t y1,  uint8_t color) {
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

void LightLCD::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    drawHLine(x, y, w, color);
    drawHLine(x, y+h-1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x+w-1, y, h, color);
}

void LightLCD::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    for (uint8_t i = x; i < x+w; i++) {
        drawVLine(i, y, h, color); 
    }
}

// ############################################################################################

uint8_t LightLCD::drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color, uint8_t transparentBg, uint8_t size) {
    uint8_t len;
    uint8_t line;
    
    int8_t col;
    
    len = pgm_read_byte(font_width + c);
    
    
    if (x >= width()  || y >= height() ||
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
    
    return (len + 1) * size;
}

/* Draw XBitMap Files (*.xbm), exported from GIMP,
 * Uses PROGMEM array directly from *.xbm as this:
 * 
 *  const static uint8_t image[] PROGMEM = { ... };
 * 
*/
void LightLCD::drawXBitmap(uint8_t x, uint8_t y,
                 const uint8_t *bitmap, uint8_t width, uint8_t height,
                 uint8_t color, uint8_t transparentBg) {
    // NOTE: being 8bit long, max width & height is 255.
    uint8_t i, j;
    // Pixel blocks (bytes) for each row of the bitmap
    uint8_t blocksPerRow = (width + 7) / 8;

    uint8_t bit, block = 0;
    int8_t  final_color;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++ ) {
            // The current pixel in the block.
            bit = i % 8;

            // Get the next block from flash only if needed
            if (bit == 0)
                block = pgm_read_byte(bitmap + j * blocksPerRow + i / 8);

            // Decide the color
            final_color = block & (1 << bit) ? color // Foreground pixel
                        : transparentBg      ? -1    // Background pixel, do not draw
                        : !color;                    // Background pixel, draw it

            if(final_color != -1)
                drawPixel(x + i, y + j, final_color);
        }
    }
}

size_t LightLCD::write(uint8_t c) {
    if (c == '\n') {
        cursor_y += text_prop.size * 8;
        cursor_x = 0;
    } else if (c != '\r')  {
        uint8_t c_width = drawChar(cursor_x, cursor_y, c, text_prop.color, text_prop.transparent, text_prop.size);
        
        cursor_x += c_width;
        
        if (cursor_x >= width()) {
            cursor_y += text_prop.size * 8;
            cursor_x = 0;
        }
    }

    return 1;
}

uint8_t LightLCD::getCharWidth(char c) {
    return c > 127 ? 0 : (pgm_read_byte(font_width + c) * text_prop.size);
}

uint8_t LightLCD::getStringWidth(const char* str) {
    if(str == NULL) return 0;
    
    char* ptr = (char*)str;
    uint8_t w = 0;
    
    while(*ptr != 0) { 
        // Add the single char's width + 1 blank column
        w += getCharWidth(*ptr) + text_prop.size;
        
        ptr++;
    }
    
    return w;
}

uint8_t LightLCD::getStringWidth(const __FlashStringHelper* str) {
    if(str == NULL) return 0;
    
    PGM_P ptr = reinterpret_cast<PGM_P>(str);
    uint8_t w = 0;
    uint8_t c = pgm_read_byte(ptr);
    
    while(c != 0) { 
        // Add the single char's width() + 1 blank column
        w += getCharWidth(c) + text_prop.size;
        
        c = pgm_read_byte(ptr++);
    }
    
    return w;
}


// ############################################################################################
uint8_t LightLCD::getCursorX() { return cursor_x; }
uint8_t LightLCD::getCursorY() { return cursor_y; }

void LightLCD::setCursor(uint8_t x, uint8_t y) {
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
        return;

    cursor_x = x;
    cursor_y = y;
}

void LightLCD::setTextColor(uint8_t color, uint8_t transparentBg) {
    if(transparentBg != 0xff)
        //bitWrite(text_prop, 1, transparentBg);
        text_prop.transparent = transparentBg;

    //bitWrite(text_prop, 0, color);
    text_prop.color = color;
}
void LightLCD::setTextSize(uint8_t size) {
    //text_prop = (text_prop & 0x03) | (size << 2);
    text_prop.size = size;
}
