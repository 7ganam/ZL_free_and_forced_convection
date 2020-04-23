
bool zero_cross_detected = false;
int zero_cross_pin = 2;
int firing_pin = 3;
int valor=0;
void ISR_1();
void setup() 
{

  attachInterrupt(digitalPinToInterrupt(zero_cross_pin), ISR_1,  FALLING);
  pinMode(firing_pin,OUTPUT);        
  Serial.begin(9600);
}

void loop() 
{
    valor = map(analogRead(A0),0,1024,8200,0);
    if (zero_cross_detected)
    {
      delayMicroseconds(valor); //This delay controls the power
      Serial.println(valor);
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
