# Arduino-GarageDoorOpener
Arduino project that uses a Wiegand keypad to work as a garage door opener. The purpose of this project is to replace the LiftMaster keypad with something more durable and robust.

<br/>
**REPLACE THIS:**  <img src="./images/original.jpg" alt="arduino" width="25%" height="25%"> 
<br/>
<br/>
**WITH THIS:**  <img src="./images/weigand_keypad.jpg" alt="arduino" width="25%" height="25%"> 
<br/>

# Why?
If you're wondering why I chose to do this, there are a few motivations:
1) The LiftMaster keypads won't last more than a couple years at my home before wearing out - they aren't cheap to replace
2) I wanted to do it better and cheaper than what you could purchase off the shelf
3) I wanted something that I could enhance over time (see future enhancements)

# Is an Arduino Required?
You could just use the Wiegand keypad directly to the garage door opener - but it isn't a good idea. If someone were to pry the keypad off the base, they'd only have to short two of the wires to open your garage door.  Putting an Arduino in between means that the attacker would need more complex protocols (a wiegand emulator) to brute force the arduino.  Failed keypad entry delays of 3 seconds should mitigate the risk of a brute force attack - assuming you use the full 8 digits.  

# Future enhancements
Future features to be put in (not in any particular order)
- open door detection
- Google Assistant support
- Increased delays on failed passcodes.


# Instructions

## Add Passcode
Short pressing the input button will enter passcode entry.  The LED will flash green - you'll have 15 seconds to input a passcode of up to 8 digits.  Then press the "#" to confirm.

## Erase All Passcodes
Long pressing the input button (5 seconds) will trigger a memory wipe and erase all stored passcodes.

## Opening The Door
1) Enter the passcode
2) Press the "#" key

It the passcode is valid, the LED will flash indicating you're in door selection mode

3) Press either 1, 2, or 3 button 
4) Press the "#" key

## Closing Doors
If you've wired in a LiftMaster keypad enter button to relay 4 - then you can trigger a door close:
1) Press "*" key
2) Press the "#" key


# Schematic
The schematic below shows the main circuitry.  The Arduino Nano accepts 12v in Vin, and the 5v output is used to power the relays and the garage door remote.

Note: 
Not shown on the schematic is that you need power to the relays.  I purchased 5v relays, so the +5v was drawn from the Arduino +5v.  There are 12v relays available too, in which case you'll draw power from the 12v power supply.

![schematic](./images/schematic.png)


# Parts
All these parts can be found easily on Amazon or Aliexpress
Part|Count|Notes
-|-|-
Arduino Nano| x1 | 
510 Ohm Resistor|x3| load resistors for LEDs
Tricolour LED | x1 | 
Buzzer | x1 | 
High Trigger Relay | x3 | used to interface to the garage door opener remote
Moment Switch | x1 | used to add or clear codes
12v power supply | x1 | main power source for Wiegand keypad and arduino.  A cheap $10-15 ac adapter with 12v @ 2A should be more than enough. 
Wiegand keypad | x1 | I used the Retekess H1EM-W 
Garage door remote | x1 | if your garage door opener is a newer one, you can't directly trigger the garage door open/close on the main unit.  I've tied it into the garage door remote instead.
Through-hole board | x1 | mine were approx 2"x3"
Wiring | x | I cut and used Cat 5 network cable.  Those wires generally work well

# Code Notes
## Max Passcode Length
The max passcode length is 8. So all passcodes are stored will take up 8 bytes.  You can change by altering the MAXCODESIZE definition.  Make sure you also change the keyMemory KEYSIZE definition to the same length.

## RFID 
This code has not been tested with RFID tags. The wiegand device I used supports it, but I never had any to test or code with.  So, if you do use this code, you may need to make minor modifications. 


# Reference
Heavily inspired by Jason Hamilton.  I heavily modified the keyMemory.h to make it a little easiler to use.  You can find his work here: https://bitbucket.org/jason955/arduino-access-control/src/master/


# Assembly
Basic assembly is basically a few main parts:
1) Through hole board with Arduino, LEDs, button, and buzzer
2) Garage door remote wireup
3) Connecting everything together



## Through-hold Board with Arduino
Basically you're looking to get your Arduino board soldered to the through-hole board (with sockets - not directly) along with some wires so you can hook into the relays and keypad.  See images below.
<br/>
<img src="./images/arduino_LI.jpg" alt="arduino" width="50%" height="50%">

## Garage Door Wireup
As stated earlier, newer garage door openers don't have two pins on the main unit to short to trigger the garage door to open.  With no easy access, you'll have to sacrifice a garage door remote to make it work.  The unit i used was from LiftMaster and is designed to be used in your car.  Opening up the casing and you'll see a very simple mainboard - we'll use relays to act as button presses.  

1) Add wire to the buttons so we can hook it up to the relays
2) Add wire to the solder points where the battery normally goes.  We'll use the +5v from the arduino to power it rather use batteries
<br/>
<img src="./images/garage_door_remote_annotated.jpg" alt="arduino" width="50%" height="50%">
<br/>
## Connecting It All Together
This is the easy part - while the picture is messy, i assure you, it's not all that bad.
Garage door remote
- connect to arduino +5v
- connect to relays

Relays
- connect to arduino

Weigand Keypad
- connect to power
- connect to arduino

<br/>
<img src="./images/all_wired.jpg" alt="arduino" width="50%" height="50%">

## New - Door Close Feature
I still had a working LiftMaster keypad.  The "enter" button acts as a door close feature.  The main unit knows that pressing the "enter" button will only close doors if they are open.  Having this feature is easier than having to punch in the full code to close the door.

To use this capability:
- D12 of the Arduino output needs to output to relay 4
- the "enter" button on the keypad needs to be shorted by relay 4.  








