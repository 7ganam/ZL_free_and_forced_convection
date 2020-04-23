/*******************************************************************************

     Filename: DegreeC_UART.ino
     Development Board: Arduino Uno SMD R3
     Project: Reading DegreeC Embedded Sensors with Arduino
     Summary: The sketch will attempt to read a sensor's Temperature,
              Velocity and Status of the Arduino's UART. 
     Author: Eric Zweighaft
     Version: 1.0
     Published: 2019-06-03

     Copyright (c) 2019 Degree Controls, Inc - Milford, NH
*******************************************************************************/

#include <Wire.h>

const byte CMD_VELOCITY = 0x01;
const byte CMD_TEMPERATURE = 0x02;
const byte CMD_WRITE = 0x06;
const byte CMD_READ = 0x07;
const byte REG_STATUS = 0x42;


void setup() 
{
  Serial3.begin (19200); // Enable communication to the Sensor through the Serial3 Port
  Serial.begin(9600);
}

void loop() 
{
  float velocity = GetVelocity ();
  Serial.println(velocity);
//  float temperature = GetTemperature ();
//  byte sensorStatus = GetStatus();
}

/*
 *  GetVelocity
 *  Summary: Requests the Velocity in meters per second throug the UART
 *  returns: A floating point value of the sensor's velocity reading in m/s
 */

float GetVelocity ()
{
  unsigned int result;
  byte buf [4];

  Serial3.write(CMD_VELOCITY);
  Serial3.write(0);
  Serial3.write(0);
  Serial3.write(CMD_VELOCITY); // the checksum of any byte XOR'd with two 0s will always be the that byte

  Serial3.readBytes(buf, 4);
  
 
  result = buf[0]; // first byte read is MSB
  result = result << 8; // shift the MSB up;
  result += buf[1]; // add in the LSB
 
  return ((float) result) / 1000.0;
}

/*
 *  GetTemperature
 *  Summary: Requests the sensor's ambient temperature through the UART
 *  returns: A floating point value of the sensor's ambient reading in celcius
 */

float GetTemperature ()
{
  unsigned int result;
  byte buf [4];

  Serial3.write(CMD_TEMPERATURE);
  Serial3.write(0);
  Serial3.write(0);
  Serial3.write(CMD_TEMPERATURE); // the checksum of any byte XOR'd with two 0s will always be the that byte

  Serial3.readBytes(buf, 4);
  
 
  result = buf[0]; // first byte read is MSB
  result = result << 8; // shift the MSB up;
  result += buf[1]; // add in the LSB
  return ((float) result) / 100.0;
}

/*
 *  GetStatus
 *  Summary: Requests the sensor's status through the UART
 *  returns: An 1 byte bitfield of the sensor's status. If bits are set, the errors are:
 *              bit 0 - Not Used
 *              bit 1 - Flow bead control error
 *              bit 2 - Voutput Control Error, disabled if VoutMaximum=0
 *              bit 3 - Ambient Temperature Sensor Error
 *              bit 4 - Air Flow Temperature Sensor Error
 *              bit 5 - Toutput Control Error, disabled if ToutMaximum=0
 *              bit 6 - (F500 Only): 0: current output drive error enabled; 1: current output drive error disabled
 *              bit 7 - Not Used
 */

byte GetStatus ()
{
  unsigned int result;
  byte buf [4];

  Serial3.write(CMD_READ);
  Serial3.write(REG_STATUS);
  Serial3.write(0);
  Serial3.write(CMD_READ); // the checksum of any byte XOR'd with two 0s will always be the that byte

  Serial3.readBytes(buf, 4);
  
  return buf [1];
}
