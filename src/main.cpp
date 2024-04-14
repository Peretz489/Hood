#define DHT_PIN 5           
#define RELAY_PIN 4         
#define BUTTON_PRESS 2      
#define BUTTON_SELECT 6     
#define DHT_QUERY_FREQ 1    
#define FAN_WORK_TIME 300     
#define FAN_COOLDOWN_TIME 120 

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
  time_to_measure=true;
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
  if (time_to_measure)
  {
    myhood.StartReadSequence();
    time_to_measure = false;
  }
}
