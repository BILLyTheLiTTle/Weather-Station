#include <gtest/gtest.h>
#include "../lib/display/controller/ButtonSensor.h"
#include "../lib/display/controller/ButtonSensor.cpp"
#include "../../fake_hal/FakeArduinoBoard.h"

void advanceTime(FakeArduinoBoard& hw, ButtonSensor& button, uint32_t ms) {
    // Αντί για απότομο άλμα, προχωράμε 1ms τη φορά και καλούμε την update()
    // για να προσομοιώσουμε τη συνεχή ροή της loop() του Arduino
    for (uint32_t i = 0; i < ms; i++) {
        hw.currentTimeMs++;
        button.update();
    }
}

// --- TEST CASES ---

// 1. Έλεγχος ότι ο θόρυβος (bouncing) μικρότερος από το debounceMs αγνοείται
TEST(ButtonSensorTest, DebounceIgnoresNoise) {
    FakeArduinoBoard hw;
    ButtonSensor button(&hw, 2, 50, 200, 1000); // debounce: 50ms

    hw.digitalReadValue = LOW;
    advanceTime(hw, button, 20); 

    EXPECT_FALSE(button.isPressed());
    
    advanceTime(hw, button, 40);
    EXPECT_TRUE(button.isPressed());
}

// 2. Έλεγχος για Απλό Κλικ (Single Click)
TEST(ButtonSensorTest, DetectsSingleClick) {
    FakeArduinoBoard hw;
    ButtonSensor button(&hw, 2, 50, 200, 1000); // doubleClick window: 200ms

    hw.digitalReadValue = LOW;
    advanceTime(hw, button, 60); 
    
    hw.digitalReadValue = HIGH;
    advanceTime(hw, button, 60); 
    
    EXPECT_EQ(button.getEvent(), BUTTON_NONE); 

    advanceTime(hw, button, 250);
    
    EXPECT_EQ(button.getEvent(), BUTTON_CLICK);
}

// 3. Έλεγχος για Διπλό Κλικ (Double Click)
TEST(ButtonSensorTest, DetectsDoubleClick) {
    FakeArduinoBoard hw;
    ButtonSensor button(&hw, 2, 50, 200, 1000);

    // 1ο Πάτημα και Άφεση
    hw.digitalReadValue = LOW;  advanceTime(hw, button, 60);
    hw.digitalReadValue = HIGH; advanceTime(hw, button, 60);

    advanceTime(hw, button, 50); 
    
    // 2ο Πάτημα και Άφεση
    hw.digitalReadValue = LOW;  advanceTime(hw, button, 60);
    hw.digitalReadValue = HIGH; advanceTime(hw, button, 60);

    EXPECT_EQ(button.getEvent(), BUTTON_DOUBLE_CLICK);
}

// 4. Έλεγχος για Παρατεταμένο Κλικ (Long Click)
TEST(ButtonSensorTest, DetectsLongClick) {
    FakeArduinoBoard hw;
    ButtonSensor button(&hw, 2, 50, 200, 1000); // longClick: 1000ms

    hw.digitalReadValue = LOW;
    advanceTime(hw, button, 60); 
    
    advanceTime(hw, button, 1100);
    
    hw.digitalReadValue = HIGH;
    advanceTime(hw, button, 60);

    EXPECT_EQ(button.getEvent(), BUTTON_LONG_CLICK);
}