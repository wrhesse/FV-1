#include <Wire.h>

/* We're using a 24LC32A */
// Make sure all of these match what's in the pedal's code, too.
#define EEPROM_ADDRESS B1010000 // 1010 + 3 bits for pins 1,2,3
#define BUFFER_SIZE 30 // It has a 32 byte page write buffer, but we use as little as possible to save space on the arduino's RAM.
#define PAGE_SIZE 32
#define EEPROM_CLOCK_SPEED 400000


/* This is interpretted as sets of 5 strings: the program name, optional line 2 of program name, then 3 control labels.
*/
#define NUM_STRINGS 5  // 5 strings per program
#define NUM_PROGRAMS 8 // 8 programs per bank
#define NUM_BANKS 1   // one bank per eeprom. V1 of the pedal also stored the internal ROM bank on here.
char *labels[] = {
  "Envelope Filter\0",
  "\0",
  "Amount\0",
  "Resonance\0",
  "Frequency\0",

  "Envelope Tremolo\0",
  "\0",
  "Rate\0",
  "Depth\0",
  "Mode\0",

  "Harmonic Tremolo\0",
  "\0",
  "Rate\0",
  "Depth\0",
  "Filter Cutoffs\0",

  "Tremolo\0",
  "\0",
  "Rate\0",
  "Depth\0",
  "Shape\0",

  "Tap Delay\0",
  "\0",
  "Tap\0",
  "Divisions\0",
  "Feedback\0",

  "Reverb/HP/LP\0",
  "\0",
  "Reverb\0",
  "High Pass Filter\0",
  "Low Pass Filter\0",

  "Spring Reverb + Tremolo\0",
  "\0",
  "Reverb\0",
  "Rate\0",
  "Depth\0",

  "Through Zero Flanger\0",
  "\0",
  "Rate\0",
  "Feedback\0",
  "N/A\0"
};



void setup() {
  Serial.begin(9600);

  Wire.begin();
  Wire.setClock(EEPROM_CLOCK_SPEED);

  unsigned int address = 0x0;
  for (int i = 0; i < NUM_STRINGS * NUM_PROGRAMS * NUM_BANKS; i++) {
    Serial.print(address);
    Serial.print(": ");
    Serial.println(labels[i]);
    writeEEPROMPage(address, labels[i]);
    address = address + PAGE_SIZE;
  }

  Serial.println("***********DONE************");
}

void loop() {
}

/* NOTE: This is not general purpose. It's only useful for writing within a single page.
    Address needs to be the start of a page.
*/
void writeEEPROMPage(unsigned int address, char *data)
{
  unsigned char i = 0;

  Wire.beginTransmission(EEPROM_ADDRESS);

  Wire.write((int)(address >> 8)); // most significant byte
  Wire.write((int)(address & 0xFF)); // least significant byte

  while (i < BUFFER_SIZE) {
    // Serial.println(i);
    // Serial.println(data[i]);
    Wire.write((byte) data[i]);
    if (data[i] == '\0') {
      break;
    }
    i++;
  }

  //Serial.println("Ending transmission");
  Wire.endTransmission();
  //Serial.println("Ended");
  delay(6);  // needs 5ms for page write
}
