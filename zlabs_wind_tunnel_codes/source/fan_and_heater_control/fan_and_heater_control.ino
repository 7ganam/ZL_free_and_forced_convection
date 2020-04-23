
// FAN CONTROL VARIABLES------------------------------------------------------------------------------------------
      #define  fan 5
      #include <PID_v1.h>
      //Define PID controller variables
       float Velocity_m_per_s=0;
      //--------------------------------------------------------------------------------------------------------------------------------------------
      double  Kp =.5;
      double  Ki = 4;
      double  Kd = .5;
      double  fan_setpoint =8;
      //--------------------------------------------------------------------------------------------------------------------------------------------
      //Specify the links and initial tuning parameters
      double  Input, Output; //Define Variables we'll be connecting to
      PID myPID(&Input, &Output, &fan_setpoint, Kp, Ki, Kd, DIRECT);
      
      byte Velocity_hi_byte  ;        // stores received byte
      byte Velocity_low_byte  ;        // stores received byte
      byte zero_byte  ;        // stores received byte
      byte checksum_byte ;        // stores received byte

      byte q ;        
      byte w ;        
      byte e ;        
      byte r;        
      
      byte a ;        
      byte s;        
      byte d ;        
      byte f;   

// END OF FAN CONTROL VARIABLES------------------------------------------------------------------------------------------


// HEATER CONTROL VARIABLES-------------------------------------------------------------------------------------------------
      int heater_setpoint =40;
      #include <MAX6675.h>
      #include <TimerOne.h>
      // the library timeone is use
//      #define CS_PIN1 31
      #define CS_PIN2 33
      int time_to_fire=0;
      
      MAX6675 tcouple_heater(CS_PIN2);
//      MAX6675 tcouple_free(CS_PIN1);
      
      #include <Wire.h> 
      
      
//      void ISR_1();
      
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
      int kp = 200;   int ki= 10;  int ki2= 10; int kd =3;
      int PID_p = 0;    int PID_i = 0;    int PID_d = 0;

// END OF HEATER CONTROL VARIABLES-------------------------------------------------------------------------------------------------





void setup() 
{

// FAN CONTROL SETUP-------------------------------------------------------------------------
             pinMode(fan,OUTPUT);
              q = 1; w = 0; e = 0; r= q^w^e;       
              a = 12; s = 0; d = 0; f= q^w^e;        
            
              Serial3.begin(19200);   // serial port 3
              Serial.begin(9600);
          
              myPID.SetOutputLimits(0, 255);
              myPID.SetMode(AUTOMATIC);
              // reset the sensor
                  Serial3.write(a);
                  Serial3.write(s);
                  Serial3.write(d);
                  Serial3.write(f);
                  Velocity_hi_byte  = Serial3.read();
                  Serial.println(" ************* ");
              //empty the serial buffer        
                  while(Serial3.available() > 0) 
                  {
                     char t = Serial3.read();
                  }
// END OF FAN CONTROL SETUP-------------------------------------------------------------------------

// HEATER CONTOL SETUP-------------------------------------------------------------------------
              attachInterrupt(digitalPinToInterrupt(zero_cross_pin), ISR_1,  FALLING);
              pinMode (firing_pin,OUTPUT); 
              pinMode (zero_cross_pin,INPUT); 
              Serial.begin(9600);
// END OF HEATER CONTOL SETUP-------------------------------------------------------------------------

}


void loop() 
{    
 // FAN CONTROL----------------------------------------------------------------------------
    
              Velocity_m_per_s= GetVelocity ();
              Input = Velocity_m_per_s*2;
              myPID.Compute();
              analogWrite(fan,Output);

 // END OF FAN CONTROL --------------------------------------------------------------------
 
 //HEATER PID ---------------------------------------------------------------------
              currentMillis = millis();          
              if(currentMillis - previousMillis >= temp_read_Delay)
              {
                      previousMillis += temp_read_Delay;              //Increase the previous time for next loop
                      real_temperature = tcouple_heater.readTempC();;  //get the real temperature in Celsius degrees
                      PID_error = heater_setpoint - real_temperature;        //Calculate the pid ERROR
                      
        
        
                      if(PID_error < 0)                          
                      {ki2 = ki/10;}
                      else if(PID_error >= 0 )                            
                      {ki2 = ki;}
        
                      if(PID_error > 10)                              //integral constant will only affect errors below 10ºC             
                      {PID_i = 0;}
                      
                      PID_p = kp * PID_error;                         //Calculate the P value
                      PID_i = PID_i + (ki2 * PID_error);               //Calculate the I value
                      timePrev = Time;                    // the previous time is stored before the actual time read
                      Time = millis();                    // actual time read
                      elapsedTime = (Time - timePrev) / 1000;   
                      PID_d = kd*((PID_error - previous_error)/elapsedTime);  //Calculate the D value
                      PID_value = PID_p + PID_i + PID_d;                      //Calculate total PID value
                      if(PID_value < 0)
                      {
                            PID_value = 0;       
                      }
                      if(PID_error < 0)
                      {
                            PID_value = 0;       
                      }
                      if(PID_value > maximum_firing_delay)
                      {     
                            PID_value = maximum_firing_delay-20;   
                      }
                      Serial.println("----------------------------");
                      Serial.print("temp= ");
                      Serial.print(real_temperature);
                      Serial.print(", fan= ");
                      Serial.println(Velocity_m_per_s);
                      Serial.println(PID_i);
                      Serial.println(Output);
                      Serial.println("----------------------------");
                      previous_error = PID_error; //Remember to store the previous error.
            
              }
               time_to_fire=maximum_firing_delay - PID_value;
         //END OF HEATER PID ---------------------------------------------------------------------


 
}
//End of void loop


float GetVelocity ()
{
  unsigned int result;
  byte buf [4];

  Serial3.write(1);
  Serial3.write(0);
  Serial3.write(0);
  Serial3.write(1); // the checksum of any byte XOR'd with two 0s will always be the that byte

  Serial3.readBytes(buf, 4);
  
 
  result = buf[0]; // first byte read is MSB
  result = result << 8; // shift the MSB up;
  result += buf[1]; // add in the LSB
 
  return ((float) result) / 1000.0;
}

//
void ISR_1()
{
    Timer1.initialize(time_to_fire);     //set a timer for time_to_fire microseconeds 
    Timer1.attachInterrupt(open_pulse_timer_ISR);                                 
}
//
void open_pulse_timer_ISR()
{
    digitalWrite(firing_pin, HIGH); 
    Timer1.initialize(500);         
    Timer1.attachInterrupt(close_pulse_timer_ISR);  
}
//
void close_pulse_timer_ISR()
{
    digitalWrite(firing_pin, LOW); 
    Timer1.stop();
}
