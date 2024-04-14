#define DHT_PIN 5           // Датчик прицеплен к пину №5
#define RELAY_PIN 4         // Реле прицеплено к пину №4
#define BUTTON_PRESS 2      // На прерывание int0 вешаем кнопку повышения порога (расточительно)
#define BUTTON_SELECT 6     // На пине 6 выбор кнопки
#define DHT_QUERY_FREQ 1    // опрос DHT 1 раз в секунду
#define FAN_WORK_TIME 3     // время работы вентилятора в секундах
#define FAN_COOLDOWN_TIME 2 // время охлаждения вентилятора в секундах

#include <Arduino.h>
#include "hood.h"

hood::Hood myhood(DHT_PIN,
                  RELAY_PIN,
                  BUTTON_PRESS,
                  BUTTON_SELECT,
                  FAN_WORK_TIME,
                  FAN_COOLDOWN_TIME,
                  hood::DhtType::Type11);

volatile bool time_to_measure = false;

void btnPressed()
{
  if (digitalRead(BUTTON_SELECT))
  {
    myhood.IncreaseTreshold();
  }
  else
  {
    myhood.DecreaseTreshold();
  }
}

ISR(TIMER1_COMPA_vect)
{
  myhood.StartReadSequence();
}

void setup()
{
  //--- key interrupt ---
  attachInterrupt(0, btnPressed, FALLING);
  //--- timer ---
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TCCR1B |= (1 << WGM12);                 // устанавливаем режим СТС (сброс по совпадению)
  TIMSK1 |= (1 << OCIE1A);                // устанавливаем бит разрешения прерывания 1ого счетчика по совпадению с OCR1A(H и L)
  OCR1A = F_CPU / (256 * DHT_QUERY_FREQ); // устанавливаем частоту прерывания в соответствии с нужной частотой опроса
  TCCR1B |= (1 << CS12);                  // установим делитель
  sei();
  //--- hood init ---
  myhood.InitDisplay().InitDHT();
}

void loop()
{
}
