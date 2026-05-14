// #include <Arduino.h>
#include <unity.h>

void test_add() {
    TEST_ASSERT_EQUAL(5, 5);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    return UNITY_END();
}