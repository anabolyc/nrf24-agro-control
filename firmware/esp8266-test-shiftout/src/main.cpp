#include <Arduino.h>
#include <SPI.h>

#define SERIAL_BAUD 74880

#define PIN_595_DATA 2
#define PIN_595_LATCH 5
#define PIN_595_CLK 16
#define PIN_595_EN 0

void waitForKey()
{
  while (!Serial.available())
    ;
  Serial.read();
}

void shiftout(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
    uint8_t i;

    for(i = 0; i < 8; i++) {
        if(bitOrder == LSBFIRST)
            digitalWrite(dataPin, !!(val & (1 << i)));
        else
            digitalWrite(dataPin, !!(val & (1 << (7 - i))));

        digitalWrite(clockPin, HIGH);
        delay(1);
        digitalWrite(clockPin, LOW);
    }
}

void setup()
{
  Serial.begin(SERIAL_BAUD);
  while (!Serial)
    ;

  Serial.println(F("Starting.."));

  pinMode(PIN_595_DATA, OUTPUT);
  pinMode(PIN_595_LATCH, OUTPUT);
  pinMode(PIN_595_CLK, OUTPUT);
  pinMode(PIN_595_EN, OUTPUT);

  digitalWrite(PIN_595_EN, LOW);

  Serial.print(F("Press any key.."));
  waitForKey();
}

uint8_t x = 0;

void loop()
{
  waitForKey();

  uint8_t v = 1 << (x % 8);
  Serial.print(v);
  Serial.print(" = 0b");
  Serial.println(v, BIN);

  digitalWrite(PIN_595_LATCH, LOW);
  shiftout(PIN_595_DATA, PIN_595_CLK, LSBFIRST, v);
  digitalWrite(PIN_595_LATCH, HIGH);

  x++;
}
