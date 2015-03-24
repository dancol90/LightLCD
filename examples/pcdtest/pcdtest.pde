#include <SPI.h>
#include <LightPCD8544.h>

LightPCD8544 lcd = LightPCD8544(8, 9);

void setup()
{
  lcd.begin();
  lcd.setContrast(50);
}

void loop()
{
  lcd.clearDisplay();
  
  lcd.setCursor(0, 0);
  
  lcd.println("Upper case:");
  lcd.println("ABCDEFGHIJKLM");
  lcd.println("NOPQRSTUVWXYZ");
  
  lcd.display();
  delay (5000);
  
  lcd.clearDisplay();
  lcd.println("Lower case:");
  lcd.println("abcdefghijklm");
  lcd.println("nopqrstuvwxyz");
  
  lcd.display();
  delay (5000);
  
  lcd.clearDisplay();
  lcd.println("Numbers:");
  lcd.println("0123456789");
  
  lcd.display();
  delay (5000);
  
  lcd.clearDisplay();
  lcd.println("Special:");
  lcd.println("!\"#$%&'()*+,-.");
  lcd.println("/:;<=>?@[\\]^_`");
  lcd.println("{|}~");
  
  lcd.display();
  delay (5000);
}