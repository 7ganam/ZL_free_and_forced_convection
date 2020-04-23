#include <MAX6675.h>
      // the library timeone is use
      #define CS_PIN1 17
      #define CS_PIN2 33
      int time_to_fire=0;
      
      MAX6675 tcouple_heater(CS_PIN2);
      MAX6675 tcouple_free(CS_PIN1);
      
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
}

void loop() {
              float real_temperature = tcouple_free.readTempC();;  //get the real temperature in Celsius degrees
              Serial.println(real_temperature);
//
              float real_temperature2 = tcouple_heater.readTempC();;  //get the real temperature in Celsius degrees
              Serial.println(real_temperature2);
                             Serial.println("++++++++++++++++++++");

}
