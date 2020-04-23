
/*TRIAC control with potentiometer; author: ELECTRONOOBS 
 * Subscribe: http://www.youtube.com/c/ELECTRONOOBS
 * Tutorial: http://www.ELECTRONOOBS.com/eng_circuitos_tut20.php
 * Thank you
*/
int detectado = 0;
int valor=0;
int last_CH1_state = 0;

void ISRe();
void setup() 
{

  attachInterrupt(digitalPinToInterrupt(2), ISRe,  FALLING);
  pinMode(3,OUTPUT);        
  Serial.begin(9600);
}

void loop() {
   //Read the value of the pot and map it from 10 to 10.000 us. AC frequency is 50Hz, so period is 20ms. We want to control the power
   //of each half period, so the maximum is 10ms or 10.000us. In my case I've maped it up to 7.200us since 10.000 was too much
   
   valor = map(analogRead(A0),0,1024,7200,10);
    if (detectado)
    {
//      delayMicroseconds(8000); //This delay controls the power
      digitalWrite(3,HIGH);
      delayMicroseconds(100);
      digitalWrite(3,LOW);
      detectado=0;
    } 
}




//This is the interruption routine
//----------------------------------------------

void ISRe()
{
    detectado=1;                                      
}
