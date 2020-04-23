
bool zero_cross_detected = false;
int zero_cross_pin = 2;
int firing_pin = 3;
#include <TimerOne.h>
int time_to_fire=0;
void ISR_1();
void setup() 
{

  attachInterrupt(digitalPinToInterrupt(zero_cross_pin), ISR_1,  FALLING);
  pinMode(firing_pin,OUTPUT);        
  Serial.begin(9600);
}

void loop() 
{
    time_to_fire = map(500,0,1024,8200,20);
}




//This is the interruption routine
//----------------------------------------------

void ISR_1()
{
    zero_cross_detected=true;  
    Timer1.initialize(time_to_fire);     //set a timer for time_to_fire microseconeds 
    Timer1.attachInterrupt(open_pulse_timer_ISR);                                 
}

void open_pulse_timer_ISR()
{
    digitalWrite(firing_pin, HIGH); 
    Timer1.initialize(1000);         
    Timer1.attachInterrupt(close_pulse_timer_ISR);  
}

void close_pulse_timer_ISR()
{
    digitalWrite(firing_pin, LOW); 
    Timer1.stop();
}
