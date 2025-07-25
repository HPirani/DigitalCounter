#include <Arduino.h>
#line 1 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
int data = 2;
int clock = 3;

byte zero = B00000001;
byte one = B00000010;
byte two = B00000100;
byte three = B00100000;
byte four = B00100000;
byte five = B01000000;
byte six = B01000000;
byte seven = B10000000;
byte eight = B11111110;
byte nine = B11111111;

static const uint8_t digits[29] = {
    0b00111111, // 0
    0b00110000, // 10b00100001
    0b11011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00100111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110110, // H
    0b01110011, // P
    0b00110000, // i
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b10000000,
    0b00000011,
    0b00000111,
    0b00001111,
    0b00011111,
    0b00111111,
    0b01111111,
    0b11111111, // 25
    0b000000001,
    0b0000000001,
    0b00000000001};
String input = "";
int substringData = 0;
int substringchar = 36;

#line 49 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void setup();
#line 58 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void loop();
#line 49 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void setup()
{
    Serial.begin(9600);

    pinMode(data, OUTPUT);

    pinMode(clock, OUTPUT);
}

void loop()
{
    if (Serial.available())
    {
        input = Serial.readStringUntil('\n');
        substringData = input.substring(0, 3).toInt();
        substringchar = input.substring(0).toInt();
        delay(1);
        Serial.print("data:   ");
        //Serial.print(substringData);
       // Serial.print(" ,   ");
        Serial.println(substringchar);
        // if (substringData > 0)
        // {

        //}
    
    shiftOut(data, clock, LSBFIRST, digits[substringchar]);
    delay(500);
    }
}
