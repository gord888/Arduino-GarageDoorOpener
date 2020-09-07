# Arduino-GarageDoorOpener
Arduino project that uses a Wiegand keypad to work as a garage door opener.  More details to follow.



# Wiring Notes:
This will have to do until i find a schematic design app i like.
## Arduino
PIN 2 -> Wiegand Keypad D0
PIN 3 -> Wiegand Keypad D1

PIN 4 -> + Buzzer/Speaker - -> ground

PIN 6 -> 500 ohm resistor -> + REDLED - -> Ground
PIN 7 -> 500 ohm resistor -> + BLUELED - -> Ground
PIN 8 -> 500 ohm resistor -> + GREENLED - -> Ground

PIN 9 -> Relay Switch IN 1
PIN 10 -> Relay Switch IN 2
PIN 11 -> Relay Switch IN 3

5v out -> Garage Door Opener Remote Vin (battery +)


## Power
12v + -> Arduino Vin
12v + -> Weigand Keypad Vin

Ground -> Arduino Gnd
Ground -> Weigand Keypad Ground
Ground -> Garay Door Opener Remote Ground (battery -)

## Relay
Relay set to: Trigger High
Normally Open circuit closes the garage door remote button

