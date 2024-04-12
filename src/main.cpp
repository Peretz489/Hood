// Поскольку смысл вытяжки в ванной препятствовать образованию плесени, максимальная относительная влажность стен при 20 градусах
// должна составлять менее 60%. Но упираемся в то, что температура в ванной меняется и всегда выше температуры наружной стены
// поэтому надо считать не относительную, а абсолютную влажность, например
// точка росы при 15 градусах цельсия - 12,83 гр/м3, это 100% относительной влажности, чему соответствует 66% при 22 градусах
// Если считать по самой холодной стене, в доме будет Сахара, однако в ванной стараемся держать 60% при 22 градусах, это 11.66 гр/м3
// Формула расчёта абсолютной влажности (гр/м3) H_abs = 6.122*EXP((17.67*T)/(T+243.5))*H_otn*2,1674/(273.15+T)

#include <Arduino.h>

#include "DHT.h"     // Стандартная библиотека Arduino
#include "Average.h" // https://github.com/MajenkoLibraries/Average
#include <math.h>

#include "fDigitsSegtPin.h" //https://github.com/KLszTsu/fDigitsSegtPin

#define DEBUG 1
// #define SEG_IND 1            // есть индикатор
#define DHTTYPE DHT11       // Если используется не DHT11, надо поменять
#define relaypin 4          // Реле прицеплено к пину №4
#define dhtpin 5            // Датчик прицеплен к пину №5
#define button_up 2         // На прерывание int0 вешаем кнопку повышения порога (расточительно)
#define button_select 6     // На пине 6 выбор кнопки
#define DHT_QUERY_FREQ 1    // опрос DHT 1 раз в секунду
#define FAN_WORK_TIME 3     // время работы вентилятора в секундах
#define FAN_COOLDOWN_TIME 3 // время охлаждения вентилятора в секундах
#define RELATIVE 0          // режим отображения порога влажности устанавливаем абсолютные величины

// #ifdef SEG_IND
#define SEG_A 8
#define SEG_B 10
#define SEG_C 9
#define SEG_D 12
#define SEG_E 11
#define SEG_F 13
#define SEG_G A1
#define SEG_P A0
#define DIG_1 A2
#define DIG_2 A5
#define DIG_3 A4
#define DIG_4 A3
// #endif

DHT dht(dhtpin, DHTTYPE);
Average<float> ave(10); // Для усреднения последних значений с датчика создаём массив на 10 шт. float

// pin assigment as on indicator
fDigitsSegtPin Display(SEG_E, SEG_D, SEG_P, SEG_C, SEG_G, DIG_4, SEG_B, DIG_3, DIG_2, SEG_F, SEG_A, DIG_1);

volatile float treshold = 11.6;
volatile bool time_to_measure = false;
volatile bool relative = false;
float temperature = 0;
float relative_humidity = 0;
float absolute_humidity = 0;
// float last_displayed=-1;
int8_t fan_on_time = 0;
int8_t fan_off_time = 0;
bool fan_on = false;
bool fan_cooldown = false;

bool ReadAndPutData();

void btnPressed()
{
  if (digitalRead(button_select))
  {
    treshold += 0.2;
  }
  else
  {
    treshold -= 0.2;
  }
  delay(100);
  uint32_t start = 0;
  while (start < F_CPU/50000) // многопоточности нет, поэтому приходится показывать
                              // текущее пороговое значение под прерыванием. Иначе гаснет экран
                              // на время опроса DHT
  {
    Display.Print(treshold);
    ++start;
  }
}

ISR(TIMER1_COMPA_vect)
{
  time_to_measure = true;
}

void setup()
{
  if (DEBUG)
  {
    Serial.begin(9600);
    Serial.print("Starting, humidity treshold set to: ");
    Serial.print(treshold);
    Serial.println("g/m3.");
  }
  //--- relay ----
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, HIGH); // По умолчанию вентилятор не работает (подключён к "N.O." - "normally open" разъёму реле)
  //--- buttons ----
  pinMode(button_up, INPUT_PULLUP);
  attachInterrupt(0, btnPressed, FALLING);
  pinMode(button_select, INPUT_PULLUP);
  //--- timer ---
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TCCR1B |= (1 << WGM12);                 // устанавливаем режим СТС (сброс по совпадению)
  TIMSK1 |= (1 << OCIE1A);                // устанавливаем бит разрешения прерывания 1ого счетчика по совпадению с OCR1A(H и L)
  OCR1A = F_CPU / (256 * DHT_QUERY_FREQ); // устанавливаем частоту прерывания
  TCCR1B |= (1 << CS12);                  // установим делитель
  sei();
  //--- display ---
  Display.Begin(Type::COMMON_CATHODE);
  Display.doPrint_firstZero = 1;
  //--- DHT ---
  dht.begin();
}

void loop()
{
  // Display.Print(1234); // тест экрана
  if (time_to_measure)
  {
    if (!ReadAndPutData())
    {
      if (DEBUG)
        Serial.println("Sensor error!");
    }
    if (fan_on)
    {
      if (fan_on_time < FAN_WORK_TIME)
      {
        ++fan_on_time;
      }
      else
      {
        digitalWrite(relaypin, HIGH); // отключаем вентиль чтобы остыл
        if (DEBUG)
          Serial.println("Fan cooldown started.");
        fan_cooldown = true;
        fan_on_time = 0;
        fan_on = false;
      }
    }
    if (fan_cooldown)
    {
      if (fan_off_time < FAN_COOLDOWN_TIME)
      {
        ++fan_off_time;
      }
      else
      {
        if (DEBUG)
          Serial.println("Fan cooldown ended.");
        fan_off_time = 0;
        fan_cooldown = false;
      }
    }
    time_to_measure = false;
  }
  if ((ave.mean() >= treshold) && !fan_on && !fan_cooldown)
  {                              // проверяем превышение порога если вентиль не остывает и не включен
    digitalWrite(relaypin, LOW); // если порог превышен- включаем.
    if (DEBUG)
      Serial.println("Fan started.");
    fan_on = true;
  }
}

bool ReadAndPutData()
{
  temperature = dht.readTemperature(); // Опрашиваем датчик
  relative_humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(relative_humidity))
  {
    return false;
  }
  else
  {
    absolute_humidity = 6.122 * exp((17.67 * temperature) / (temperature + 243.5)) * relative_humidity * 2.1674 / (273.15 + temperature);
    ave.push(absolute_humidity);
    if (DEBUG)
    {
      Serial.print("Humidity: ");
      Serial.print(relative_humidity);
      Serial.print("% \t");
      Serial.print("Absolute humidity average: ");
      Serial.print(ave.mean());
      Serial.print(" g/m3 \t");
      Serial.print("Temp.: ");
      Serial.print(temperature);
      Serial.println("C");
      Serial.print("Current treshold: ");
      Serial.println(treshold);
    }
  }
  return true;
}