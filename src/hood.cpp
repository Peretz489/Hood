#include "hood.h"

extern uint8_t SmallFontRus[]; // OLED font

namespace hood
{
    Hood::Hood(uint8_t dhtpin, uint8_t relaypin, uint16_t fan_on_time, uint16_t fan_cooldown_time, DhtType sensor_type)
        : _config(HoodConfig{fan_on_time,
                             fan_cooldown_time,
                             dhtpin,
                             relaypin,
                             sensor_type}),
          _ave(Average<float>(10))
    {
    }

    Hood &Hood::InitDisplay()
    {
        _display.begin();
        _display.setFont(SmallFontRus);
        return *this;
    }

    Hood &Hood::InitDHT()
    {
        _dht.begin();
        return *this;
    }

    void Hood::StartReadSequence()
    {
        _temperature = _dht.readTemperature();
        _relative_humidity = _dht.readHumidity();
        if (isnan(_temperature) || isnan(_relative_humidity))
        {
            _dht_error = true;
        }
        else
        {
            _absolute_humidity = 6.122 * exp((17.67 * _temperature) / (_temperature + 243.5)) * _relative_humidity * 2.1674 / (273.15 + _temperature);
            _ave.push(_absolute_humidity);
            if (_fan_on)
            {
                if (_fan_on_time < _config.fan_on_time)
                {
                    ++_fan_on_time;
                }
                else
                {
                    digitalWrite(_config.relaypin, HIGH);
                    _fan_cooldown = true;
                    _fan_on_time = 0;
                    _fan_on = false;
                }
            }
            if (_fan_cooldown)
            {
                if (_fan_off_time < _config.fan_cooldown_time)
                {
                    ++_fan_off_time;
                }
                else
                {
                    _fan_off_time = 0;
                    _fan_cooldown = false;
                }
            }
            if ((_ave.mean() >= _treshold) && !_fan_on && !_fan_cooldown)
            {
                digitalWrite(_config.relaypin, LOW);
                _fan_on = true;
            }
        }
        UpdateDisplay();
        _dht_error = false;
    }

    void Hood::IncreaseTreshold()
    {
        _treshold += 0.2;
        UpdateDisplay();
    }

    void Hood::DecreaseTreshold()
    {
        _treshold -= 0.2;
        UpdateDisplay();
    }

    void Hood::UpdateDisplay()
    {
        if (!_dht_error)
        {
            _display.clrScr();
            _display.print(F("Отн. влажность:"), 0, 10);
            _display.print(_relative_humidity, 96, 10);
            _display.print(F("Абс. влажность:"), 0, 25);
            _display.print(_absolute_humidity, 96, 25);
            _display.print(F("Порог включения:"), 0, 40);
            _display.print(_treshold, 96, 40);
            _display.print(F("Вентилятор:"), 0, 55);
            if (_fan_on)
            {
                _display.print(F("ВКЛ"), 90, 55);
            }
            else if (_fan_cooldown)
            {
                _display.print(F("СОН"), 90, 55);
            }
            else
            {
                _display.print(F("ВЫКЛ"), 90, 55);
            }
        }
        else
        {
            _display.clrScr();
            _display.print(F("ОШИБКА СЕНСОРА"), 20, 35);
        }
    }
}
