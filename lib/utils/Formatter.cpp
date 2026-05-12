#include "Formatter.h"

void formatNumber(char* buffer, int16_t value) {
    int16_t integral = value / 100;
    int16_t decimals = value % 100;

    itoa(integral, buffer, 10); 
    
    int len = strlen(buffer);
    buffer[len] = '.';
    
    if (decimals < 10) {
        buffer[len + 1] = '0';
        itoa(decimals, buffer + len + 2, 10);
    } else {
        itoa(decimals, buffer + len + 1, 10);
    }
}

void formatNumber(char* buffer, uint16_t value) {
    uint16_t integral = value / 100;
    uint16_t decimals = value % 100;

    itoa(integral, buffer, 10); 
    
    int len = strlen(buffer);
    buffer[len] = '.';
    
    if (decimals < 10) {
        buffer[len + 1] = '0';
        itoa(decimals, buffer + len + 2, 10);
    } else {
        itoa(decimals, buffer + len + 1, 10);
    }
}

void formatDateTime(char* buffer, uint8_t d, uint8_t m, uint16_t y, uint8_t h, uint8_t min) {
    buffer[0] = (d / 10) + '0';
    buffer[1] = (d % 10) + '0';
    buffer[2] = '/';

    buffer[3] = (m / 10) + '0';
    buffer[4] = (m % 10) + '0';
    buffer[5] = '/';

    itoa(y, buffer + 6, 10); 
    
    buffer[10] = ' ';

    buffer[11] = (h / 10) + '0';
    buffer[12] = (h % 10) + '0';
    buffer[13] = ':';

    buffer[14] = (min / 10) + '0';
    buffer[15] = (min % 10) + '0';

    buffer[16] = '\0';
}