#include "thermistor.h"
#include "unity.h"
#include "unity_internals.h"

void setUp() {}
void tearDown() {}

static int16_t convert_human_readable_to_voltage(float voltage) {
  return (int16_t)((voltage - 1.5F) / 0.000150F);
}

void testGetTempFromResistanceZero(void) {
  TEST_ASSERT_EQUAL_INT16(150, get_temp_from_resistance(0));
}

void testGetTempFromResistanceLess(void) {
  TEST_ASSERT_EQUAL_INT16(150, get_temp_from_resistance(100));
}

void testGetTempFromResistanceGreater(void) {
  TEST_ASSERT_EQUAL_INT16(-50, get_temp_from_resistance(130000));
}

void testGetTempFromResistance1100(void) {
  TEST_ASSERT_INT16_WITHIN(5, 90, get_temp_from_resistance(1100));
}

void testGetTempFromResistance1050(void) {
  TEST_ASSERT_INT16_WITHIN(5, 95, get_temp_from_resistance(1050));
}

void testGetTempFromResistance500(void) {
  TEST_ASSERT_INT16_WITHIN(5, 120, get_temp_from_resistance(500));
}

void testGetTempFromResistance80000(void) {
  TEST_ASSERT_LESS_OR_EQUAL_INT16(-25, get_temp_from_resistance(80000));
}

void testGetTempFromResistance91000(void) {
  TEST_ASSERT_LESS_OR_EQUAL_INT16(-25, get_temp_from_resistance(91000));
}

void testGetTempFromResistance16000(void) {
  TEST_ASSERT_INT16_WITHIN(5, 10, get_temp_from_resistance(16000));
}

void testGetTempFromResistance3000(void) {
  TEST_ASSERT_INT16_WITHIN(5, 57, get_temp_from_resistance(3000));
}

void testGetTempFromResistance10000(void) {
  TEST_ASSERT_INT16_WITHIN(5, 25, get_temp_from_resistance(10000));
}

void testGetTempFromResistance10500(void) {
  TEST_ASSERT_INT16_WITHIN(5, 17, get_temp_from_resistance(10500));
}

int main(void) {
  RUN_TEST(testGetTempFromResistanceZero);
  RUN_TEST(testGetTempFromResistanceLess);
  RUN_TEST(testGetTempFromResistanceGreater);
  RUN_TEST(testGetTempFromResistance1100);
  RUN_TEST(testGetTempFromResistance1050);
  RUN_TEST(testGetTempFromResistance500);
  RUN_TEST(testGetTempFromResistance80000);
  RUN_TEST(testGetTempFromResistance91000);
  RUN_TEST(testGetTempFromResistance16000);
  RUN_TEST(testGetTempFromResistance3000);
  RUN_TEST(testGetTempFromResistance10000);
  RUN_TEST(testGetTempFromResistance10500);
  return UNITY_END();
}
