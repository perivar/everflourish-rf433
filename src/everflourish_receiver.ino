#include "Arduino.h"

// function prototype allowing the function to be used before it is defined.
static char *dec2binWzerofill(unsigned long dec, unsigned int bitLength);
void printBinary(unsigned long inNumber, unsigned int digits = 32);

int rxPin = 2;

void setup()
{
  pinMode(rxPin, INPUT);
  Serial.begin(9600);
}

void loop()
{
  int i = 0;
  unsigned long t = 0;
  byte bit = 0;

  unsigned long sender = 0;
  unsigned long group = 0;
  unsigned long command = 0;

  // latch
  while ((t < 11000 || t > 17000))
  {
    t = pulseIn(rxPin, LOW, 1000000);
  }

  // data (52 bits)
  while (i < 52)
  {
    t = pulseIn(rxPin, LOW, 1000000);

    if (t > 550 && t < 650)
    {
      bit = 1;
    }
    else if (t > 1000 && t < 1360)
    {
      bit = 0;
    }
    else
    {
      i = 0;
      break;
    }

    if (i < 32)
    { // first 32 data bits
      sender <<= 1;
      sender |= bit;
    }
    else if (i >= 32 && i < 44)
    { // next 12 group bits
      group <<= 1;
      group |= bit;
    }
    else
    { // last 8 data bits
      command <<= 1;
      command |= bit;
    }

    ++i;
  }

  // interpret message
  if (i > 0)
  {
    printResult(sender, group, command);
  }
}

void printResult(unsigned long sender, unsigned long group, unsigned int command)
{
  Serial.print("sender ");
  Serial.println(sender);

  Serial.print("group ");
  Serial.println(group);

  Serial.print("command ");
  Serial.println(command);

  Serial.print("binary ");
  printBinary(sender);
  Serial.print(" ");
  printBinary(group, 12);
  Serial.print(" ");
  printBinary(command, 8);
  Serial.println();

  Serial.println();
}

void printBinary(unsigned long inNumber, unsigned int digits = 32)
{
  /*
  for (int b = digits-1; b >= 0; b--)
  {
    Serial.print(bitRead(inNumber, b));
  }
  */
  Serial.print(dec2binWzerofill(inNumber, digits));
}

static char *dec2binWzerofill(unsigned long dec, unsigned int bitLength)
{
  static char bin[64];
  unsigned int i = 0;

  while (dec > 0)
  {
    bin[32 + i++] = ((dec & 1) > 0) ? '1' : '0';
    dec = dec >> 1;
  }

  for (unsigned int j = 0; j < bitLength; j++)
  {
    if (j >= bitLength - i)
    {
      bin[j] = bin[31 + i - (j - (bitLength - i))];
    }
    else
    {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';

  return bin;
}
