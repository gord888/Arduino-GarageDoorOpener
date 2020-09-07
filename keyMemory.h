
// library to store the keys in EEPROM
// written by Jason Hamilton


// The microcontrollers on the various Arduino boards have different 
// amounts of EEPROM: 1024 bytes on the ATmega328, 512 bytes on the 
// ATmega168 and ATmega8, 4 KB (4096 bytes) on the ATmega1280 and ATmega2560.
// arduino.cc/en/Reference/EEPROM
#define EEPROM_SIZE (1024)
#define KEYSIZE 8 // 8 digit key codes

//////////////////
//  HEADER////////
//////////////////
class KeyMemory{
  public:
    KeyMemory(void);
    void reset();
    bool addKey(uint8_t key[]);
    bool isKeyPresent(uint8_t key[]);
    void readKey(int pos);
    void dumpEeprom();   
    void clearEeprom(); 
    uint8_t keyCount();
    uint8_t key[KEYSIZE];
    bool compareKeys(uint8_t key1[], uint8_t key2[]);
  private:
    uint8_t _keyCount;
    void _incrementKeyCount();
};



//////////////////////
/// IMPLEMENTATION ///
//////////////////////

KeyMemory::KeyMemory(){
  uint8_t keyCount = EEPROM.read(0);
  if (keyCount == 255){
    reset();
  }
  _keyCount = keyCount;
}

uint8_t KeyMemory::keyCount()
{
  return _keyCount;
}

void KeyMemory::reset(){
  EEPROM.write(0,0);
  _keyCount = 0;
}

//add key to last spot in EEPROM
bool KeyMemory::addKey(uint8_t key[]){

  // this technically shouldn't happen - but i guess better safe than sorry?
  if (_keyCount > (EEPROM_SIZE / KEYSIZE) - 1 ){
    return 0;
  } 
  else
  {
    // read the number of keys alraedy in storage
    // write to that area

    int writePos = (_keyCount * 8) + 1;

    for(int i = 0; i < KEYSIZE; i++)
    {
      EEPROM.write(writePos + i, key[i]);
    }

    _incrementKeyCount();
    Serial.println("Key saved!");
    return 1;
  }
  
}



bool KeyMemory::isKeyPresent(uint8_t checkKey[]){
  if(_keyCount > 0)
  {
    for (int i = 0; i <= _keyCount; i++)
    {
      readKey(i);
      bool match = compareKeys(key,checkKey);

      if(match){
        Serial.println("Found a matching key!");
        return 1;      
      }      
    }
  }
  
  return 0;
}


/// comparing 2 keys if they are equal
boolean KeyMemory::compareKeys(uint8_t key1[], uint8_t key2[]){
  for (int i=0; i < KEYSIZE; i++ ){
    if (key1[i] != key2[i]){
      return 0;
    }
  }
  return 1;
}


/// read the key that in the memory position
/// we'll be skipping eeprom slots
void KeyMemory::readKey(int pos)
{
  // skip position zero
  // start looking from index 1
  int start = (pos*KEYSIZE) + 1;

  int k = 0;
  for(int i = start; i < start + KEYSIZE; i++)
  {
     key[k++] = EEPROM.read(i);
  }
  
  
}


// reset eeprom to all 255
void KeyMemory:: clearEeprom(){
  for(int i = 0; i < EEPROM.length(); i++)
  {
    EEPROM.update(i, 255);
    EEPROM.write(0,0);
    _keyCount = 0;
  }  
}



void KeyMemory::dumpEeprom(){
  int value;
  for(int i = 0; i <= EEPROM.length(); i++){
    Serial.print(EEPROM.read(i));
    Serial.print(",");
  }

  Serial.println();
}

void KeyMemory::_incrementKeyCount(){
  _keyCount++;
  EEPROM.write(0,_keyCount);
}
