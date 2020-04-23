
byte rx_byte1 ;        // stores received byte
byte rx_byte2 ;        // stores received byte
byte rx_byte3 ;        // stores received byte
byte rx_byte4 ;        // stores received byte

byte q ;        // stores received byte
byte w ;        // stores received byte
byte e ;        // stores received byte
byte r;        // stores received byte

void setup() 
{
  q = 1;        // stores received byte
  w = 0;        // stores received byte
  e = 0;        // stores received byte
  r= q^w^e;        // stores received byte

  // put your setup code here, to run once:
  Serial3.begin(19200);   // serial port 3
  Serial.begin(9600);
  Serial3.flush();
    pinMode(12,OUTPUT);
  analogWrite(12,125);
        while(Serial3.available() > 0) 
      {
         char t = Serial3.read();
      }
delay(1000);

analogWrite(5,255);
}

void loop() 
{ 

analogWrite(5,255);

      Serial3.write(q);
      Serial3.write(w);
      Serial3.write(e);
      Serial3.write(r);
    
        // get a byte from serial port 3
        if (Serial3.available())
     {
        rx_byte1 = Serial3.read();
        Serial.print(rx_byte1);
        Serial.print(" * ");
        
        rx_byte2 = Serial3.read();
        Serial.print(rx_byte2);
        Serial.print(" * ");
        
        rx_byte3 = Serial3.read();
        Serial.print(rx_byte3);
        Serial.print(" * ");

        if (rx_byte3 ==0)
        {
          rx_byte4 = Serial3.read();
          Serial.print(rx_byte4);
          Serial.print(" * ");
  
          int combined = rx_byte1 << 8 | rx_byte2;
          float combined2=combined/100/ 3.6;
          Serial.print(combined2);
  
          Serial.println(" **************************** ");
        }
        
     }

    
}
