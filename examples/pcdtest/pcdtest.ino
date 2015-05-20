#include <Wire.h>
#include <LightLCD.h>
#include <LightSSD1306.h>

LightSSD1306 lcd = LightSSD1306();

void setup()
{
  lcd.begin();
  //lcd.setContrast(50);
}

void loop()
{
  lcd.clear();
  
  lcd.setCursor(0, 0);
  
  lcd.println("Upper case:");
  lcd.println("ABCDEFGHIJKLM");
  lcd.println("NOPQRSTUVWXYZ");
  
  lcd.update();
  delay (5000);
  
  lcd.clear();
  lcd.println("Lower case:");
  lcd.println("abcdefghijklm");
  lcd.println("nopqrstuvwxyz");
  
  lcd.update();
  delay (5000);
  
  lcd.clear();
  lcd.println("Numbers:");
  lcd.println("0123456789");
  
  lcd.update();
  delay (5000);
  
  lcd.clear();
  lcd.println("Special:");
  lcd.println("!\"#$%&'()*+,-.");
  lcd.println("/:;<=>?@[\\]^_`");
  lcd.println("{|}~");
  
  lcd.update();
  delay (5000);
}