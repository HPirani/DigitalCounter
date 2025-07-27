#line 1 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
/**********************************************************************************
**                             HPi Source File                                   **
**    Copyright (C) 2020-2025 HPiStudio. All rights Reserved.                    **
** ********************************************************************************
** this code is part of Soldier(WorkingTitle)                                    **
** Description:                                                                  **
**            Industrial Digital counter.                                        **
**            With AVR Atmega8 ande proximity sensor(PR12_4DN,BRN10_30VDC,NPN    **
** Created in fri 1404/05/03 01:39 PM By Hosein Pirani                           **
**                                                                               **
** Modified In sat 1404/05/04 07:20 PM To 10:40 PM by me.                        **
** :	                                           								 **
** TODO:                                                                         **
** TODO:                                                                         **
** ..  :                                                                         **
** ...                                                                           **
** .... Code                                                                     **
** ..... Code?																	 **
** ........ Code!                                                                **
** ........... More Code                                                         **
** ...............  #_#                                                          **
*********************************************************************************/
#include <Arduino.h>
#include <EEPROM.h>

// pin defs
constexpr auto data = 10;
constexpr auto clock = 11;



constexpr auto seg1 = 4; // using common cathode 7segments
constexpr auto seg2 = 5;
constexpr auto seg3 = 6;
constexpr auto seg4 = 7;
constexpr auto seg5 = 8;
constexpr auto seg6 = 9;

//#if defined(__AVR_ATmega8__) // main chip
constexpr auto reset = 2; //mega8: int0 = 2 PD2 - same for nano!
constexpr auto senseIn = 3; // mega8: int1 = 3 PD3 - same for nano!
/* #elif defined (__AVR_ATmega328P__) || defined (__ARDUINO_NANO__) //Arduino Nano, for debug
constexpr auto reset = 11;
constexpr auto senseIn = 4;
#endif */

//EEPROM
constexpr auto eep_addr = 0;

volatile unsigned long count = 0;
long prevCount = 0;
unsigned long delayTime = 7; // delay time for display
bool bisMicroDelay = false; // for micro delay, to prevent flickering on display.
String strnum;

//7Segment digits, for 74hc164
static const uint8_t digits[13] = {
    0b00111111, // 0
    0b00110000, // 1
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
    0b00110000 // i
};

// debug
String input = "";
long substringData = 0;
long substringchar = 0;

volatile bool bisSensLow = false; // Sensor Pin Is LOW.
//bool bisSensHigh = false; // Sensor pin Is High.
//bool bisChanged  = false; //

#line 82 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void setup();
#line 124 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void loop();
#line 159 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void adjustDelayTime();
#line 186 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void ReadSensor();
#line 205 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void ResetCounter();
#line 216 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void displayNumber();
#line 236 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void DisplaySplash();
#line 269 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void displayDigitOnSegment(int segNum,uint8_t num);
#line 358 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void zeroAllSegments();
#line 372 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void saveDataToEEPROM();
#line 82 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
void setup()
{
    //debug
    Serial.begin(9600);

    //ISR
    attachInterrupt(digitalPinToInterrupt(senseIn), ReadSensor, CHANGE);
    attachInterrupt(digitalPinToInterrupt(reset), ResetCounter, CHANGE);

    //74hc164
    pinMode(data, OUTPUT);
    pinMode(clock, OUTPUT);

    pinMode(seg1,OUTPUT);
    pinMode(seg2, OUTPUT);
    pinMode(seg3, OUTPUT);
    pinMode(seg4, OUTPUT);
    pinMode(seg5, OUTPUT);
    pinMode(seg6, OUTPUT);

    pinMode(senseIn, INPUT);

    pinMode(reset,INPUT);

    // Turn Of All Segments
  // zeroAllSegments();
    digitalWrite(seg3, HIGH);
    digitalWrite(seg1, HIGH);
    digitalWrite(seg2, HIGH);
    digitalWrite(seg4, HIGH);
    digitalWrite(seg5, HIGH);
    digitalWrite(seg6, HIGH);

    DisplaySplash();

    //REad Saved Count From EEPROM
    long i; 
    EEPROM.get(eep_addr, i);
    if(i > 0)
        count = i;
}

void loop()
{
    // debug
    if (Serial.available())
    {
        input = Serial.readStringUntil('\n');
        substringData = input.substring(0).toInt();
        substringchar = input.substring(2).toInt();
        delay(1);
        Serial.print("data:   ");
        Serial.print(substringData);
        Serial.print(" ,   ");
        Serial.println(substringchar);
        // if (substringData > 0)
        // {

        //}
       // displayDigitOnSegment(substringchar, substringData);
        count = substringData;
        // shiftOut(data, clock, LSBFIRST, digits[11]);
        //delay(5);
    }
    //displayDigitOnSegment(substringchar, substringData);
    //
    // sensor Readings
   // if (prevCount != count)
    //{
       // prevCount = count;
       adjustDelayTime();
        displayNumber();
   // }
    
    saveDataToEEPROM();
}
// Adjust delay time based on count value
void adjustDelayTime()
{
    if (count > 99)
    {
        bisMicroDelay = false;
        delayTime = 7; // 13 ms for single digit
    }
    else if (count > 999)
    {
        bisMicroDelay = true;
        //delayTime = 10; // 20 ms for double digits
    }
    else if (count > 9999)
    {
        bisMicroDelay = true; // for micro delay, to prevent flickering on display.
       // delayTime = 1; // 30 ms for triple digits
    }
    else if (count < 10)
    {
        bisMicroDelay = false;
        delayTime = 11; // 50 ms for four digits or more
    }
   // Serial.print("Delay Time: ");
   // Serial.println(delayTime);
}

//ISR functions
void ReadSensor()
{
    //Serial.print("sensor... ");
    if((digitalRead(senseIn)) == HIGH)
    {
      if(bisSensLow)
      {
          Serial.println("adding... ");
          count++;
          bisSensLow = false;
      }
    }
    else
    {
        Serial.println("setting to low... ");
        bisSensLow = true;
    }
}

void ResetCounter()
{
    Serial.println("reset eep... ");
    for (unsigned int i = eep_addr; i < sizeof(long); ++i)
    {
        EEPROM.write(i, 0);
        count = 0;
    }
}

/// @brief Display Numbers On 7-Segments.
void displayNumber()
{
strnum = String(count);
int numLengh = strnum.length() - 1;
/* Serial.print("strnum: ");
Serial.println(strnum);
Serial.print("numLengh: ");
Serial.println(numLengh); */
for(int i = 0; i <= numLengh; ++i)
{
    uint8_t number = strnum.substring(i,i + 1).toInt();
    /* Serial.print("Displaying: ");
    Serial.println(number);
    Serial.print("On Segment: ");
    Serial.println(i); */
    displayDigitOnSegment( i+1,  number);
}

}

void DisplaySplash()
{
    //for(int i = 0; i >= 3; ++i)
   // {
   
   
    //Serial.print("drawing... ");
    for (int i = 0; i < 260; ++i)
    {
        //delay(1);
        for (int j = 2; j < 5; ++j)
        {
            //Serial.print("Displaying: ");
            //Serial.print(j);
            //Serial.print(" , ");
            //Serial.println(8 + j);
            //delay(1);
            displayDigitOnSegment(j, 8 + j);
            //delay(1);
        }

        }
        shiftOut(data, clock, LSBFIRST, digits[0]);
        digitalWrite(seg1, LOW);

        digitalWrite(seg2, HIGH);
        digitalWrite(seg3, HIGH);
        digitalWrite(seg4, HIGH);
        digitalWrite(seg5, HIGH);
        digitalWrite(seg6, HIGH);
        //}
}

void displayDigitOnSegment(int segNum,uint8_t num)
{
    // First we need To Shift out our number.
    // and just need one time per draw.
    if (bisMicroDelay == true)
    {
      // delayMicroseconds(delayTime);
    }else
    {
    delay(delayTime); // A bit Delay.
    }
    shiftOut(data, clock, LSBFIRST, digits[num]);
    
    switch (segNum)
    {
    case 1:
        // we Need To turn Off Other Segs When Displaying number on a segment.
        digitalWrite(seg2, HIGH);
        digitalWrite(seg3, HIGH);
        digitalWrite(seg4, HIGH);
        digitalWrite(seg5, HIGH);
        digitalWrite(seg6, HIGH);

        digitalWrite(seg1,LOW);
        //delay(1);
        break;
        case 2:
            digitalWrite(seg1, HIGH);
            digitalWrite(seg3, HIGH);
            digitalWrite(seg4, HIGH);
            digitalWrite(seg5, HIGH);
            digitalWrite(seg6, HIGH);

            digitalWrite(seg2, LOW);
           // delay(1);
            break;
        case 3:

            digitalWrite(seg1, HIGH);
            digitalWrite(seg2, HIGH);
            digitalWrite(seg4, HIGH);
            digitalWrite(seg5, HIGH);
            digitalWrite(seg6, HIGH);

            digitalWrite(seg3, LOW);
            //delay(1);

            break;
        case 4:
            digitalWrite(seg2, HIGH);
            digitalWrite(seg3, HIGH);
            digitalWrite(seg1, HIGH);
            digitalWrite(seg5, HIGH);
            digitalWrite(seg6, HIGH);

            digitalWrite(seg4, LOW);
            //delay(1);

            break;
        case 5:

            digitalWrite(seg2, HIGH);
            digitalWrite(seg3, HIGH);
            digitalWrite(seg4, HIGH);
            digitalWrite(seg1, HIGH);
            digitalWrite(seg6, HIGH);

            digitalWrite(seg5, LOW);
           // delay(1);
            break;
        case 6:

            digitalWrite(seg2, HIGH);
            digitalWrite(seg3, HIGH);
            digitalWrite(seg4, HIGH);
            digitalWrite(seg5, HIGH);
            digitalWrite(seg1, HIGH);

            digitalWrite(seg6, LOW);
            //delay(1);
            break;
        
    
    default:
        break;
    }

}

void zeroAllSegments()
{
    digitalWrite(seg1, LOW);
    digitalWrite(seg2, LOW);
    digitalWrite(seg3, LOW);
    digitalWrite(seg4, LOW);
    digitalWrite(seg5, LOW);
    digitalWrite(seg6, LOW);
    // clear Shift Register.
    shiftOut(data, clock, LSBFIRST, digits[0]);

    delay(10); // A bit Delay.
}

void saveDataToEEPROM()
{
    //first, Check If count is Updated, To Prevent From EEPROM
    // Save Count To EEPROM.
    long prevdata =0;
     EEPROM.get(eep_addr,prevdata);

     if(count != prevdata)
    EEPROM.put(eep_addr, count);
}
