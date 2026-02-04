#include "thermistor.h"
#include "unity.h"
#include "unity_internals.h"

void setUp() {}
void tearDown() {}

static int16_t convert_human_readable_to_voltage(float voltage) {
  return (int16_t)((voltage - 1.5F) / 0.000150F);
}

void testGetTempFromVoltageZero(void) {
  TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.793969773, get_temp_from_voltage(0.0));
}

void testGetTempFromVoltageLess(void) {
  TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.793969773, get_temp_from_voltage(-400.0));
}

void testGetTempFromVoltageGreater(void) {
  TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(100.753857,
                                     get_temp_from_voltage(13000.0));
}

void testGetTempFromVoltage2_6(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.11, 28.25, get_temp_from_voltage(2.6));
}

void testGetTempFromVoltage1_7(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.11, 68.5, get_temp_from_voltage(1.7));
}

// dont say it, dont think about it Sebastian
void testGetTempFromVoltage1_6767676767(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.11, 69.4, get_temp_from_voltage(1.6767676767));
}

void testGetTempFromVoltage2_886662(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.001, 1.793969773, get_temp_from_voltage(2.886662));
}

void testGetTempFromVoltage0_918936968(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.001, 100.753857,
                           get_temp_from_voltage(0.9189369682));
}

int main(void) {
  RUN_TEST(testGetTempFromVoltageZero);
  RUN_TEST(testGetTempFromVoltageLess);
  RUN_TEST(testGetTempFromVoltageGreater);
  RUN_TEST(testGetTempFromVoltage2_6);
  RUN_TEST(testGetTempFromVoltage1_7);
  RUN_TEST(testGetTempFromVoltage1_6767676767);
  RUN_TEST(testGetTempFromVoltage2_886662);
  RUN_TEST(testGetTempFromVoltage0_918936968);
  return UNITY_END();
}
