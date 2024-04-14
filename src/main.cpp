// Поскольку смысл вытяжки в ванной препятствовать образованию плесени, максимальная относительная влажность стен при 20 градусах
// должна составлять менее 60%. Но упираемся в то, что температура в ванной меняется и всегда выше температуры наружной стены
// поэтому надо считать не относительную, а абсолютную влажность, например
// точка росы при 15 градусах цельсия - 12,83 гр/м3, это 100% относительной влажности, чему соответствует 66% при 22 градусах
// Если считать по самой холодной стене, в доме будет Сахара, однако в ванной стараемся держать 60% при 22 градусах, это 11.66 гр/м3
// Формула расчёта абсолютной влажности (гр/м3) H_abs = 6.122*EXP((17.67*T)/(T+243.5))*H_otn*2,1674/(273.15+T)

#define OLED                // есть OLED индикатор
#define DHTTYPE DHT11       // Если используется не DHT11, надо поменять
#define relaypin 4          // Реле прицеплено к пину №4
#define dhtpin 5            // Датчик прицеплен к пину №5
#define button_press 2         // На прерывание int0 вешаем кнопку повышения порога (расточительно)
#define button_select 6     // На пине 6 выбор кнопки
#define DHT_QUERY_FREQ 1    // опрос DHT 1 раз в секунду
#define FAN_WORK_TIME 3000     // время работы вентилятора в секундах
#define FAN_COOLDOWN_TIME 1800 // время охлаждения вентилятора в секундах


#include <Arduino.h>
#include "DHT.h"     // Стандартная библиотека Arduino
#include "Average.h" // https://github.com/MajenkoLibraries/Average
#include <math.h>
#ifdef OLED
#include "iarduino_OLED.h" // https://github.com/Sylaina/oled-display
#endif

DHT dht(dhtpin, DHTTYPE);
Average<float> ave(10); // Для усреднения последних значений с датчика создаём массив на 10 шт. float

#ifdef OLED
iarduino_OLED Display(0x3C);
extern uint8_t SmallFontRus[];
#endif

volatile float treshold = 11.6;
#ifdef OLED
volatile bool update_display = false;
#endif
volatile bool time_to_measure = false;
float temperature = 0;
float relative_humidity = 0;
float absolute_humidity = 0;
int8_t fan_on_time = 0;
int8_t fan_off_time = 0;
bool fan_on = false;
bool fan_cooldown = false;
bool dht_error = false;

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
#ifdef OLED
  update_display = true;
#endif
}

ISR(TIMER1_COMPA_vect)
{
  time_to_measure = true;
}

void setup()
{
#ifndef OLED
  Serial.begin(9600);
  Serial.print("Starting, humidity treshold set to: ");
  Serial.print(treshold);
  Serial.println("g/m3.");
#endif
  //--- relay ----
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, HIGH); // По умолчанию вентилятор не работает (подключён к "N.O." - "normally open" разъёму реле)
  //--- buttons ----
  pinMode(button_press, INPUT_PULLUP);
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
#ifdef OLED
  Display.begin(); // Инициируем работу с дисплеем.
  Display.setFont(SmallFontRus);
#endif
  //--- DHT ---
  dht.begin();
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
#ifndef OLED
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
#endif
  }
  return true;
}

#ifdef OLED
void UpdateDisplay()
{
  if (!dht_error)
  {
    Display.clrScr();                           // Чистим экран.
    Display.print(F("Отн. влажность:"), 0, 10); // Выводим текст начиная с координаты 0x7.
    Display.print(relative_humidity, 96, 10);   // Выводим число начиная с координаты 0x39.
    Display.print(F("Абс. влажность:"), 0, 25);
    Display.print(absolute_humidity, 96, 25);
    Display.print(F("Порог включения:"), 0, 40);
    Display.print(treshold, 96, 40);
    Display.print(F("Вентилятор:"), 0, 55);
    if (fan_on)
    {
      Display.print(F("ВКЛ"), 90, 55);
    }
    else if (fan_cooldown)
    {
      Display.print(F("СОН"), 90, 55);
    }
    else
    {
      Display.print(F("ВЫКЛ"), 90, 55);
    }
    update_display = false;
  }
  else
  {
    Display.print(F("ОШИБКА СЕНСОРА"), 20, 35);
  }
}
#endif

void loop()
{
#ifdef OLED
  if (update_display)
  {
    UpdateDisplay();
  }
#endif
  if (time_to_measure)
  {
    dht_error = !ReadAndPutData();
#ifndef OLED
    if (dht_error == true)
    {
      Serial.println("Sensor error!");
    }
#endif
    if (fan_on)
    {
      if (fan_on_time < FAN_WORK_TIME)
      {
        ++fan_on_time;
      }
      else
      {
        digitalWrite(relaypin, HIGH); // отключаем вентиль чтобы остыл
#ifndef OLED
        Serial.println("Fan cooldown started.");
#endif
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
#ifndef OLED
        Serial.println("Fan cooldown ended.");
#endif
        fan_off_time = 0;
        fan_cooldown = false;
      }
    }
    time_to_measure = false;
  }
  if ((ave.mean() >= treshold) && !fan_on && !fan_cooldown)
  {                              // проверяем превышение порога если вентиль не остывает и не включен
    digitalWrite(relaypin, LOW); // если порог превышен- включаем.
#ifndef OLED
    Serial.println("Fan started.");
#endif
    fan_on = true;
  }
#ifdef OLED
  update_display = true;
#endif
}
