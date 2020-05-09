#define SHIFT_DATA   2
#define SHIFT_CLK    3
#define SHIFT_LATCH  4
#define WRITE_ENABLE 13

/* Arduino pins connected to EEPROM I/O pins */
#define D0 5
#define D7 12

#define OUT OUTPUT
#define IN  INPUT

#define MAX_ADDRESS 255 // 2047

// Set data pins (D0-D7) as input or output.
void setDataDir(int dir) {
  for (int pin = D0; pin <= D7; pin += 1) {
    pinMode(pin, dir);
  }
}

void setAddress(int address, bool outputEnable) {
  // an int stores a 16-bit (2-byte) value

  // for CAT28C16A address
  // we need to transfer 11 bits (A0 - A10)
  // thats why we shift 2 byts of data here

  // 1. shift out the highest 3 bits of address (A10, A9, A8)
  // combined with the outputEnable bit
  // (will be the highest bit of the second 595 chip)
  // 2. then shift out the rest 8 bits (A7-A0)

  // 0x80 is 10000000
  const byte oe = outputEnable ? 0x00 : 0x80;
  const byte b1 = byte(address >> 8) | oe;
  const byte b2 = byte(address);
  // see the function table in the datasheet for 595 chip
  // to understand the meaning of signal sequence for SHIFT_CLK
  digitalWrite(SHIFT_LATCH, LOW);
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, b1);
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, b2);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

// Reads one byte from EEPROM at the given address.
byte readEEPROM(int address) {
  setDataDir(IN);
  // set outputEnable to true, since
  // we want to read what is coming in from the EERPOM
  setAddress(address, /* outputEnable */ true);
  // build a byte by reading one bit at a time
  // and store it in "data" variable
  byte data = 0;
  for (int pin = D7; pin >= D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

// Writes one byte to EEPROM at the given address.
void writeEEPROM(int address, byte data) {
  // Make sure the OE pin is low
  // before we set the I/O pins as output
  setAddress(address, /*outputEnable */ false);
  setDataDir(OUT);
  // clear the WE pin
  digitalWrite(WRITE_ENABLE, HIGH);
  setAddress(address, /* outputEnable */ false);
  for (int pin = D0; pin <= D7; pin += 1) {
    // output HIGH (1) iff the last (current) bit
    // of the "data" byte is 1
    digitalWrite(pin, data & 1);
    // advance to the next bit
    data = data >> 1;
  }
  // write "data" the EEPROM
  // (make a pusle on the WE pin)
  digitalWrite(WRITE_ENABLE, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_ENABLE, HIGH);
  // wait to get the data written
  // maximum write cycle time is 10ms
  // according to the datasheet
  delay(100);
}

void dumpEEPROM() {
  // read MAX_ADDRESS * 16 bytes, 16 bytes at a time
  for (int base = 0; base <= MAX_ADDRESS; base += 16) {
    // read 16 bytes
    byte data[16];
    for (int offset = 0; offset <= 15; offset +=1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(
        buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
        base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
        data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}

void clearEEPROM() {
  Serial.print("Erasing EEPROM");
  for (int address = 0; address <= MAX_ADDRESS; address += 1) {
    writeEEPROM(address, 0xFF);
    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");
}

// common anode
// #include "rom/5611ah.h"

// common cathode
#include "rom/1741_mess.h"

void uploadEEPROM() {
  Serial.print("Erasing EEPROM");
  for (int address = 0; address < sizeof(rom_bin); address += 1) {
    writeEEPROM(address, rom_bin[address]);
    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");
}

void setup() {
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);

  // this is a correct order
  // make sure we doesn't accidentially write EEPROM
  digitalWrite(WRITE_ENABLE, HIGH);
  pinMode(WRITE_ENABLE, OUTPUT);

  Serial.begin(57600);

  uploadEEPROM();
  dumpEEPROM();
}

void demo5611AH() {
  setDataDir(IN);
  for (int address = 0; address < 10; address += 1) {
    setAddress(address, /* outputEnable */ true);
    // set outputEnable to true, since
    // we want to output to the segment indicator
    // what is coming in from the EERPOM
    delay(500);
  }
}

void loop() {
  demo5611AH();
}
