
#include <LiquidCrystal.h> 

LiquidCrystal lcd(A8, A7, A5, A4, A3, A2); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 
void setup()
{ 
       Serial.begin(115200);
       lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display } 
}
void loop() 
{
 if ( Serial.available() )
   {
      String Input;
      Input =Serial.readString(); // read the incoming data as string
      lcd.print(Input); // Prints "Arduino" on the LCD 
   }
}
