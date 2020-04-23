
bool zero_cross_detected = false;
int zero_cross_pin = 2;
int firing_pin = 3;
int valor=0;
void ISR_1();
float reading=8200;

float right_tag=111.11;

void setup() 
{

  attachInterrupt(digitalPinToInterrupt(zero_cross_pin), ISR_1,  FALLING);
  pinMode(firing_pin,OUTPUT);        
  Serial.begin(9600);
}

void loop() 
{
   if ( Serial.available() )
   {
          float tag;
          tag =(float)Serial.parseFloat(); 
          if (abs(tag - right_tag) <= 1.0e-05)
          {
               reading = Serial.parseFloat();
          }
    }
    
    valor = (int)reading ;
    if (zero_cross_detected)
    {
      delayMicroseconds(valor); //This delay controls the power
      digitalWrite(firing_pin,HIGH);
      delayMicroseconds(100);
      digitalWrite(firing_pin,LOW);
      zero_cross_detected=false;
    } 
}




//This is the interruption routine
//----------------------------------------------

void ISR_1()
{
    zero_cross_detected=true;                                      
}
