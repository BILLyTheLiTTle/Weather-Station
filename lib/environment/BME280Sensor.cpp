#include "BME280Sensor.h"

BME280Sensor::BME280Sensor() : _addr(0x76), t_fine(0), _lastTemperature(0), _lastPressure(0), _lastHumidity(0) {}

bool BME280Sensor::begin(uint8_t i2cAddress) {
    _addr = i2cAddress;
    Wire.begin();
    
    // Reset αισθητήρα
    Wire.beginTransmission(_addr);
    Wire.write(0xE0);
    Wire.write(0xB6);
    if (Wire.endTransmission() != 0) return false;
    delay(100);

    readCalibrationData();

    // Ρύθμιση Humidity (ctrl_hum: 0xF2) -> Oversampling x1
    Wire.beginTransmission(_addr);
    Wire.write(0xF2);
    Wire.write(0x01);
    Wire.endTransmission();

    // Ρύθμιση Control (ctrl_meas: 0xF4) -> Temp x1, Pres x1, Mode Normal
    Wire.beginTransmission(_addr);
    Wire.write(0xF4);
    Wire.write(0x27); 
    Wire.endTransmission();

    return true;
}

void BME280Sensor::readCalibrationData() {
    uint8_t data[24];
    
    // Μπλοκ 1: Θερμοκρασία & Πίεση
    Wire.beginTransmission(_addr);
    Wire.write(0x88);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)24);
    for(int i=0; i<24; i++) data[i] = Wire.read();

    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11] << 8) | data[10];
    dig_P4 = (data[13] << 8) | data[12];
    dig_P5 = (data[15] << 8) | data[14];
    dig_P6 = (data[17] << 8) | data[16];
    dig_P7 = (data[19] << 8) | data[18];
    dig_P8 = (data[21] << 8) | data[20];
    dig_P9 = (data[23] << 8) | data[22];

    // Μπλοκ 2: H1
    Wire.beginTransmission(_addr);
    Wire.write(0xA1);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)1);
    dig_H1 = Wire.read();

    // Μπλοκ 3: Υπόλοιπη Υγρασία
    Wire.beginTransmission(_addr);
    Wire.write(0xE1);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)7);
    for(int i=0; i<7; i++) data[i] = Wire.read();

    dig_H2 = (data[1] << 8) | data[0];
    dig_H3 = data[2];
    dig_H4 = (data[3] << 4) | (data[4] & 0x0F);
    dig_H5 = (data[5] << 4) | (data[4] >> 4);
    dig_H6 = (int8_t)data[6];
}

void BME280Sensor::update() {
    Wire.beginTransmission(_addr);
    Wire.write(0xF7); 
    Wire.endTransmission();
    
    // Διάβασμα και των 8 registers (Pressure, Temperature, Humidity)
    Wire.requestFrom(_addr, (uint8_t)8);
    uint8_t data[8];
    for(int i=0; i<8; i++) data[i] = Wire.read();

    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | (data[5] >> 4);
    int32_t adc_H = ((int32_t)data[6] << 8) | data[7];

    // Υπολογισμός και αποθήκευση εσωτερικά
    _lastTemperature = compensateTemperature(adc_T);
    _lastPressure = compensatePressure(adc_P);
    _lastHumidity = compensateHumidity(adc_H);
}

int32_t BME280Sensor::getTemperature() { return _lastTemperature; }
uint32_t BME280Sensor::getPressure() { return _lastPressure; }
uint32_t BME280Sensor::getHumidity() { return _lastHumidity; }

// Αλγόριθμοι Bosch
int32_t BME280Sensor::compensateTemperature(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

uint32_t BME280Sensor::compensatePressure(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if (var1 == 0) return 0;
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    return (uint32_t)(p / 256);
}

uint32_t BME280Sensor::compensateHumidity(int32_t adc_H) {
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
                                                   (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                                                 ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)((v_x1_u32r >> 12) * 100 / 1024);
}