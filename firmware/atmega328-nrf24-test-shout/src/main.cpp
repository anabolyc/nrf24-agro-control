#include <Arduino.h>
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define SERIAL_BAUD 115200

#define PIN_LED_BUILTIN 13
#define PIN_NRF24_CE 7
#define PIN_NRF24_CSN 8

#define NRF24_POW RF24_PA_LOW // RF24_PA_HIGH, RF24_PA_MAX
#define NRF24_TIMEOUT 200000

uint8_t NRF24_ADDR_SRC[6] = "N-001";
uint8_t NRF24_ADDR_DST[6] = "M-001";

RF24 radio(PIN_NRF24_CE, PIN_NRF24_CSN);

void setup()
{
  Serial.begin(SERIAL_BAUD);
  Serial.println(F("Starting.."));
  pinMode(PIN_LED_BUILTIN, OUTPUT);

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

void loop()
{
  radio.stopListening();
  digitalWrite(PIN_LED_BUILTIN, HIGH);

  // Shoot
  unsigned long start_time = micros();
  Serial.print(F("))) "));
  Serial.print(start_time);
  if (!radio.write(&start_time, sizeof(unsigned long)))
    Serial.println(F(" X"));
  else
    Serial.println(F(" V"));

  // Wait for response
  digitalWrite(PIN_LED_BUILTIN, LOW);
  radio.startListening();

  unsigned long started_waiting_at = micros();
  boolean timeout = false;

  while (!radio.available())
  {
    if (micros() - started_waiting_at > NRF24_TIMEOUT)
    {
      timeout = true;
      break;
    }
  }

  if (timeout)
  {
    Serial.println(F("Failed, response timed out."));
  }
  else
  {
    unsigned long got_time;
    radio.read(&got_time, sizeof(unsigned long));
    unsigned long end_time = micros();

    Serial.print(F("Sent "));
    Serial.print(start_time);
    Serial.print(F(", Got response "));
    Serial.print(got_time);
    Serial.print(F(", Round-trip delay "));
    Serial.print(end_time - start_time);
    Serial.println(F(" us"));
  }

  _delay_ms(1000);
}