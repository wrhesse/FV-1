// The SpinASM software can take a spin project and write
// a C++ header file with 8 sets of ROM data.
// If you call your spin project "Bob" then it will output
// a header file called "Bob.h" containing lines such as
//   Bob_00[] = {
// etc. for the 8 programs in that project.
//
// This Arduino sketch wants a header file called "ROM.h"
// with lines that say:
//   const unsigned char ROM_00[] PROGMEM = {
// etc.
//
// So do the following...
//   1) Open the header file produced by SpinASM (e.g. "Bob.h")
//      in Notepad++.
//   2) Find all 8 occurrences of "Bob" in the file and replace
//      them with "const unsigned char ROM".
//   3) Find all 8 occurrences of "[]" in the file and replace
//      them with "[] PROGMEM".
//   4) Do "Encoding -> Convert to ANSI", and then save the file
//      as "ROM.h" in this sketch folder.
//   Tip: You can use Notepad++ to record a macro to perform
//        steps 2) and 3).  I ended up doing those steps lot !!!
//
//----------------------------------------------------------------
//
// The 3 address pins on the EEPROM IC define the 3 lowest bits of
// the EEPROM address. The address 0x50 in the code means the
// three address pins on the EEPROM IC should be wired low.
//
// So connect the EEPROM to the Arduino as follows:
//
// EEPROM pin  5         ==> Arduino pin A4 (SDA)
// EEPROM pin  6         ==> Arduino pin A5 (SCL)
// EEPROM pins 1,2,3,4,7 ==> Arduino GND
// EEPROM pin  8         ==> Arduino 5V
//
// When you write this sketch to the Arduino to run it, open the
// serial port message window to see the program messages.
// 
// I only insert/remove the EEPROM while the Arduino is not powered.


#include <Wire.h> //I2C library
#include <avr/pgmspace.h>
#include "ROM.h"

const byte* ROMS[] = {(byte*)ROM_00, (byte*)ROM_01, (byte*)ROM_02, (byte*)ROM_03, (byte*)ROM_04, (byte*)ROM_05, (byte*)ROM_06, (byte*)ROM_07 };

const int EEPROM_ADDR = 0x50;

void setup()	// Write to EEPROM
{
  delay(500);

  const int pinSDA = 4;  // SDA = Pin A4.  On the Uno Revision 3 board, this is the same as pin 16.
  const int pinSCL = 5;  // SCL = Pin A5.  On the Uno Revision 3 board, this is the same as pin 17.
  
  pinMode(pinSDA, OUTPUT);  
  pinMode(pinSCL, OUTPUT);  
  digitalWrite(pinSDA, HIGH);
  digitalWrite(pinSCL, LOW);

  Serial.begin(9200);
  while (!Serial) {}	// Wait for serial port to connect (for Leonardo boards)

  Serial.println("Spin FV-1 EEPROM Writer");

  // Check that ROM.h has correct size ROMs.  They should be 512 bytes each.
  Serial.println("Checking ROM sizes:");
  Serial.print("ROM_00: ");  Serial.print(sizeof(ROM_00));  Serial.println(" Bytes");
  Serial.print("ROM_01: ");  Serial.print(sizeof(ROM_01));  Serial.println(" Bytes");
  Serial.print("ROM_02: ");  Serial.print(sizeof(ROM_02));  Serial.println(" Bytes");
  Serial.print("ROM_03: ");  Serial.print(sizeof(ROM_03));  Serial.println(" Bytes");
  Serial.print("ROM_04: ");  Serial.print(sizeof(ROM_04));  Serial.println(" Bytes");
  Serial.print("ROM_05: ");  Serial.print(sizeof(ROM_05));  Serial.println(" Bytes");
  Serial.print("ROM_06: ");  Serial.print(sizeof(ROM_06));  Serial.println(" Bytes");
  Serial.print("ROM_07: ");  Serial.print(sizeof(ROM_07));  Serial.println(" Bytes");

  Wire.begin(); // Initialise Wire library
  
  // Write ...
  for (unsigned int iRom = 0; iRom < 8; iRom++)  // Loop ROMs
  {
    Serial.print("Writing ROM_0");  Serial.println(iRom);

    const byte* pROM = ROMS[iRom];  // Pointer to data to copy to EEPROM
	  
    for (unsigned int page = 0; page < 512; page += 16 )  // Loop data in 16-byte pages (Wire library has limited buffer)
    {
      const unsigned int eeaddress = iRom*512 + page;	  // EEPROM address of page
      
      Wire.beginTransmission(EEPROM_ADDR);
      Wire.write((int)(eeaddress >> 8));    // MSB
      Wire.write((int)(eeaddress & 0xFF));  // LSB
      for (unsigned int i = 0; i < 16; i++ )  // Loop data bytes in page
      {
        Wire.write( pgm_read_byte_near(pROM + page + i) );  // Write to EEPROM
      }
      Wire.endTransmission();
      delay(10); // Small delay
    }
  }
  
  // Verify ...
  for (unsigned int iRom = 0; iRom < 8; iRom++)  // Loop ROMs
  {
    Serial.print("Verifying ROM_0");  Serial.print(iRom);
		
    const byte* pROM = ROMS[iRom];  // Pointer to data to verify against EEPROM
		
    int nBytesOK(0);  // Number of bytes that were written OK
    for (unsigned int i = 0; i < 512; i++ )  // Loop data bytes
    {
      const unsigned int eeaddress = iRom*512 + i;  // EEPROM address
      	
      Wire.beginTransmission(EEPROM_ADDR);
      Wire.write((int)(eeaddress >> 8));    // MSB
      Wire.write((int)(eeaddress & 0xFF));  // LSB
      Wire.endTransmission();
      Wire.requestFrom(EEPROM_ADDR, 1);
      if ( Wire.available() )
      {
	    const byte data = Wire.read();  // Read byte from EEPROM
	    if ( data == pgm_read_byte_near(pROM + i) )  // Check it is as expected
          nBytesOK++;
      }
      if ( (i % 32) == 31 )
        delay(10);	// Small delay every 32 bytes		
    }
    Serial.print(": "); Serial.print(nBytesOK); Serial.println(" Bytes written correctly");
  }
  Serial.println("DONE");
}

void loop()
{
  while(true) {}
}

