//this code is for arduino mega only as it uses Serial2
float right_tag=111.11;

boolean newData = false;
void setup() 
{
    Serial.begin(9600);
    pinMode(13,OUTPUT);
}

void loop() 
{
   if ( Serial.available() )
   {

      float tag;
      tag =(float)Serial.parseFloat(); // the expected input is in the format of four numbers separated by any charachter EX: 145*11*18*19 .. parse float reads one number at a time. the first number is the tag.
      if (abs(tag - right_tag) <= 1.0e-05)
      {
        float reading = Serial.parseFloat();
        digitalWrite(13,HIGH);
        delay(reading);
        digitalWrite(13,LOW);
        delay(reading);

      }
    }

}
