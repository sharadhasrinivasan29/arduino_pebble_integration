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
//#include <limits.h>

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
  double to_double (byte, int, bool);
  void convert_c_to_f (int&, byte&, bool&, int, byte, bool&);
  void Cal_temp (int&, byte&, byte&, bool&);
  void Dis_7SEG (int, byte, byte, bool);
  void Send7SEG (byte, byte);
  void SerialMonitorPrint (byte, int, bool, bool);
  void UpdateRGB (byte);


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
    double temperature_double[3600];
    int max_temp_h, max_temp_dec, min_temp_h, min_temp_dec, avg_temp_h, avg_temp_dec;
    double max_temp, min_temp, avg_temp;
    double current_temp_f, max_temp_f, min_temp_f, avg_temp_f;

    byte tooHot = -1;
    byte tooCold = -1;

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

      int min_index = 0; // for conversion
      int max_index = 0; // for conversion

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
      double current_temp = to_double(Temperature_H, Decimal, IsPositive);
      temperature_double[j] = current_temp;

      double max_temp = -1000;
      double min_temp = 1000;
      double avg_temp = 0;

       for (int i = 0; i < j; i++) {
        if (temperature_double[i] < min_temp) {
          min_temp = temperature_double[i];
          min_index = i; // for conversion
        }
        if (temperature_double[i] > max_temp) {
          max_temp = temperature_double[i];
          max_index = i; // for conversion
        }
         avg_temp += temperature_double[i];
       }
       avg_temp = (avg_temp / j);

      current_temp_f = convert_double_c_to_f(current_temp);
      max_temp_f = convert_double_c_to_f(max_temp);
      min_temp_f = convert_double_c_to_f(min_temp);
      avg_temp_f = convert_double_c_to_f(avg_temp);


      // update index
      array_index++;

      /* Display temperature on the serial monitor.
      Comment out this line if you don't use serial monitor.*/
      //      if (stop == 0) {
      //        if (celsius == true && showing_trend == false) {
      //          SerialMonitorPrint (Temperature_H, Decimal, IsPositive, true);
      //        }
      //        else if (celsius == false && showing_trend == false) {
      //          SerialMonitorPrint(f_temperature_H, f_decimal, f_IsPositive, false);
      //        }
      //      }

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
        char msg = Serial.read();

        if (msg == '0') { // display current temp
          if (!standby) {
            writing_word = false;
            blinking = false;
            showing_trend = false;
            if (celsius) {
              Serial.print(current_temp);
              Serial.print(" C");
              continue;
            } else {
              Serial.print(current_temp_f);
              Serial.print(" F");
              continue;
            }
          }
        }
        if (msg == '1'){ // set temp in F
          celsius = false;
          writing_word = false;
          blinking = false;
          showing_trend = false;
          Dis_7SEG(f_decimal * 10, f_temperature_H, f_temperature_L, f_IsPositive, 0);
        }
        if (msg == '2') { // set temp in C
          celsius = true;
          writing_word = false;
          blinking = false;
          showing_trend = false;
          Dis_7SEG (Decimal, Temperature_H, Temperature_L, IsPositive, 1);
        }

        if (msg == '3') { // stats : avg temp
          if (!standby) {
            writing_word = false;
            blinking = false;
            showing_trend = false;
            if (celsius) {
              double test = avg_temp / array_index;
               Serial.print(avg_temp);
              Serial.print(" C.");
              continue;
            } else {
              double test = convert_double_c_to_f(avg_temp);
              test = (test / array_index);
               Serial.print(avg_temp_f);
              Serial.print(" F.");
              continue;
            }
          }
        }
        if (msg == '4') { // stats : max temp
          if (!standby) {
            writing_word = false;
            blinking = false;
            showing_trend = false;
            if (celsius) {
              Serial.print(max_temp);
              Serial.print(" C.");
              continue;
            } else {
              Serial.print(max_temp_f);
              Serial.print(" F.");
              continue;
            }
          }
        }
        if (msg == '5') { // stats : min
          if (!standby) {
            writing_word = false;
            blinking = false;
            showing_trend = false;
            if (celsius) {
              Serial.print(min_temp);
              Serial.print(" C.");
              continue;
            } else {
              Serial.print(avg_temp_f);
              Serial.print(" F.");
              continue;
            }
          }
        }
        if (msg == '6') { // standby
          standby = true;
          blinking = false;
          writing_word = true;
          showing_trend = false;
          if (standby) {
            for (counter = 1; counter <= 4; counter++) {
              Send7SEG(counter,0x40); // print ----
            }
            delay(1000);
          }
        }

        if (msg == 'd') { // resume from standby
          standby = false;
          writing_word = false;
          blinking = false;
          showing_trend = false;
          if (!standby) {
            if (celsius) {
              Dis_7SEG (Decimal, Temperature_H, Temperature_L, IsPositive, 1);
            } else {
              Dis_7SEG(f_decimal * 10, f_temperature_H, f_temperature_L, f_IsPositive, 0);
            }
          }
        }

        if (msg == '7') { // feature 1 : trend
          if (!standby) {
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
              xysum = xysum + (i * temperature_stats[i][0]);
              xxsum = xxsum + (i*i);
            }

            //calculate slope of regression
            mNumerator = (j * xysum) - (xsum * ysum);
            mDenominator = (j * xxsum) - (xsum * xsum);
            m = mNumerator / mDenominator;

            //send trend to computer
            if (m > 0){
              Serial.print("Rising");
            } else if (m < 0){
              Serial.print("Falling");
            } else {
              Serial.print("Constant");
            }
          }
        }

        if (msg == '8') { // feature 2 word 1 (Oops)
          if (!standby) {
            writing_word = true;
            showing_trend = false;
            Word_7SEG(0, 1, 2, 3);
            Serial.print(" ");
          }
        }

        if (msg == '9') { // feature 3 blink green
          digitalWrite(RED, LOW);
          digitalWrite(GREEN, LOW);
          digitalWrite(BLUE, LOW);
          if (!standby) {
            Serial.print(" ");
            writing_word = false;
            blinking = true;
            showing_trend = false;
            
            if (celsius) {
              Dis_7SEG (Decimal, Temperature_H, Temperature_L, IsPositive, 1);
            } else {
              Dis_7SEG(f_decimal * 10, f_temperature_H, f_temperature_L, f_IsPositive, 0);
            }
            
            int counter = 3;
            while (counter > 0) {
              digitalWrite(GREEN, HIGH);
              delay(1000);                       // wait for a second
              digitalWrite(GREEN, LOW);
              delay(1000);                       // wait for a second
              counter--;
            }
          }
        }

        if (msg == 'a') { // feature 3 blink blue
          digitalWrite(RED, LOW);
          digitalWrite(GREEN, LOW);
          digitalWrite(BLUE, LOW);
          if (!standby) {
            Serial.print(" ");
            writing_word = false;
            blinking = true;
            showing_trend = false;
            
            if (celsius) {
              Dis_7SEG (Decimal, Temperature_H, Temperature_L, IsPositive, 1);
            } else {
              Dis_7SEG(f_decimal * 10, f_temperature_H, f_temperature_L, f_IsPositive, 0);
            }
            
            int counter = 3;
            while (counter > 0) {
              digitalWrite(BLUE, HIGH);
              delay(1000);                       // wait for a second
              digitalWrite(BLUE, LOW);
              delay(1000);                       // wait for a second
              counter--;
            }
          }
        }


        if (msg == 'b') { // feature 3 blink red
          digitalWrite(RED, LOW);
          digitalWrite(GREEN, LOW);
          digitalWrite(BLUE, LOW);

          if (!standby) {
            Serial.print(" ");
            writing_word = false;
            blinking = true;
            showing_trend = false;

            if (celsius) {
              Dis_7SEG (Decimal, Temperature_H, Temperature_L, IsPositive, 1);
            } else {
              Dis_7SEG(f_decimal * 10, f_temperature_H, f_temperature_L, f_IsPositive, 0);
            }
            
            int counter = 3;
            while (counter > 0) {
              digitalWrite(RED, HIGH);
              delay(1000);                       // wait for a second
              digitalWrite(RED, LOW);
              delay(1000);                       // wait for a second
              counter--;
            }
          }
        }

        if (msg == 'c') { // feature 4 drink recommendation
          showing_trend = false;
          writing_word = false;
          blinking = false;
          if (tooHot == -1 && tooCold == -1) {
            // error handling
            Serial.print("No preference found.");
          } else if (Temperature_H >= tooHot){
            Serial.print("Have an iced tea!");
          } else if (Temperature_H <= tooCold){
            Serial.print("Have a hot cocoa!");
          } else {
            Serial.print("Have a glass of water!");
          }
        }

        if (msg == 'e') { // feature 2 word 2 Hello
          if (!standby) {
            writing_word = true;
            blinking = false;
            showing_trend = false;
            Word_7SEG(4, 5, 6, 0);
            Serial.print(" ");
          }
        }
        if (msg == 'f') { // feature 2 word 3 Hola
          if (!standby) {
            writing_word = true;
            blinking = false;
            showing_trend = false;
            Word_7SEG(4, 0, 11, 10);
            Serial.print(" ");
          }
        }
        if (msg == 'g') {
          Serial.print(" ");
        }
        if (msg == 'h') { // 35
          showing_trend = false;
          writing_word = false;
          blinking = false;
          tooHot = 35;
          Serial.print(" ");
        }
        if (msg == 'i') { // 30
          showing_trend = false;
          writing_word = false;
          blinking = false;
          tooHot = 30;
          Serial.print(" ");
        }
        if (msg == 'j') { // 25
          writing_word = false;
          showing_trend = false;
          blinking = false;
          tooHot = 25;
          Serial.print(" ");
        }
        if (msg == 'k') {
          Serial.print(" ");
        }
        if (msg == 'l') { // cold 0
          showing_trend = false;
          blinking = false;
          writing_word = false;
          tooCold = 0;
          Serial.print(" ");
        }
        if (msg == 'm') { // cold 5
          showing_trend = false;
          writing_word = false;
          blinking = false;
          tooCold = 5;
          Serial.print(" ");
        }
        if (msg == 'n') { // cold 10
          showing_trend = false;
          writing_word = false;
          blinking = false;
          tooCold = 10;
          Serial.print(" ");
        }
      }

      if (celsius == true && writing_word == false) { // display in C
        Dis_7SEG(Decimal, Temperature_H, Temperature_L, IsPositive, 1);
      } else if (celsius == false && writing_word == false) { // display in F
        Dis_7SEG(f_decimal * 10, f_temperature_H, f_temperature_L, f_IsPositive, 0);
      }

      delay (1000);        /* Take temperature read every 1 second */
    }
  }

  /***************************************************************************
 Function Name: to_double

 Purpose: 
   Return temp as double
****************************************************************************/
double to_double (byte Temperature_H, int Decimal, bool IsPositive) {
  double ans = 0;
  if (Decimal < 1000) {
    ans = Temperature_H + (Decimal / 1000.0);  
  } else {
    ans = Temperature_H + (Decimal / 10000.0);
  }
  if (!IsPositive) {
    ans *= -1;
  }
  return ans;
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

    if (c_decimal < 1000) {
      deg_c = c_high + (c_decimal / 1000.0);
    } else {
      deg_c = c_high + (c_decimal / 10000.0);
    }
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
  Function Name: double_convert_c_to_f
  // DC
  Purpose:
  Calculate temperature from raw data.
  ****************************************************************************/
  double convert_double_c_to_f (double input_c) {
    return (9.0 / 5.0) * input_c + 32.0;
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
  //  void SerialMonitorPrint (byte Temperature_H, int Decimal, bool IsPositive, bool isCelsius) // DC added bool parameter
  //  {
  //    if (!IsPositive)
  //    {
  //      Serial.print("-");
  //    }
  //    Serial.print(Temperature_H, DEC);
  //    Serial.print(".");
  //    Serial.print(Decimal, DEC);
  //  }
