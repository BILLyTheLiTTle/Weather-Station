#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

class BME280Sensor {
private:
    uint8_t _addr;
    
    // Παράμετροι καλιμπραρίσματος (Bosch)
    uint16_t dig_T1; int16_t  dig_T2; int16_t  dig_T3;
    uint16_t dig_P1; int16_t  dig_P2; int16_t  dig_P3; int16_t  dig_P4;
    int16_t  dig_P5; int16_t  dig_P6; int16_t  dig_P7; int16_t  dig_P8; int16_t  dig_P9;
    uint8_t  dig_H1; int16_t  dig_H2; uint8_t  dig_H3; int16_t  dig_H4; int16_t  dig_H5; int8_t dig_H6;
    
    int32_t t_fine;

    // Εσωτερικές μεταβλητές αποθήκευσης
    int32_t _lastTemperature;   // Not used
    uint32_t _lastPressure;     // Σε Pascal
    uint32_t _lastHumidity;     // Not working

    void readCalibrationData();
    int32_t compensateTemperature(int32_t adc_T);
    uint32_t compensatePressure(int32_t adc_P);
    uint32_t compensateHumidity(int32_t adc_H);

    static constexpr float ALTITUDE = 455.0; //m (Αργυρά is on 288m altitude + 167 for calibration)

public:
    BME280Sensor();
    bool begin(uint8_t i2cAddress = 0x76);
    
    // Ανανέωση των εσωτερικών registers
    void update(); 

    // Οι get συναρτήσεις
    int32_t getTemperature();   // Not used
    uint32_t getPressure();     // Σε Pascal
    uint32_t getHumidity();     // Not working
};

#endif