// General CONTROL VARIABLES--------------------------------------------------------------------------------------
 int control_method=0;

//----------------------------------------------------------------------------------------------------------------
// FAN CONTROL VARIABLES------------------------------------------------------------------------------------------
      #define  fan 5
      #include <PID_v1.h>
      //Define PID controller variables
       float Velocity_m_per_s=0;
       float fan_Power = 0;
      //--------------------------------------------------------------------------------------------------------------------
      double  Kp =.5;
      double  Ki = 4;
      double  Kd = .5;
      double  fan_setpoint =0;
      //--------------------------------------------------------------------------------------------------------------------
      //Specify the links and initial tuning parameters
      double  Input, Output; //Define Variables we'll be connecting to
      PID myPID(&Input, &Output, &fan_setpoint, Kp, Ki, Kd, DIRECT);
      
      byte Velocity_hi_byte  ;        // stores received byte
      byte Velocity_low_byte  ;        // stores received byte
      byte zero_byte  ;        // stores received byte
      byte checksum_byte ;        // stores received byte

      byte q ;   byte w ;    byte e ;    byte r;        
      byte a ;   byte s;     byte d ;    byte f;  

// END OF FAN CONTROL VARIABLES----------------------------------------------------------------------------------------


// HEATER CONTROL VARIABLES----------------------------------------------------------------------------------------------
      int heater_setpoint =0;
      bool allow_heater = 1;
      #include <MAX6675.h>
      #include <TimerOne.h>
      // the library timeone is use
      #define CS_PIN1 17
      #define CS_PIN2 33
      int time_to_fire=8200;
      
      MAX6675 tcouple_heater(CS_PIN2);
      MAX6675 tcouple_free(CS_PIN1);
      int heater_temperature = 0;
      float heater_power =0;
      int prop1_temprature = 0;
      #include <Wire.h> 
      
      
//      void ISR_1();
      
      //Inputs and outputs
      int firing_pin = 3;
      int zero_cross_pin = 2;
      
      
      //Start a MAX6675 communication with the selected pins
      
      
      //Variables
      int last_CH1_state = 0;
      bool zero_cross_detected = false;
      bool allow_fire_pulse = false;
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
      
      
      
      //PID variables
      float PID_error = 0;
      float previous_error = 0;
      float elapsedTime, Time, timePrev;
      int PID_value = 0;
      //PID constants
      int kp = 300;   int ki= 2;  int ki2= .5; int kd =7;
      int PID_p = 0;    int PID_i = 0;    int PID_d = 0;
      int Cooling_threshold;
// END OF HEATER CONTROL VARIABLES--------------------------------------------------------------------------------------------

// DEBUGING VARIABLES---------------------------------------------------------------------------------------------------------
      #include <LiquidCrystal.h> // includes the LiquidCrystal Library 
      LiquidCrystal lcd(41, 42, 44, 45, 46, 47); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 
      #define LED 13
      int count=0;
//END OF DEBUGING VARIABLES---------------------------------------------------------------------------------------------------

// FSM VARIABLES-------------------------------------------------------------------------------------------
      enum STATE {Connection_confirmation, Waiting_start , Waiting_variables,  Cooling , PID_running ,Peek_for_serial_input2, Halt }state ;
      float start_time_micros; // this timer starts onece only when the encoder readings start flowing out, it's used to send the time of each reading to ML
      float switch_to_peek_state_timer_start;  
      float switch_to_peek_state_timer_current;
      float switch_to_peel_state_period=10000;

//END OF FSM VARIABLES-------------------------------------------------------------------------------------------



void setup() 
{

// FAN CONTROL SETUP-------------------------------------------------------------------------
   pinMode(fan,OUTPUT);
    q = 1; w = 0; e = 0; r= q^w^e;       
    a = 12; s = 0; d = 0; f= q^w^e;        
  
    Serial3.begin(19200);   // serial port 3

    myPID.SetOutputLimits(0, 255);
    myPID.SetMode(AUTOMATIC);
    // reset the sensor
       Serial3.write(a); Serial3.write(s); Serial3.write(d); Serial3.write(f);   Velocity_hi_byte  = Serial3.read();
    //empty the sensors serial buffer        
       while(Serial3.available() > 0)  {char t = Serial3.read(); }
// END OF FAN CONTROL SETUP-------------------------------------------------------------------------

// HEATER CONTOL SETUP------------------------------------------------------------------------------
      attachInterrupt(digitalPinToInterrupt(zero_cross_pin), ISR_1,  FALLING);
      pinMode (firing_pin,OUTPUT); 
      pinMode (zero_cross_pin,INPUT); 
// END OF HEATER CONTOL SETUP-------------------------------------------------------------------------

// FSM SETUP------------------------------------------------------------------------------------------
      state = Connection_confirmation;
      Serial.begin(115200);

// END OF FSM SETUP-----------------------------------------------------------------------------------

// DEBUGING SETUP------------------------------------------------------------------------------------------
       lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display } 
// END OF DEBUGING SETUP-----------------------------------------------------------------------------------
}


void loop() 
{    
    if (state == Connection_confirmation)
    {

        //     1-listen to the serial input till the message "111.1" arrives
        //     2-sends the string "Right\n" back to the serial" 
        //     3-sets the state to "Reserting"       
         lcd.setCursor(0,0);
         lcd.print(state);
         bool right_message_arrived=0;
         while (!right_message_arrived)
         {
             if ( Serial.available() )
             {

                  float tag;
                  tag =Serial.parseFloat(); 
                if (abs(tag - 111.1) <= 1.0e-05)
                {
                    Serial.println("RighT");
                    right_message_arrived=1;
                    state = Waiting_variables;
                }
             }
         }
    }
    if (state == Waiting_variables)
    {
         allow_fire_pulse=false ; 
         analogWrite(fan,0);
         lcd.setCursor(0,0);
         lcd.print(state);
         time_to_fire = maximum_firing_delay - PID_value; // close the heater //find a more convinient way to close it later
         PID_error=0; // stop the heater pid error from accumulating
         PID_i = 0;
         time_to_fire=8200;
         heater_temperature = tcouple_heater.readTempC();;  //get the real temperature in Celsius degrees

         if ( Serial.available() )
         {
            float tag;
            tag =Serial.parseFloat(); 
            if (tag == 221.2)
            {
                  control_method=1;
                  heater_setpoint=Serial.parseFloat();
                  fan_setpoint= Serial.parseFloat();
//fan_setpoint=map(fan_setpoint,0,9.43,0,17);
fan_setpoint=fan_setpoint/9.43*17;
                  switch_to_peek_state_timer_start = micros();
                  state =  PID_running;
            }
            else if (tag == 222.2)
            {
               control_method=2;
                heater_setpoint=Serial.parseFloat();
                fan_setpoint= Serial.parseFloat();
                switch_to_peek_state_timer_start = micros();
                state =  PID_running;
            }  
            
            else if (tag == 223.2)
            {
                control_method=3;
                heater_setpoint=Serial.parseFloat();
                fan_setpoint= Serial.parseFloat();
fan_setpoint=map(fan_setpoint,0,9.43,0,17);              
                switch_to_peek_state_timer_start = micros();
                state =  PID_running;
            }
            
            else if (tag == 224.2)
            {
                control_method=4;
                heater_setpoint=Serial.parseFloat();
                fan_setpoint= Serial.parseFloat();
                switch_to_peek_state_timer_start = micros();
                state =  PID_running;
              
            }       
         }
    }

    if (state == PID_running)
    {        
      //whatever the state that sent the FSM to this state it has to set ""switch_to_peek_state_timer_start = micros()"" first
      //redesign this to make PID_running sets its own time once it starts later.

      //PEEK FOR SERIAL INPUT----------------------------------------------------------------
             switch_to_peek_state_timer_current = micros()-switch_to_peek_state_timer_start ;
            if(switch_to_peek_state_timer_current > switch_to_peel_state_period)
            {
              state = Peek_for_serial_input2;
            }
      //END OF PEEK FOR SERIAL INPUT----------------------------------------------------------------      
      
      prop1_temprature = tcouple_free.readTempC();
      // CONTROL--------------------------------------------------------------------------------------------------------------------------------------------------
      if (control_method==1)
      {
                    //FAN CONTROL----------------------------------------------------------------------------
                         Serial3.write(q); Serial3.write(w); Serial3.write(e); Serial3.write(r);
                         // get a byte from serial port 3
                         if (Serial3.available())
                         {
                                Velocity_hi_byte  = Serial3.read();
                                Velocity_low_byte = Serial3.read();
                                zero_byte  = Serial3.read();
                                if (zero_byte  ==0)
                                {
                                     checksum_byte = Serial3.read();
                                     int combined = Velocity_hi_byte  << 8 | Velocity_low_byte;
                                     Velocity_m_per_s=combined/100/ 3.6;   
                                     Input = Velocity_m_per_s;
                                     myPID.Compute();
                                     analogWrite(fan,Output);
                                     fan_Power= Output/255*100;
                                }
                            
                          }
                    //END OF FAN CONTROL --------------------------------------------------------------------
                    //HEATER PID ---------------------------------------------------------------------
                        currentMillis = millis();          
                        if(currentMillis - previousMillis >= temp_read_Delay)
                        {
                                previousMillis += temp_read_Delay;                 //Increase the previous time for next loop
                                heater_temperature = tcouple_heater.readTempC();  //get the real temperature in Celsius degrees
                                PID_error = heater_setpoint - heater_temperature;        //Calculate the pid ERROR
              
                                if(PID_error < 0)                          
                                { ki2 = ki/10; allow_fire_pulse=false ; }
                                else if(PID_error >= 0 )                            
                                { ki2 = ki; allow_fire_pulse=true ; }
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
                                { PID_value = 30; }
                                previous_error = PID_error; //Remember to store the previous error.
              
                                time_to_fire=maximum_firing_delay - PID_value;
                                if (time_to_fire< 0)
                                {time_to_fire=20;}
              
                                heater_power=map(time_to_fire,20,maximum_firing_delay,300,0);
                                if(allow_fire_pulse==false)
                                {heater_power=0;}
                                Serial.println("s");
                                Serial.println(prop1_temprature);
                                Serial.println(heater_temperature);
//Velocity_m_per_s=map(Velocity_m_per_s,0,17,0,9.43);
Velocity_m_per_s=Velocity_m_per_s/17*9.43;  
                                Serial.println(Velocity_m_per_s);
                                Serial.println(fan_Power);
                                Serial.println(heater_power);          
                         }
              
                   //END OF HEATER PID ---------------------------------------------------------------------
      }
      else if (control_method==2)
      {
                    //FAN CONTROL----------------------------------------------------------------------------
                                Serial3.write(q); Serial3.write(w); Serial3.write(e); Serial3.write(r);
                                   // get a byte from serial port 3
                                   if (Serial3.available())
                                   {
                                          Velocity_hi_byte  = Serial3.read();
                                          Velocity_low_byte = Serial3.read();
                                          zero_byte  = Serial3.read();
                                          if (zero_byte  ==0)
                                          {
                                               checksum_byte = Serial3.read();
                                               int combined = Velocity_hi_byte  << 8 | Velocity_low_byte;
                                               Velocity_m_per_s=combined/100/ 3.6;   
                                          }
                                    }
                                    analogWrite(fan,fan_setpoint/100*255);
                                    fan_Power= fan_setpoint;

                    //END OF FAN CONTROL --------------------------------------------------------------------
                    //HEATER PID ---------------------------------------------------------------------
                        currentMillis = millis();          
                        if(currentMillis - previousMillis >= temp_read_Delay)
                        {
                                previousMillis += temp_read_Delay;                 //Increase the previous time for next loop
                                heater_temperature = tcouple_heater.readTempC();  //get the real temperature in Celsius degrees
                                PID_error = heater_setpoint - heater_temperature;        //Calculate the pid ERROR
              
                                if(PID_error < 0)                          
                                { ki2 = ki/10; allow_fire_pulse=false ; }
                                else if(PID_error >= 0 )                            
                                { ki2 = ki; allow_fire_pulse=true ; }
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
                                { PID_value = 30; }
                                previous_error = PID_error; //Remember to store the previous error.
              
                                time_to_fire=maximum_firing_delay - PID_value;
                                if (time_to_fire< 0)
                                {time_to_fire=20;}
              
                                heater_power=map(time_to_fire,20,maximum_firing_delay,300,0);
                                 if(allow_fire_pulse==false)
                                {heater_power=0;}
                                Serial.println("s");
                                Serial.println(prop1_temprature);
                                Serial.println(heater_temperature);
//Velocity_m_per_s=map(Velocity_m_per_s,0,17,0,9.43);
Velocity_m_per_s=Velocity_m_per_s/17*9.43;  
                                Serial.println(Velocity_m_per_s);

                                Serial.println(fan_Power);
                                Serial.println(heater_power);          
                         }
              
                   //END OF HEATER PID ---------------------------------------------------------------------
        
      }
      else if (control_method==3)
      {
                          //FAN CONTROL----------------------------------------------------------------------------
                         Serial3.write(q); Serial3.write(w); Serial3.write(e); Serial3.write(r);
                         // get a byte from serial port 3
                         if (Serial3.available())
                         {
                                Velocity_hi_byte  = Serial3.read();
                                Velocity_low_byte = Serial3.read();
                                zero_byte  = Serial3.read();
                                if (zero_byte  ==0)
                                {
                                     checksum_byte = Serial3.read();
                                     int combined = Velocity_hi_byte  << 8 | Velocity_low_byte;
                                     Velocity_m_per_s=combined/100/ 3.6;   
                                     Input = Velocity_m_per_s;
                                     myPID.Compute();
                                     analogWrite(fan,Output);
                                     fan_Power= Output/255*100;
                                }
                            
                          }
                    //END OF FAN CONTROL --------------------------------------------------------------------
                    //HEATER PID ---------------------------------------------------------------------
                        time_to_fire=map(heater_setpoint,0,150, maximum_firing_delay ,maximum_firing_delay/2);

                        if(time_to_fire==maximum_firing_delay)
                        {allow_fire_pulse=false ;}
                        else
                        {
                          allow_fire_pulse=true ; 
                        }
                        currentMillis = millis();          
                        if(currentMillis - previousMillis >= temp_read_Delay)
                        {
                                previousMillis += temp_read_Delay;                 //Increase the previous time for next loop
                                heater_temperature = tcouple_heater.readTempC();  //get the real temperature in Celsius degrees
                                heater_power=heater_setpoint;
                                if(allow_fire_pulse==false)
                                {heater_power=0;}
                                Serial.println("s");
                                Serial.println(prop1_temprature);
                                Serial.println(heater_temperature);
//Velocity_m_per_s=map(Velocity_m_per_s,0,17,0,9.43);
Velocity_m_per_s=Velocity_m_per_s/17*9.43;  
                                Serial.println(Velocity_m_per_s);
                                Serial.println(fan_Power);
                                Serial.println(heater_power);          
                         }
                        
              
                   //END OF HEATER PID ---------------------------------------------------------------------
      }      
      else if (control_method==4)
      {
                      //FAN CONTROL----------------------------------------------------------------------------
                                Serial3.write(q); Serial3.write(w); Serial3.write(e); Serial3.write(r);
                                   // get a byte from serial port 3
                                   if (Serial3.available())
                                   {
                                          Velocity_hi_byte  = Serial3.read();
                                          Velocity_low_byte = Serial3.read();
                                          zero_byte  = Serial3.read();
                                          if (zero_byte  ==0)
                                          {
                                               checksum_byte = Serial3.read();
                                               int combined = Velocity_hi_byte  << 8 | Velocity_low_byte;
                                               Velocity_m_per_s=combined/100/ 3.6;   
                                          }
                                    }
                                    analogWrite(fan,fan_setpoint/100*255);
                                    fan_Power= fan_setpoint;
    
                         //END OF FAN CONTROL --------------------------------------------------------------------
                        //HEATER PID ---------------------------------------------------------------------
                        time_to_fire=map(heater_setpoint,0,150, maximum_firing_delay ,maximum_firing_delay/2);

                        if(time_to_fire==maximum_firing_delay)
                        {allow_fire_pulse=false ;}
                        else
                        {
                          allow_fire_pulse=true ; 
                        }
                        currentMillis = millis();          
                        if(currentMillis - previousMillis >= temp_read_Delay)
                        {
                                previousMillis += temp_read_Delay;                 //Increase the previous time for next loop
                                heater_temperature = tcouple_heater.readTempC();  //get the real temperature in Celsius degrees
                                heater_power=heater_setpoint;
                                if(allow_fire_pulse==false)
                                {heater_power=0;}
                                Serial.println("s");
                                Serial.println(prop1_temprature);
                                Serial.println(heater_temperature);
//Velocity_m_per_s=map(Velocity_m_per_s,0,17,0,9.43);
Velocity_m_per_s=Velocity_m_per_s/17*9.43;  
                                Serial.println(Velocity_m_per_s);
                                Serial.println(fan_Power);
                                Serial.println(heater_power);          
                         }
                        
              
                   //END OF HEATER PID ---------------------------------------------------------------------
      }     
                  
    }
    if (state ==Peek_for_serial_input2)
    {

           if ( Serial.available() )
           {
               float tag;
               tag =Serial.parseFloat();

               if ((tag - 145.0) <= 1.0e-05)
               {
                  state = Waiting_variables;
               }
               else
               {
                  state = PID_running;
                  switch_to_peek_state_timer_start = micros();
               }
              
           }
           else 
           {
                state = PID_running;  
                switch_to_peek_state_timer_start = micros();
           }
    }
}
//End of void loop


//
void ISR_1()
{
  if (allow_fire_pulse==true)
  {
    Timer1.initialize(time_to_fire);     //set a timer for time_to_fire microseconeds 
    Timer1.attachInterrupt(open_pulse_timer_ISR);      
  }  
  else
  {
        Timer1.stop();
  }                         
}
//
void open_pulse_timer_ISR()
{
  if (allow_heater)
  {
    digitalWrite(firing_pin, HIGH); 
    Timer1.initialize(500);         
    Timer1.attachInterrupt(close_pulse_timer_ISR); 
  } 
}
//
void close_pulse_timer_ISR()
{
    digitalWrite(firing_pin, LOW); 
    Timer1.stop();
}
