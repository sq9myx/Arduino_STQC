#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x4E/2, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
#define StartPin 3
String STQCcode;
unsigned long lastChange = 0;
int count;
void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  for (int i = 0; i < 5; i++)
  {
    pinMode(StartPin + i, INPUT);
  }
}

void loop() {
  /*
    if (Serial.available() > 0) {
    String ReceivedData = Serial.readStringUntil('\n');
    Serial.println(decodeSTQC(ReceivedData));
    // Serial.println(systemtodec(ReceivedData.toInt(),4));
    }*/

  for (int i = 0; i < 5; i++)
  {
    if (digitalRead(StartPin + i) == 0) {
      count = 0;
      for (int i = 0; i < 100; i++)
      {
        count += digitalRead(StartPin + i);
      }
      if (count < 70 && String(i) != String(STQCcode[STQCcode.length() - 1]))
      {
        lastChange = millis();
        STQCcode += String(i);
      }
    }
  }

  if (millis() - lastChange > 160 && STQCcode.length() > 1) {
    Serial.print(STQCcode);
    Serial.print(" ");
    if (decodeSTQC(STQCcode).toInt() > 9999) {
      Serial.print("Wrong code: ");
    }

    switch (STQCcode.length())
    {
      case 4:
        lcd.setCursor(0,0);
        lcd.print("A: ");
        Serial.print("A: ");
        break;

      case 7:
        lcd.setCursor(0,0);
        lcd.print("B: ");
        Serial.print("B: ");
        break;

      case 8:
        lcd.setCursor(0,0);
        lcd.print("C: ");
        Serial.print("C: ");
        break;

      default:
        lcd.setCursor(0,0);
        lcd.print("ERR");
        Serial.print("error");
        break;
    }

    Serial.println(decodeSTQC(STQCcode));
    Serial.println();
    lcd.setCursor(3,0);  
    lcd.print(decodeSTQC(STQCcode));  
    lcd.print("       ");
    STQCcode = "";
  }
}

String decodeSTQC(String input)
{
  String inputstr = input;
  if (inputstr[0] == '4') {
    inputstr[0] = '0';
  }
  for (int i = 1; i < inputstr.length(); i++) {
    if (inputstr[i] == '4') {
      inputstr[i] = inputstr[i - 1];
    }
  }
  return String(systemtodec(inputstr.toInt(), 4));
}

unsigned long systemtodec(unsigned long input, unsigned long system)
{
  unsigned long sum = 0;
  String inputstr = String(input);
  for (int i = (inputstr.length() - 1); i >= 0; i--)
  {
    sum += ((unsigned long)inputstr[i] - 48) * ceil(pow(system, (inputstr.length() - i - 1)));
  }
  return sum;
}
