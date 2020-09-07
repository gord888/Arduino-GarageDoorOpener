#include "EEPROM.h"
#include "keyMemory.h"
#include "ButtonHandler.h"
#include "Wiegand.h"


/*
  TODO:
  - test with RFID tags
    - need to order some

*/



//////////////////////////////////////////////////////////////
/////////////////////////// DEFINITIONS //////////////////////

// Input Button 1 - for setting the passcode
#define PIN_SETTINGS 5

// max code size
#define MAXCODESIZE 8
#define MAXSAVEDCODES

// LEDS
#define REDLED 6
#define GREENLED 8
#define BLUELED 7

// door output pins
#define DOOR1 9
#define DOOR2 10
#define DOOR3 11

// buzzer pin
#define BUZZER 4




// These are the pins connected to the Wiegand D0 and D1 signals.
// Ensure your board supports external Interruptions on these pins
#define PIN_D0 2
#define PIN_D1 3




//////////////////////////////////////////////////////////////
/////////////////////////// GLOBAL VARS //////////////////////

// setup pin entry handling
uint8_t passcode[MAXCODESIZE];    // stores the pin as entered on the keypad (note # button is used to trigger logic)
int currentDigit;                 // stores the current digit that is being typed in from the keypad
boolean isDataReady;              // when the # is typed in, it allows the "receive data" - the logic will consume this and mark the "isDataready" as false



// used to manage when the last time the wiegand bus was checked
unsigned long lastKeyPress;       // stores the last time a keypress happened to timeout the passcode at the same time as the keypad triple beep
unsigned long lastInteruptCheck;  // used to determine if interupts need to be checked

// settings button handlers
int settingsButtonEvent;
ButtonHandler settingsButtonHandler(PIN_SETTINGS);
unsigned long keypadModeEnd; // used to ensure the duration of the settings mode
int keypadMode;  // 0 = passcode mode, 1 = add code mode, 2 = door select mode


// used for storing the keys
KeyMemory key_storage;

// The object that handles the wiegand protocol
Wiegand wiegand;


// used for flashing LEDS
unsigned long lastLedFlash;
boolean ledOn;



/////////////////////////////////////////////////////////
/////////////////////////// SETUP  //////////////////////
// Initialize Wiegand reader
void setup() {
  Serial.begin(9600);

  //Install listeners and initialize Wiegand reader
  wiegand.onReceive(receivedData, "Card read: ");
  wiegand.onReceiveError(receivedDataError, "Card read error: ");
  wiegand.onStateChange(stateChanged, "State changed: ");
  wiegand.begin(Wiegand::LENGTH_ANY, true);

  //initialize pins as INPUT and attaches interruptions
  pinMode(PIN_D0, INPUT);
  pinMode(PIN_D1, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_D0), pinStateChanged, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_D1), pinStateChanged, CHANGE);

  //Sends the initial pin state to the Wiegand library
  pinStateChanged();



  // setup input for settings button
  pinMode(PIN_SETTINGS, INPUT_PULLUP);

  // output pins
  pinMode(REDLED, OUTPUT);
  pinMode(BLUELED, OUTPUT);
  pinMode(GREENLED, OUTPUT);

  pinMode(DOOR1, OUTPUT);
  pinMode(DOOR2, OUTPUT);
  pinMode(DOOR3, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  // set passcode to -1 for empty
  clearPasscode();

  // set to 0 to ensure we check on first loop
  lastInteruptCheck = 0;

  // set to 0 to ensure we check on first loop
  lastLedFlash = 0;

  // settings mode is 0 by default
  //settingsMode = false;
  keypadMode = 0;


}



////////////////////////////////////////////////////////
/////////////////////////// LOOP  //////////////////////
// Every few milliseconds, check for pending messages on the wiegand reader
// This executes with interruptions disabled, since the Wiegand library is not thread-safe
void loop() {

  // check interrupts every 100ms
  checkWiegandInterrupts();

  // check if last keypress is more than 5 seconds
  if (millis() - lastKeyPress >= 4950 && currentDigit > 0)
  {
    clearPasscode();
    Serial.println("beep beep beep");
  }


  // button checking
  settingsButtonEvent = settingsButtonHandler.handle();
  if (settingsButtonEvent != EV_NONE)
  {

    if (settingsButtonEvent == EV_LONGPRESS && keypadMode != 1) // not add code mode
    {
      // clear memory
      digitalWrite(REDLED, HIGH);
      digitalWrite(BLUELED, HIGH);
      digitalWrite(GREENLED, HIGH);
      blinkOnce(BUZZER, 250);

      Serial.println("Clearing memory");
      //printMemoryStatus();
      key_storage.clearEeprom();
      //printMemoryStatus();
      
      blinkOnce(BUZZER, 250);
      digitalWrite(REDLED, LOW);
      digitalWrite(BLUELED, LOW);
      digitalWrite(GREENLED, LOW);

    }
    else if (settingsButtonEvent == EV_SHORTPRESS)
    {
      Serial.println("Entering Add Keycode Mode");
      keypadMode = 1; // enter add code mode
      keypadModeEnd = millis() + 15000; //end after 15 second timeout

      digitalWrite(BLUELED, HIGH);
      ledOn = true;
    }
  }

  // Handle keypadMode 1 or 2 - add keycode mode and door selection mode
  // flash the LED at intervals and timeout this mode
  if (keypadMode == 1 )
  {
    flashLed(BLUELED);

    if (millis() > keypadModeEnd)
    {
      Serial.println("Returning to Normal Key Mode");

      stopFlashLed(BLUELED);
      keypadMode = 0;
    }
  } 
  else if (keypadMode == 2 )
  {
    flashLed(GREENLED);

    if (millis() > keypadModeEnd)
    {
      Serial.println("Returning to Normal Key Mode");

      stopFlashLed(GREENLED);
      keypadMode = 0;
    }
  } 



  // Core Logic
  // only activated when data is ready for processing
  if (isDataReady)
  {
    // run the core logic
    coreLogic();
  }


}





////////////////////////////////////////////////////////
///////////////////////// CORE LOGIC //////////////////////

void coreLogic()
{
  // if keypadmode 0 - then the user is trying to open the garage door
  // check if the code is valid
  // if it's valid, then enter door selection mode
  // if not, then buzz and show red LED

  // if keypadmode 1 - then the use is trying to add another pin
  // try to store the pin in eeprom
  // store ok - green
  // already exists - blue

  // if keypadmode 2 - then the user is select a door to open
  // the first digit they enter is the door to open


  printLnPasscode();

  // only perform main logic if there's at least 1 key entered
  if (currentDigit > 0) {


    
    if (keypadMode == 0) // passcode mode
    {
      boolean keyPresent = key_storage.isKeyPresent(passcode);

      // if we find the key, then switch to door mode
      if (keyPresent)
      {
        Serial.println("Code found - switching to door selection mode.");
        keypadMode = 2;
        keypadModeEnd = millis() + 5000; //end after 5 seconds
      } else {
        Serial.println("CODE NOT FOUND!");
        digitalWrite(REDLED, HIGH);
        blinkOnce(BUZZER, 2000);
        digitalWrite(REDLED, LOW);
      }
    }




    else if (keypadMode == 1) // new pass code mode
    {
      // turn off the green LED so we can ou
      stopFlashLed(BLUELED);

      Serial.println("Attempting to add keycode.");
      if (addKeyToEeprom(passcode))
      {
        // added successfully
        blinkOnce(GREENLED, 1000);
      }
      else
      {
        // already exists
        blinkOnce(BLUELED, 1000);
      }

      // go back to normal mode
      keypadMode = 0;
    }




    else if (keypadMode == 2) // door select mode
    {
      // read the first digit of the key pressed
      // this will be the door to open

      Serial.print("Selected door: ");
      Serial.println(passcode[0]);

      if(passcode[0] >=1 && passcode[0] <= 3)
      {
      switch(passcode[0])
      {
        case 1:
          blinkOnce(DOOR1, 1500);
          break;
        case 2:
          blinkOnce(DOOR2, 1500);
          break;
        case 3:
          blinkOnce(DOOR3, 1500);
          break;
      }
      } 
      else 
      {
        Serial.println("Invalid door number");
      }
      
      // go back to normal mode
      stopFlashLed(GREENLED);
      //stopFlashLed(REDLED);
      //stopFlashLed(BLUELED);
      keypadMode = 0;

    }
  }


  // no digits entered - back to keypadMode 0;
  else {    
      Serial.println("Switching to normal mode.");
      stopFlashLed(GREENLED);
      stopFlashLed(REDLED);
      stopFlashLed(BLUELED);
      keypadMode = 0;    
  }
  
  // always clear the passcode at the end.
  clearPasscode();



}


////////////////////////////////////////////////////////
///////////////////////// HELPERS METHODS //////////////////////

void printMemoryStatus()
{
  //testing stuff here
  // read the eeprom values
  Serial.print("EEPROM Size: ");
  Serial.println(EEPROM.length());

  key_storage.dumpEeprom();

  Serial.print("Key count: ");
  Serial.println(key_storage.keyCount());

  for (int i = 0; i < key_storage.keyCount(); i++)
  {
    key_storage.readKey(i);

    Serial.print("key from memory: ");
    for (int i = 0; i < sizeof(key_storage.key); i++)
    {
      Serial.print(key_storage.key[i]);
      Serial.print(",");
    }

    Serial.println();
  }

}



// Clears the passcode that was typed in
void clearPasscode()
{
  Serial.println("Clearing passcode");
  for (int i = 0; i < MAXCODESIZE; i++)
  {
    passcode[i] = 255;
  }
  currentDigit = 0;
  isDataReady = false;
}

void printLnPasscode()
{
  Serial.print("passcode");
  for (int i = 0; i < MAXCODESIZE; i++)
  {
    Serial.print(passcode[i]);
    Serial.print(",");
  }
  Serial.println();
}





// stop flashing the led
void stopFlashLed(int pin)
{
  digitalWrite(pin, LOW);
  ledOn = false;
}

// flash leds - make sure this is in the main loop
void flashLed(int pin)
{
  if (millis() - lastLedFlash > 250) {
    if (ledOn)
    {
      //Serial.println("Turn OFF LED");
      digitalWrite(pin, LOW);
      ledOn = false;
    } else {
      //Serial.println("Turn ON LED");
      digitalWrite(pin, HIGH);
      ledOn = true;
    }
    lastLedFlash = millis();
  }
}


void blinkOnce(int pin, unsigned long ms)
{
  digitalWrite(pin, HIGH);

  unsigned long endTime = millis() + ms;
  while (millis() < endTime)
  {
    // ... wait
  }
  digitalWrite(pin, LOW);

}


bool addKeyToEeprom(uint8_t uid[]) {
  if (key_storage.isKeyPresent(uid)) {
    //key already here
    Serial.println("Card already present");
  }
  else
  {
    Serial.println("adding new card");
    key_storage.addKey(uid);
    return 1;
  }

  return 0;
}


////////////////////////////////////////////////////////////////////
///////////////////////// WIEGAND SUPPORT METHODS//////////////////////

// called in main loop, but rather than use a delay
// use a time check to see if we should check for data
void checkWiegandInterrupts()
{
  if (millis() - lastInteruptCheck > 100) {
    noInterrupts();
    wiegand.flush();
    interrupts();

    lastInteruptCheck = millis();
  }

}


// When any of the pins have changed, update the state of the wiegand library
void pinStateChanged() {
  wiegand.setPin0State(digitalRead(PIN_D0));
  wiegand.setPin1State(digitalRead(PIN_D1));
}

// Notifies when a reader has been connected or disconnected.
// Instead of a message, the seconds parameter can be anything you want -- Whatever you specify on `wiegand.onStateChange()`
void stateChanged(bool plugged, const char* message) {
  Serial.print(message);
  Serial.println(plugged ? "CONNECTED" : "DISCONNECTED");
}



// Notifies when a card was read.
// Instead of a message, the seconds parameter can be anything you want -- Whatever you specify on `wiegand.onReceive()`
void receivedData(uint8_t* data, uint8_t bits, const char* message) {

  lastKeyPress = millis(); // used to clear the passcode if timeout

  uint8_t bytes = (bits + 7) / 8;

  // for each byte - put the int into the passcode array
  for (int i = 0; i < bytes; i++) {

    // if we come across input 11 from the keypad, then
    // we have to assume the passcode is done
    if (data[i] == 11 || currentDigit >= MAXCODESIZE)
    {
      // tell the app that data is ready for the core logic to take over
      isDataReady = true;
    } else {
      // add data to the passcode array then increment the current digit
      passcode[currentDigit++] = data[i];
    }
  }

  // for debugging only - output the passcode to serial
  //printLnPasscode();
}

// Notifies when an invalid transmission is detected
void receivedDataError(Wiegand::DataError error, uint8_t* rawData, uint8_t rawBits, const char* message) {
  Serial.print(message);
  Serial.print(Wiegand::DataErrorStr(error));
  Serial.print(" - Raw data: ");
  Serial.print(rawBits);
  Serial.print("bits / ");

  //Print value in HEX
  uint8_t bytes = (rawBits + 7) / 8;
  for (int i = 0; i < bytes; i++) {
    Serial.print(rawData[i] >> 4, 16);
    Serial.print(rawData[i] & 0xF, 16);
  }
  Serial.println();
}
