#define DHT_PIN 5
#define RELAY_PIN 4
#define BUTTON_UP 7
#define BUTTON_DN 6
#define DHT_QUERY_FREQ 1
#define BTN_UPDATE_TIME_MS 10
#define FAN_WORK_TIME 300     // 300 секунд работаем
#define FAN_COOLDOWN_TIME 120 // 120 секунд отдыхаем

#include <Arduino.h>
#include "hood.h"

hood::Hood myhood(DHT_PIN,
                  RELAY_PIN,
                  BUTTON_UP,
                  BUTTON_DN,
                  FAN_WORK_TIME,
                  FAN_COOLDOWN_TIME,
                  hood::DhtType::Type11);

volatile bool time_to_measure = false;
// volatile bool time_to_check_btn = false;

// bool btn_pressed_count_started=false;
uint16_t btn_up_pressed_time;
uint16_t btn_dn_pressed_time;

// void btnPressed()
// {
//   if (digitalRead(BUTTON_SELECT))
//   {
//     myhood.IncreaseTreshold();
//   }
//   else
//   {
//     myhood.DecreaseTreshold();
//   }
// }

ISR(TIMER1_COMPA_vect)
{
  time_to_measure = true;
}

// ISR(TIMER2_COMPA_vect){
//   time_to_check_btn=true;
// }

void setup()
{
  //--- relay ---
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay off
  //--- buttons ----
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DN, INPUT_PULLUP);
  //--- timer 1---
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TCCR1B |= (1 << WGM12);                 // устанавливаем режим СТС (сброс по совпадению)
  TIMSK1 |= (1 << OCIE1A);                // устанавливаем бит разрешения прерывания 1ого счетчика по совпадению с OCR1A(H и L)
  TCCR1B |= (1 << CS12);                  // установим делитель 256 (таймер 16 бит)
  OCR1A = F_CPU / (256 * DHT_QUERY_FREQ); // устанавливаем частоту прерывания в соответствии с нужной частотой опроса
  sei();
  //--- timer 2---
  // cli();
  // TCCR2A = 0;
  // TCCR2B = 0;
  // TCNT2 = 0;
  // TCCR2B |= (1 << WGM12);                     // устанавливаем режим СТС (сброс по совпадению)
  // TIMSK2 |= (1 << OCIE1A);                    // устанавливаем бит разрешения прерывания 2ого счетчика по совпадению с OCR2A(H и L)
  // TCCR2B |= (1 << CS12);                      // установим делитель 1024 (таймер 8 бит)
  // TCCR2B |= (1 << CS10);
  // OCR2A = F_CPU / (1024 * BTN_UPDATE_TIME_MS * 10); // устанавливаем частоту прерывания в соответствии с нужной частотой опроса
  // sei();
  //--- hood init ---
  myhood.InitDisplay().InitDHT();

  Serial.begin(9600);
  Serial.println("Start");
}



void loop()
{
  if (time_to_measure)
  {
    myhood.StartReadSequence();
    Serial.println("Measurement");
    time_to_measure = false;
  }
  if (!digitalRead(BUTTON_UP) || !digitalRead(BUTTON_DN))
  {
    // cli();
    myhood.KeyPress();
    // sei();
  }
}
