#include <Arduino.h>
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define SERIAL_BAUD 74880

//#define PIN_LED_BUILTIN 2
#define PIN_NRF24_CE 4
#define PIN_NRF24_CSN 15

#define PIN_595_DATA 2
#define PIN_595_LATCH 5
#define PIN_595_CLK 16
#define PIN_595_EN 0

#define NRF24_POW RF24_PA_LOW // RF24_PA_HIGH, RF24_PA_MAX
#define NRF24_TIMEOUT 200000

uint8_t NRF24_ADDR_SRC[6] = "M-001";
uint8_t NRF24_ADDR_DST[6] = "N-001";

RF24 radio(PIN_NRF24_CE, PIN_NRF24_CSN);

void setup()
{
  Serial.begin(SERIAL_BAUD);
  while (!Serial)
    ;
  Serial.println(F("Starting.."));

  //pinMode(PIN_LED_BUILTIN, OUTPUT);
  pinMode(PIN_595_DATA, OUTPUT);
  pinMode(PIN_595_LATCH, OUTPUT);
  pinMode(PIN_595_CLK, OUTPUT);
  pinMode(PIN_595_EN, OUTPUT);

  digitalWrite(PIN_595_EN, LOW);

  if (!radio.begin())
  {
    Serial.println(F("RF24 hw init failed"));
    while (1)
      ;
  }

  radio.setPALevel(NRF24_POW);
  radio.openWritingPipe(NRF24_ADDR_DST);
  radio.openReadingPipe(1, NRF24_ADDR_SRC);
  radio.startListening();

#ifdef NRF24_DEBUG
  printf_begin();
  //radio.printDetails();
  radio.printPrettyDetails();

  // Serial.print(F("Press any key.."));
  // while (!Serial.available())
  //   ;
#endif
}

uint8_t x = 0;

void loop()
{
  unsigned long got_time;
  if (radio.available())
  {
    //digitalWrite(PIN_LED_BUILTIN, HIGH);

    while (radio.available())
    {
      radio.read(&got_time, sizeof(unsigned long));
    }

    radio.stopListening();
    radio.write(&got_time, sizeof(unsigned long));
    radio.startListening();
    Serial.print(F("))) "));
    Serial.println(got_time);

    x++;

    digitalWrite(PIN_595_LATCH, LOW);
    shiftOut(PIN_595_DATA, PIN_595_CLK, LSBFIRST, x);
    digitalWrite(PIN_595_LATCH, HIGH);

    //digitalWrite(PIN_LED_BUILTIN, LOW);
  }
}