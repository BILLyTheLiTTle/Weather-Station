#include <gtest/gtest.h>
#include "ACS712.h"
#include "../fake_hal/FakeArduinoBoard.h"

TEST(ACS712Test, TestCurrentCalculation) {
    FakeArduinoBoard mock;
    ACS712 sensor(&mock, 1, ACS712_05B, 2000);

    mock.analogReadValue = 512;
    sensor.calibrate();

    mock.analogReadValue = 612;
    
    uint32_t result = sensor.getCurrentMA();

    // We don't need an exact match nut a near one
    EXPECT_NEAR(result, 2639, 5); 
}

TEST(ACS712Test, TestZeroCurrent) {
    FakeArduinoBoard mock;
    ACS712 sensor(&mock, 1, ACS712_05B, 2000);

    mock.analogReadValue = 512;
    sensor.calibrate();
    
    EXPECT_EQ(sensor.getCurrentMA(), 0);
}