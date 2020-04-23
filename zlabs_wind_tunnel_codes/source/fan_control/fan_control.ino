#define  fan 5
#include <PID_v1.h>
//Define PID controller variables
 float Velocity_m_per_s;
//--------------------------------------------------------------------------------------------------------------------------------------------
double  Kp =10;
double  Ki =10;
double  Kd =0;
double  Setpoint =5;
//--------------------------------------------------------------------------------------------------------------------------------------------
//Specify the links and initial tuning parameters
double  Input, Output; //Define Variables we'll be connecting to
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

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

void setup() 
{
   pinMode(fan,OUTPUT);
     
  
    Serial3.begin(19200);   // serial port 3
    Serial.begin(9600);

    myPID.SetOutputLimits(0, 255);
    myPID.SetMode(AUTOMATIC);
    analogWrite(fan,255);

   
}

void loop() 
{
  
//        delay(1000);
      
           Velocity_m_per_s=GetVelocity();
          Serial.println(Velocity_m_per_s);
//          Input = Velocity_m_per_s*4;
//          myPID.Compute();
//          Serial.println(Output);
//          Serial.println(" **************************** ");
 
        



}

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
