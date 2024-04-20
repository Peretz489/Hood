#pragma once

#include <Arduino.h>
#include <DHT.h>
#include <math.h>
#include "Average.h"        // https://github.com/MajenkoLibraries/Average
#include <iarduino_OLED.h>  // https://github.com/tremaru/iarduino_OLED

namespace hood
{
    enum class DhtType
    {
        Type11,
        Type21
    };

    struct HoodConfig
    {
        uint16_t fan_on_time;
        uint16_t fan_cooldown_time;
        uint8_t dhtpin;
        uint8_t relaypin;
        DhtType sensor_type;
    };

    class Menu{

    };

    class Hood
    {
    public:
        explicit Hood(uint8_t dhtpin,
                      uint8_t relaypin,
                      uint16_t fan_on_time,
                      uint16_t fan_cooldown_time,
                      DhtType sensor_type);

        Hood(const Hood &) = delete;
        Hood &operator=(const Hood &) = delete;

        Hood &InitDisplay();
        Hood &InitDHT();
        void StartReadSequence();
        void IncreaseTreshold();
        void DecreaseTreshold();

    private:
        HoodConfig _config;
        Average<float> _ave;
        iarduino_OLED _display = iarduino_OLED(0x3C);
        DHT _dht = DHT(_config.dhtpin, _config.sensor_type == DhtType::Type11 ? DHT11 : DHT21);
        float _treshold = 11.6;
        bool _update_display = false;
        bool _time_to_measure = false;
        float _temperature = 0;
        float _relative_humidity = 0;
        float _absolute_humidity = 0;
        uint16_t _fan_on_time = 0;
        uint16_t _fan_off_time = 0;
        bool _fan_on = false;
        bool _fan_cooldown = false;
        bool _dht_error = false;

        void UpdateDisplay();
    };
};