#pragma once

#include <stdint.h>
#include <string.h>

#ifdef COMPILING_FOR_PC
    #include <stdio.h>
    #define IT_TO_STR(val, buf) snprintf(buf, 10, "%d", val)
#else
    #include <stdlib.h>
    #define IT_TO_STR(val, buf) itoa(val, buf, 10)
#endif

template <typename T>
void formatNumber(char* buffer, T value) {
    T integral = value / 100;
    int16_t decimals = value % 100;

    // Operators `/` and `%` get the negative sign in the number is a negative one. Obviously we cannot have "-5.-50"!
    if (decimals < 0) decimals = -decimals;

    IT_TO_STR(integral, buffer); 
    
    int len = strlen(buffer);
    buffer[len] = '.';
    
    if (decimals < 10) {
        buffer[len + 1] = '0';
        IT_TO_STR(decimals, buffer + len + 2);
    } else {
        IT_TO_STR(decimals, buffer + len + 1);
    }
}

void formatDateTime(char* buffer, uint8_t d, uint8_t m, uint16_t y, uint8_t h, uint8_t min);