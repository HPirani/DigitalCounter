# 1 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
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

** Modified In sat 1404/05/04 07:20 PM To 01:30 AM by me.                        **

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
# 23 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
# 24 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino" 2
# 25 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino" 2

// pin defs
constexpr auto data = 2;
constexpr auto clock = 3;



constexpr auto seg1 = 5; // using common cathode 7segments
constexpr auto seg2 = 6;
constexpr auto seg3 = 7;
constexpr auto seg4 = 8;
constexpr auto seg5 = 9;
constexpr auto seg6 = 10;

//#if defined(__AVR_ATmega8__) // main chip
constexpr auto reset = 11;
constexpr auto senseIn = 4;
/* #elif defined (__AVR_ATmega328P__) || defined (__ARDUINO_NANO__) //Arduino Nano, for debug

constexpr auto reset = 11;

constexpr auto senseIn = 4;

#endif */
# 47 "H:\\Arduino\\projects\\DigitalCounter\\DigitalCounter.ino"
//EEPROM
constexpr auto eep_addr = 0;

long count = 0;
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
int substringData = 0;
int substringchar = 36;

bool bisSensLow = false; // Sensor Pin Is LOW.
//bool bisSensHigh = false; // Sensor pin Is High.
//bool bisChanged  = false; //

void setup()
{
    //debug
    Serial.begin(9600);

    //ISR
    attachInterrupt(((senseIn) == 2 ? 0 : ((senseIn) == 3 ? 1 : -1)), ReadSensor, 1);
    attachInterrupt(((reset) == 2 ? 0 : ((reset) == 3 ? 1 : -1)), ResetCounter, 3);

    //74hc164
    pinMode(data, 0x1);
    pinMode(clock, 0x1);

    pinMode(seg1,0x1);
    pinMode(seg2, 0x1);
    pinMode(seg3, 0x1);
    pinMode(seg4, 0x1);
    pinMode(seg5, 0x1);
    pinMode(seg6, 0x1);

    pinMode(senseIn, 0x0);

    pinMode(reset,0x0);

    // Turn Of All Segments
   zeroAllSegments();
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

    shiftOut(data, clock, 0, digits[substringchar]);
    delay(500);
    }
    //
    // sensor Readings
    displayNumber();

}

//ISR functions
void ReadSensor()
{
    if((digitalRead(senseIn)) == 0x1)
    {
      if(bisSensLow)
      {
        count++;
        bisSensLow = false;
      }
    }
    else
    {
        bisSensLow = true;
    }
}

void ResetCounter()
{
    for (unsigned int i = eep_addr; i < sizeof(long); ++i)
    {
        EEPROM.write(i, 0);
        count = 0;
    }
}

/// @brief Display Numbers On 7-Segments.
void displayNumber()
{
strnum = count;
int numLengh = strnum.length() - 1;
for(int i = 0; i < numLengh; ++i)
{
    uint8_t number = strnum.substring(i,i + 1).toInt();
    displayDigitOnSegment( i, number);
}

}

void DisplaySplash()
{
    //for(int i = 0; i >= 3; ++i)
   // {
        for (int i = 0; i >= 60; ++i)
        {

            shiftOut(data, clock, 0, digits[10]); // H
            digitalWrite(seg1, 0x1);
            digitalWrite(seg2, 0x0);
            digitalWrite(seg3, 0x0);
            digitalWrite(seg4, 0x0);
            digitalWrite(seg5, 0x0);
            digitalWrite(seg6, 0x0);

            shiftOut(data, clock, 0, digits[11]); // P
            digitalWrite(seg2, 0x1);
            digitalWrite(seg1, 0x0);
            digitalWrite(seg3, 0x0);
            digitalWrite(seg4, 0x0);
            digitalWrite(seg5, 0x0);
            digitalWrite(seg6, 0x0);
            shiftOut(data, clock, 0, digits[12]); // i
            digitalWrite(seg3, 0x1);
            digitalWrite(seg2, 0x0);
            digitalWrite(seg1, 0x0);
            digitalWrite(seg4, 0x0);
            digitalWrite(seg5, 0x0);
            digitalWrite(seg6, 0x0);
        }
    //}
}

void displayDigitOnSegment(int segNum,uint8_t num)
{
    // First we need Toi Shift out our number.
    // and just need one time per draw.
    shiftOut(data, clock, 0, digits[num]);

    switch (segNum)
    {
    case 1:
        // we Need To turn Off Other Segs When Displaying number on a segment.
        digitalWrite(seg1,0x1);

        digitalWrite(seg2,0x0);
        digitalWrite(seg3,0x0);
        digitalWrite(seg4, 0x0);
        digitalWrite(seg5, 0x0);
        digitalWrite(seg6, 0x0);
        break;
        case 2:
            digitalWrite(seg2, 0x1);

            digitalWrite(seg1, 0x0);
            digitalWrite(seg3, 0x0);
            digitalWrite(seg4, 0x0);
            digitalWrite(seg5, 0x0);
            digitalWrite(seg6, 0x0);
            break;
        case 3:
            digitalWrite(seg3, 0x1);

            digitalWrite(seg1, 0x0);
            digitalWrite(seg2, 0x0);
            digitalWrite(seg4, 0x0);
            digitalWrite(seg5, 0x0);
            digitalWrite(seg6, 0x0);
            break;
        case 4:
            digitalWrite(seg4, 0x1);

            digitalWrite(seg2, 0x0);
            digitalWrite(seg3, 0x0);
            digitalWrite(seg1, 0x0);
            digitalWrite(seg5, 0x0);
            digitalWrite(seg6, 0x0);
            break;
        case 5:
            digitalWrite(seg5, 0x1);

            digitalWrite(seg2, 0x0);
            digitalWrite(seg3, 0x0);
            digitalWrite(seg4, 0x0);
            digitalWrite(seg1, 0x0);
            digitalWrite(seg6, 0x0);
            break;
        case 6:
            digitalWrite(seg6, 0x1);

            digitalWrite(seg2, 0x0);
            digitalWrite(seg3, 0x0);
            digitalWrite(seg4, 0x0);
            digitalWrite(seg5, 0x0);
            digitalWrite(seg1, 0x0);
            break;


    default:
        break;
    }

}

void zeroAllSegments()
{
    digitalWrite(seg1, 0x0);
    digitalWrite(seg2, 0x0);
    digitalWrite(seg3, 0x0);
    digitalWrite(seg4, 0x0);
    digitalWrite(seg5, 0x0);
    digitalWrite(seg6, 0x0);
    // clear Shift Register.
    shiftOut(data, clock, 0, digits[0]);

    delay(50); // A bit Delay.
}
