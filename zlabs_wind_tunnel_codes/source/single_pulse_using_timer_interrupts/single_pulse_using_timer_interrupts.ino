//Using Timer Interrupts with the Arduino
#include <TimerOne.h>
const int firing_pin=3;
void setup()
{
pinMode(firing_pin, OUTPUT);
Timer1.initialize(2000);     //microseconds 
Timer1.attachInterrupt(open_pulse_timer_ISR);  

}
void loop()
{

}
//Timer interrupt function

void open_pulse_timer_ISR()
{
    digitalWrite(firing_pin, HIGH); //Toggle LED State
    Timer1.initialize(500);     //Set a timer of length 1000000//microseconds (1 second)
    Timer1.attachInterrupt(close_pulse_timer_ISR);  //Runs "blinky" on each //timmer interrupt
}

void close_pulse_timer_ISR()
{
    digitalWrite(firing_pin, LOW); //Toggle LED State
    Timer1.stop();
}
