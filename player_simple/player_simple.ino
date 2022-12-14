/*************************************************** 
  VS1053 Sound Player by qwerty13
  20/09/1401
****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// define the pins used
#define CLK 13       // SPI Clock, shared with SD card
#define MISO 12      // Input data, from VS1053/SD card
#define MOSI 11      // Output data, to VS1053/SD card
// These are the pins used for the music maker shield
#define SHIELD_RESET  8      // VS1053 reset pin (unused!)
#define SHIELD_CS     6      // VS1053 chip select pin (output)
#define SHIELD_DCS    7      // VS1053 Data/command select pin (output)
// These are common pins between breakout and shield
#define CARDCS 9     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 2       // VS1053 Data request, ideally an Interrupt pin

unsigned int fileName = 0;

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
  
void setup() {
  Serial.begin(9600);
  Serial.println("VS1053 Sound Player by qwerty13");

  if (! musicPlayer.begin()) {
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);
  }

  // list files
  Serial.println("files found in SD card:");
  printDirectory(SD.open("/"), 0);
  Serial.println("");
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);
  // // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // // audio playing
  // musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

  // // Play one file, don't return until complete
  // Serial.println(F("Playing track 001"));
  // musicPlayer.playFullFile("/track001.mp3");
  // // Play another file in the background, REQUIRES interrupts!
  // Serial.println(F("Playing track 002"));
  // musicPlayer.startPlayingFile("/track002.mp3");

  // play files
  playDirectory(SD.open("/"), 0);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    
    // if we get an 's' on the serial console, stop!
    if (c == 's') {
      musicPlayer.stopPlaying();
    }
    
    // if we get an 'p' on the serial console, pause/unpause!
    if (c == 'p') {
      if (! musicPlayer.paused()) {
        Serial.println("Paused");
        musicPlayer.pausePlaying(true);
      } else { 
        Serial.println("Resumed");
        musicPlayer.pausePlaying(false);
      }
    }
  }

  delay(100);
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
  while(true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    for (uint8_t i=0; i<numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs+1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

/// File playing helper
void playDirectory(File dir, int numTabs) {
  while(true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      Serial.println(">>> no more files");
      playDirectory(SD.open("/"), 0);
      break;
    }

    Serial.print(">>> now playing: ");
    Serial.print(entry.name());
    Serial.print("\t\t");
    Serial.println(entry.size(), DEC);
    musicPlayer.playFullFile(entry.name());

    entry.close();
  }
}