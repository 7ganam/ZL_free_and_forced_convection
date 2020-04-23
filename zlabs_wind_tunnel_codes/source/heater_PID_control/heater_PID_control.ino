
int setpoint =30;

#include <MAX6675.h>
#include <TimerOne.h>
// the library timeone is use
#define CS_PIN1 31
#define CS_PIN2 33
int time_to_fire=0;

MAX6675 tcouple_heater(CS_PIN2);
MAX6675 tcouple_free(CS_PIN2);

#include <Wire.h> 


void ISR_1();

//Inputs and outputs
int firing_pin = 3;
int zero_cross_pin = 2;


//Start a MAX6675 communication with the selected pins


//Variables
int last_CH1_state = 0;
bool zero_cross_detected = false;
int firing_delay = 8200;

//////////////////////////////////////////////////////
int maximum_firing_delay = 8200;
/*Later in the code you will se that the maximum delay after the zero detection
 * is 8200. Why? Well, we know that the 220V AC voltage has a frequency of around 50-60HZ so
 * the period is between 20ms and 16ms, depending on the country. We control the firing
 * delay each half period so each 10ms or 8 ms. To amke sure we wont pass thsoe 10ms, I've made tests
 * and the 8.2ms was a good value. Measure your frequency and chande that value later */
//////////////////////////////////////////////////////
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;
int temp_read_Delay = 500;
int real_temperature = 0;



//PID variables
float PID_error = 0;
float previous_error = 0;
float elapsedTime, Time, timePrev;
int PID_value = 0;
//PID constants
int kp = 400;   int ki= 5;   int kd =3;
int PID_p = 0;    int PID_i = 0;    int PID_d = 0;






void setup() 
{
    attachInterrupt(digitalPinToInterrupt(zero_cross_pin), ISR_1,  FALLING);
    pinMode (firing_pin,OUTPUT); 
    pinMode (zero_cross_pin,INPUT); 
    Serial.begin(9600);

}


void loop() 
{    
  currentMillis = millis();           //Save the value of time before the loop

   /*  We create this if so we will read the temperature and change values each "temp_read_Delay"
    *  value. Change that value above iv you want. The MAX6675 read is slow. Tha will affect the
    *  PID control. I've tried reading the temp each 100ms but it didn't work. With 500ms worked ok.*/
  if(currentMillis - previousMillis >= temp_read_Delay)
  {
          previousMillis += temp_read_Delay;              //Increase the previous time for next loop
          real_temperature = tcouple_heater.readTempC();;  //get the real temperature in Celsius degrees
          PID_error = setpoint - real_temperature;        //Calculate the pid ERROR
          
          if(PID_error > 10)                              //integral constant will only affect errors below 30ºC             
          {PID_i = 0;}
          
          PID_p = kp * PID_error;                         //Calculate the P value
          PID_i = PID_i + (ki * PID_error);               //Calculate the I value
          timePrev = Time;                    // the previous time is stored before the actual time read
          Time = millis();                    // actual time read
          elapsedTime = (Time - timePrev) / 1000;   
          PID_d = kd*((PID_error - previous_error)/elapsedTime);  //Calculate the D value
          PID_value = PID_p + PID_i + PID_d;                      //Calculate total PID value
          if(PID_value < 0)
          {
            PID_value = 0;       
          }
          if(PID_error <= 0)
          {
            PID_value = 0;       
          }
          if(PID_value > maximum_firing_delay)
          {     
            PID_value = maximum_firing_delay-20;   
          }
          Serial.print("temp= ");
          Serial.print(real_temperature);
          Serial.print(", err= ");
          Serial.print(PID_error);
          Serial.print(", PID= ");
          Serial.println(PID_value);
          Serial.println(time_to_fire);
          Serial.println("----------------------------");
          previous_error = PID_error; //Remember to store the previous error.
//          PID_value = 4000;                      //Calculate total PID value

  }
//
//
//          if (PID_value >0)
          {
            time_to_fire=maximum_firing_delay - PID_value;
//            time_to_fire=8200;

          }
//          else
//          {
//              time_to_fire=maximum_firing_delay ;
//          }
          
 
}
//End of void loop


void ISR_1()
{
    zero_cross_detected=true;  
    Timer1.initialize(time_to_fire);     //set a timer for time_to_fire microseconeds 
    Timer1.attachInterrupt(open_pulse_timer_ISR);                                 
}

void open_pulse_timer_ISR()
{
    digitalWrite(firing_pin, HIGH); 

        Timer1.initialize(500);         
        Timer1.attachInterrupt(close_pulse_timer_ISR);  

}

void close_pulse_timer_ISR()
{
    digitalWrite(firing_pin, LOW); 
    Timer1.stop();
}
