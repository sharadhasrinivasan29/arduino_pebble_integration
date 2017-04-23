/***************************************************************************
Copyright 2008 Gravitech
All Rights Reserved
****************************************************************************/

/***************************************************************************
File Name: I2C_7SEG_Temperature.pde
Hardware: Arduino Diecimila with 7-SEG Shield
Description:
This program reads I2C data from digital thermometer and display it on 7-Segment
Change History:
03 February 2008, Gravitech - Created
****************************************************************************/

#include <Wire.h>
#include <math.h>
#include <limits.h>

#define BAUD (9600)    /* Serial baud define */
#define _7SEG (0x38)   /* I2C address for 7-Segment */
#define THERM (0x49)   /* I2C address for digital thermometer */
#define EEP (0x50)     /* I2C address for EEPROM */
#define RED (3)        /* Red color pin of RGB LED */
#define GREEN (5)      /* Green color pin of RGB LED */
#define BLUE (6)       /* Blue color pin of RGB LED */

#define COLD (23)      /* Cold temperature, drive blue LED (23c) */
#define HOT (26)       /* Hot temperature, drive red LED (27c) */

const byte NumberLookup[16] =   {0x3F,0x06,0x5B,0x4F,0x66,
  0x6D,0x7D,0x07,0x7F,0x6F,
  0x77,0x7C,0x39,0x5E,0x79,0x71};
  // 0 1 2 3 4 5 6 7 8 9 A B C D E F

  /* Function prototypes */
  void convert_c_to_f (int&, byte&, bool&, int, byte, bool&);
  void Cal_temp (int&, byte&, byte&, bool&);
  void Dis_7SEG (int, byte, byte, bool);
  void Send7SEG (byte, byte);
  void SerialMonitorPrint (byte, int, bool, bool);
  void UpdateRGB (byte);
  int incomingByte;

  /***************************************************************************
  Function Name: setup
  Purpose:
  Initialize hardwares.
  ****************************************************************************/

  void setup()
  {
    Serial.begin(BAUD);
    Wire.begin();        /* Join I2C bus */
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(500);          /* Allow system to stabilize */
  }

  /***************************************************************************
  Function Name: loop
  Purpose:
  Run-time forever loop.
  ****************************************************************************/

  void loop()
  {
    String msg = ""; // DC
    int Decimal;
    byte Temperature_H, Temperature_L, counter, counter2;
    bool IsPositive;
    bool stop = 0;

    // DC statuses
    bool showing_trend = false; // feature 1
    bool writing_word = false; // feature 2
    bool blinking = false; // feature 3

    // DC temperature conversion
    bool celsius = true;
    bool standby = false;
    bool f_IsPositive = 0;
    int f_decimal = 0;
    byte f_temperature_H = 0;
    byte f_temperature_L = 0;

    // DC storing temperature data
    int array_index = 0;
    byte temperature_stats[3600][2];
    int max_temp_h, max_temp_dec, min_temp_h, min_temp_dec, avg_temp_h, avg_temp_dec;



    /* Configure 7-Segment to 12mA segment output current, Dynamic mode,
    and Digits 1, 2, 3 AND 4 are NOT blanked */

    Wire.beginTransmission(_7SEG);
    byte val = 0;
    Wire.write(val);
    val = B01000111;
    Wire.write(val);
    Wire.endTransmission();

    /* Setup configuration register 12-bit */

    Wire.beginTransmission(THERM);
    val = 1;
    Wire.write(val);
    val = B01100000;
    Wire.write(val);
    Wire.endTransmission();

    /* Setup Digital THERMometer pointer register to 0 */

    Wire.beginTransmission(THERM);
    val = 0;
    Wire.write(val);
    Wire.endTransmission();

    /* Test 7-Segment */
    for (counter=0; counter<8; counter++)
    {
      Wire.beginTransmission(_7SEG);
      Wire.write(1);
      for (counter2=0; counter2<4; counter2++)
      {
        Wire.write(1<<counter);
      }
      Wire.endTransmission();
      delay (250);
    }

    while (1)
    {
      Wire.requestFrom(THERM, 2);
      Temperature_H = Wire.read();
      Temperature_L = Wire.read();

      /* Calculate temperature */
      Cal_temp (Decimal, Temperature_H, Temperature_L, IsPositive);
      convert_c_to_f(f_decimal, f_temperature_H, f_IsPositive, Decimal, Temperature_H, IsPositive);

      // DC : recording temperature every second
      int index = array_index % 3600;
      // populate array
      temperature_stats[index][0] = Temperature_H;
      temperature_stats[index][1] = Decimal;
      // statistics
      int j = 0;
      // determine how long it's been
      if(array_index < 3600) {
        // has not been over an hour
        j = array_index;
      } else {
        j = 3600;
      }
      // compute statistics
      max_temp_h = INT_MIN;
      max_temp_dec = INT_MIN;
      min_temp_h = INT_MAX;
      min_temp_dec = INT_MAX;
      double current_temp = Temperature_H + ((double) Decimal / 100);
      double max_temp = -1000;
      double min_temp = 1000;
      avg_temp_h = 0;
      avg_temp_dec = 0;

      for (int i = 0; i < j; i++) {
        if (current_temp < min_temp) {
          min_temp = current_temp;
          min_temp_h = temperature_stats[i][0];
          min_temp_dec = temperature_stats[i][1];
        }
        if (current_temp > max_temp) {
          max_temp = current_temp;
          max_temp_h = temperature_stats[i][0];
          max_temp_dec = temperature_stats[i][1];
        }
        avg_temp_h += temperature_stats[i][0];
        avg_temp_dec += temperature_stats[i][1];
      }
      if (avg_temp_dec >= 100) {
        avg_temp_h += (avg_temp_dec / 100);
        avg_temp_dec = (avg_temp_dec % 100);
      }
      avg_temp_h = (avg_temp_h / j);
      avg_temp_dec = (avg_temp_dec / j);

      Serial.print(" max  : ");
      Serial.print(max_temp_h);
      Serial.print(".");
      Serial.println(max_temp_dec); 

      Serial.print(" min  : ");
      Serial.print(min_temp_h);
      Serial.print(".");
      Serial.println(min_temp_dec); 

      Serial.print(" avg  : ");
      Serial.print(avg_temp_h);
      Serial.print(".");
      Serial.println(avg_temp_dec); 

      // update index
      array_index++;

      /* Display temperature on the serial monitor.
      Comment out this line if you don't use serial monitor.*/
      if (stop == 0) {
        if (celsius == true && showing_trend == false) {
          SerialMonitorPrint (Temperature_H, Decimal, IsPositive, true);
        }
        else if (celsius == false && showing_trend == false) {
          SerialMonitorPrint(f_temperature_H, f_decimal, f_IsPositive, false);
        }
      }

      /* Update RGB LED.*/
      if (blinking == false) {
        UpdateRGB (Temperature_H);
      } else {
        // turn off all lights if we want the blinking feature
        // note : will still blink a mixed color for some time
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, LOW);
        digitalWrite(BLUE, LOW);
      }

      if (Serial.available() > 0) {
        // handle commands from server
        //        incomingByte = Serial.read(); // DC commented out
        msg = Serial.readString();

        if (msg.equals("02")) {
          celsius = true;
          writing_word = false;
          blinking = false;
          showing_trend = false;
          Dis_7SEG (Decimal, Temperature_H, Temperature_L, IsPositive, 1);
          SerialMonitorPrint (Temperature_H, Decimal, IsPositive, true);
        }
        if (msg.equals("01")){
          celsius = false;
          writing_word = false;
          blinking = false;
          showing_trend = false;
          Dis_7SEG(f_decimal * 10, f_temperature_H, f_temperature_L, f_IsPositive, 0);
          SerialMonitorPrint(f_temperature_H, f_decimal, f_IsPositive, false);
        }
        if (msg.equals("03")) { // stats : avg
          // avg temp
          writing_word = false;
          blinking = false;
          showing_trend = false;
          if (celsius) {
            Serial.print(avg_temp_h, DEC);
            Serial.print(".");
            if (avg_temp_dec < 10) {
              Serial.print(0, DEC);
            }
            Serial.print(avg_temp_dec, DEC);
            Serial.println(" C.");
          } else {
            int convert_decimal;
            byte convert_high;
            bool convert_sign;
            convert_c_to_f (convert_decimal, convert_high, convert_sign, avg_temp_dec, avg_temp_h, IsPositive);
            Serial.print(convert_high, DEC);
            Serial.print(".");
            if (convert_decimal < 10) {
              Serial.print(0, DEC);
            }
            Serial.print(convert_decimal, DEC);
            Serial.println(" F.");
          }

        }
        if (msg.equals("04")) { // stats : max
          writing_word = false;
          blinking = false;
          showing_trend = false;
          if (celsius) {
            Serial.print("max : ");
            Serial.print(max_temp_h, DEC);
            Serial.print(".");
            Serial.print(max_temp_dec, DEC);
            Serial.println(" C.");
          } else {
            int convert_decimal;
            byte convert_high;
            bool convert_sign;

            convert_c_to_f (convert_decimal, convert_high, convert_sign, max_temp_dec, max_temp_h, IsPositive);
            Serial.print("max : ");
            Serial.print(convert_high, DEC);
            Serial.print(".");
            if (convert_decimal < 10) {
              Serial.print(0, DEC);
            }
            Serial.print(convert_decimal, DEC);
            Serial.println(" F.");
          }
        }
        if (msg.equals("05")) { // stats : min
          writing_word = false;
          blinking = false;
          showing_trend = false;
          if (celsius) {
            Serial.print("min : ");
            Serial.print(min_temp_h, DEC);
            Serial.print(".");
            Serial.print(min_temp_dec, DEC);
            Serial.println(" C.");
          } else {
            int convert_decimal;
            byte convert_high;
            bool convert_sign;

            convert_c_to_f (convert_decimal, convert_high, convert_sign, min_temp_dec, min_temp_h, IsPositive);
            Serial.print("min : ");
            Serial.print(convert_high, DEC);
            Serial.print(".");
            if (convert_decimal < 10) {
              Serial.print(0, DEC);
            }
            Serial.print(convert_decimal, DEC);
            Serial.println(" F.");
          }
        }
        if (msg.equals("06")) { // standby
          standby = true;
          blinking = false;
          writing_word = false;
          showing_trend = false;
          for (counter = 1; counter <= 4; counter++) {
            Send7SEG(counter,0x40); // print ----
          }
          Serial.print("--- standby ---");
          delay(1000);
        }
        
        if (msg.equals("13")) { // resume from standby
          // resume from standby
          standby = false;
          writing_word = false;
          celsius = true;
          blinking = false;
          showing_trend = false;
          Dis_7SEG (Decimal, Temperature_H, Temperature_L, IsPositive, 1);
          SerialMonitorPrint (Temperature_H, Decimal, IsPositive, true);
        }

        if (msg.equals("07")) { // feature 1 : trend
          writing_word = false;
          blinking = false;
          showing_trend = true;
          long xsum;
          long ysum;
          long xxsum;
          long xysum;
          double mNumerator;
          double mDenominator;
          double m;

          //calculate summations over the past hour
          for (int i = 0; i < j; i++){
            ysum = ysum + temperature_stats[i][0];
            xsum = xsum + i;
            xysum = xysum + (i * temperature_stats[index][0]);
            xxsum = xxsum + (i*i);
          }

          //calculate slope of regression
          mNumerator = (j * xysum) - (xsum * ysum);
          mDenominator = (j * xxsum) - (xsum * xsum);
          m = mNumerator / mDenominator;

          //send trend to computer
          if (m > 0){
            Serial.println("The temperature trend is increasing.");
          } else if (m < 0){
            Serial.println("The temperature trend is decreasing.");
          } else {
            Serial.println("The temperature has remained constant.");
          }
        }

        if (msg.equals("08")) { // feature 2 word 1 (Oops)
          writing_word = true;
          showing_trend = false;
          Word_7SEG(0, 1, 2, 3);
          //          writing_word = true;
          //          showing_trend = false;
          //          Word_7SEG(4, 5, 6, 0); //  Hello
          //          writing_word = true;
          //          blinking = false;
          //          showing_trend = false;
          //          Word_7SEG(4, 0, 11, 10); //  HOLA
        }

        if (msg.equals("09")) { // feature 3 blink green
          writing_word = false;
          blinking = true;
          showing_trend = false;
          digitalWrite(GREEN, HIGH);
          delay(1000);                       // wait for a second
          digitalWrite(GREEN, LOW);
          delay(1000);                       // wait for a second
        }

        if (msg.equals("10")) { // feature 3 blink blue
          writing_word = false;
          blinking = true;
          showing_trend = false;
          digitalWrite(BLUE, HIGH);
          delay(1000);                       // wait for a second
          digitalWrite(BLUE, LOW);
          delay(1000);                       // wait for a second
        }


        if (msg.equals("11")) { // feature 3 blink red
          writing_word = false;
          blinking = true;
          showing_trend = false;
          digitalWrite(RED, HIGH);
          delay(1000);                       // wait for a second
          digitalWrite(RED, LOW);
          delay(1000);                       // wait for a second
        }

        if (msg.equals("12")) { // feature 4
          // TODO drink recommendation
        }
        
        byte tooHot = -1;
        byte tooCold = -1;

        if (incomingByte == 9999){ // TODO : feature 4 35 degrees too hot
          showing_trend = false;
          writing_word = false;
          blinking = false;
          tooHot = 35;
        }

        if (incomingByte == 9999){ // TODO: feature 4 30 degrees too hot
          showing_trend = false;
          writing_word = false;
          blinking = false;
          tooHot = 30;
        }

        if (incomingByte == 9999){ // TODO: feature 4 25 degrees too hot
          writing_word = false;
          showing_trend = false;
          blinking = false;
          tooHot = 25;

        }

        if (incomingByte == 9999) { // TODO: feature 4 0 is too cold
          showing_trend = false;
          blinking = false;
          writing_word = false;
          tooCold = 0;
        }

        if (incomingByte == 9999 ){ // TODO: feature 4 5 degrees is too cold
          showing_trend = false;
          writing_word = false;
          blinking = false;
          tooCold = 5;
        }

        if (incomingByte == 9999){ // TODO : feature 4 10 degrees is too cold
          showing_trend = false;
          writing_word = false;
          blinking = false;
          tooCold = 10;
        }

        if (incomingByte == 9990){ // TODO : feature 4 asking for a drink 9990 is "thirsty" option
        showing_trend = false;
        writing_word = false;
        blinking = false;
        if (tooHot == -1 || tooCold == -1) {
          // error handling
          Serial.print("No preference found.");
        }
        if (Temperature_H >= tooHot){
          Serial.print("Have an iced tea!");
        } else if (Temperature_H <= tooCold){
          Serial.print("Have a hot cocoa!");
        } else {
          Serial.print("Have a glass of water!");
        }
      }
    }

    /* Display temperature on the 7-Segment */
    if (standby == true) {
      for (counter = 1; counter <= 4; counter++) {
        Send7SEG(counter,0x40); // print ----
        //          Wire.endTransmission();
      }
      delay(1000);
    }

    if (celsius == true && writing_word == false) { // DC : display in C
      Dis_7SEG (Decimal, Temperature_H, Temperature_L, IsPositive, 1);
    } else if (celsius == false && writing_word == false) { // DC: display in F
      Dis_7SEG(f_decimal * 10, f_temperature_H, f_temperature_L, f_IsPositive, 0);
    }

    delay (1000);        /* Take temperature read every 1 second */
  }
}

/***************************************************************************
Function Name: Word_7SEG
Purpose:
Display word on the 7-segment display.
****************************************************************************/
void Word_7SEG (byte letterOne, byte letterTwo, byte letterThree, byte letterFour)
{

  const byte alphabet[12]= {
    0x3F, // O 0
    0x5C, // o 1
    0x73, // P 2
    0x6D, // S 3
    0x76, // H 4
    0x79, // E 5
    0x36, // ll 6
    0x3D, // G 7
    0x7F, // B 8
    0x6E, // Y 9
    0x77, // A 10
    0x38, // L 11
  };
  byte Digit = 4;                 /* Number of 7-Segment digit */
  /* Temporary variable hold the number to display */


  Send7SEG (Digit,alphabet[letterOne]);     /* Display on the 7-Segment */
  Digit--;                      /* Subtract 1 digit */

  Send7SEG (Digit,alphabet[letterTwo]);     /* Display on the 7-Segment */
  Digit--;                      /* Subtract 1 digit */

  Send7SEG (Digit,alphabet[letterThree]);     /* Display on the 7-Segment */
  Digit--;                      /* Subtract 1 digit */

  Send7SEG (Digit,alphabet[letterFour]);     /* Display on the 7-Segment */
  Digit--;                      /* Subtract 1 digit */


  if (Digit > 0)                 /* Clear the rest of the digit */
  {
    Send7SEG (Digit,0x00);
  }

}


/***************************************************************************
Function Name: convert_c_to_f
// DC
Purpose:
Calculate temperature from raw data.
****************************************************************************/
void convert_c_to_f (int& f_decimal, byte& f_high, bool& f_sign, int c_decimal, byte c_high, bool& c_sign) {
  double deg_c, deg_f;
  deg_c = c_high + ((double) c_decimal / 1000.0);
  if (c_sign == 0) { // negative temperature
    deg_c = - deg_c;
  }
  deg_f = (9.0 / 5.0) * deg_c + 32.0;
  if (deg_f < 0) {
    f_sign = 0; // negative f temp
  } else {
    f_sign = 1;
  }
  f_high = (int) deg_f;
  f_decimal = (int) ((deg_f - f_high) * 1000);
}

/***************************************************************************
Function Name: Cal_temp
Purpose:
Calculate temperature from raw data.
****************************************************************************/
void Cal_temp (int& Decimal, byte& High, byte& Low, bool& sign)
{
  if ((High&B10000000)==0x80)    /* Check for negative temperature. */
  sign = 0;
  else
  sign = 1;

  High = High & B01111111;      /* Remove sign bit */
  Low = Low & B11110000;        /* Remove last 4 bits */
  Low = Low >> 4;
  Decimal = Low;
  Decimal = Decimal * 625;      /* Each bit = 0.0625 degree C */

  if (sign == 0)                /* if temperature is negative */
  {
    High = High ^ B01111111;    /* Complement all of the bits, except the MSB */
    Decimal = Decimal ^ 0xFF;   /* Complement all of the bits */
  }
}

/***************************************************************************
Function Name: Dis_7SEG
Purpose:
Display number on the 7-segment display.
****************************************************************************/
void Dis_7SEG (int Decimal, byte High, byte Low, bool sign, bool celsius) // DC : added a bool parameter
{
  byte Digit = 4;                 /* Number of 7-Segment digit */
  byte Number;                    /* Temporary variable hold the number to display */

  if (sign == 0)                  /* When the temperature is negative */
  {
    Send7SEG(Digit,0x40);         /* Display "-" sign */
    Digit--;                      /* Decrement number of digit */
  }

  if (High > 99)                  /* When the temperature is three digits long */
  {
    Number = High / 100;          /* Get the hundredth digit */
    Send7SEG (Digit,NumberLookup[Number]);     /* Display on the 7-Segment */
    High = High % 100;            /* Remove the hundredth digit from the TempHi */
    Digit--;                      /* Subtract 1 digit */
  }

  if (High > 9)
  {
    Number = High / 10;           /* Get the tenth digit */
    Send7SEG (Digit,NumberLookup[Number]);     /* Display on the 7-Segment */
    High = High % 10;            /* Remove the tenth digit from the TempHi */
    Digit--;                      /* Subtract 1 digit */
  }

  Number = High;                  /* Display the last digit */
  Number = NumberLookup [Number];
  if (Digit > 1)                  /* Display "." if it is not the last digit on 7-SEG */
  {
    Number = Number | B10000000;
  }
  Send7SEG (Digit,Number);
  Digit--;                        /* Subtract 1 digit */

  if (Digit > 0)                  /* Display decimal point if there is more space on 7-SEG */
  {
    Number = Decimal / 1000;
    Send7SEG (Digit,NumberLookup[Number]);
    Digit--;
  }

  //  if(incomingByte == 49) {
  if (Digit > 0)                 /* Display "c" if there is more space on 7-SEG */
  {
    if (celsius == 0) {
      Send7SEG (Digit,0x71);
    } else {
      Send7SEG (Digit,0x58);
    }
    Digit--;
  }
  //  }


  if (Digit > 0)                 /* Clear the rest of the digit */
  {
    Send7SEG (Digit,0x00);
  }

}

/***************************************************************************
Function Name: Send7SEG
Purpose:
Send I2C commands to drive 7-segment display.
****************************************************************************/

void Send7SEG (byte Digit, byte Number)
{
  Wire.beginTransmission(_7SEG);
  Wire.write(Digit);
  Wire.write(Number);
  Wire.endTransmission();
}

/***************************************************************************
Function Name: UpdateRGB
Purpose:
Update RGB LED according to define HOT and COLD temperature.
****************************************************************************/

void UpdateRGB (byte Temperature_H)
{
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);        /* Turn off all LEDs. */

  if (Temperature_H <= COLD)
  {
    digitalWrite(BLUE, HIGH);
  }
  else if (Temperature_H >= HOT)
  {
    digitalWrite(RED, HIGH);
  }
  else
  {
    digitalWrite(GREEN, HIGH);
  }
}

/***************************************************************************
Function Name: SerialMonitorPrint
Purpose:
Print current read temperature to the serial monitor.
****************************************************************************/
void SerialMonitorPrint (byte Temperature_H, int Decimal, bool IsPositive, bool isCelsius) // DC added bool parameter
{
  Serial.print("The temperature is ");
  if (!IsPositive)
  {
    Serial.print("-");
  }
  Serial.print(Temperature_H, DEC);
  Serial.print(".");
  Serial.print(Decimal, DEC);
  if (isCelsius == true) {
    Serial.print(" degrees C.");
  }
  else {
    Serial.print(" degrees F.");
  }
  Serial.print("\n\n");
}
