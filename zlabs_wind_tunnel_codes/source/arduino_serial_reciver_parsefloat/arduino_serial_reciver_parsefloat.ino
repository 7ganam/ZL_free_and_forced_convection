


boolean newData = false;
void setup() {
    Serial.begin(9600);
    Serial2.begin(9600);
    Serial.println("<Arduino is ready>");
}

void loop() 
{


   if ( Serial2.available() )
   {

      float tag;
      tag =(float)Serial2.parseFloat(); // the expected input is in the format of four numbers separated by any charachter EX: 145*11*18*19 .. parse float reads one number at a time. the first number is the tag.
      if (abs(tag - 111.11) <= 1.0e-05)
      {
        float kp= Serial2.parseFloat();
        Serial.println(kp);
      }
    }

}
