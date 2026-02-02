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
  TEST_ASSERT_EQUAL_INT16(75, get_temp_from_resistance(1100));
}

void testGetTempFromResistance1050(void) {
  TEST_ASSERT_EQUAL_INT16(100, get_temp_from_resistance(1050));
}

void testGetTempFromResistance500(void) {
  TEST_ASSERT_EQUAL_INT16(125, get_temp_from_resistance(500));
}

void testGetTempFromResistance80000(void) {
  TEST_ASSERT_EQUAL_INT16(0, get_temp_from_resistance(80000));
}

void testGetTempFromResistance91000(void) {
  TEST_ASSERT_EQUAL_INT16(-25, get_temp_from_resistance(91000));
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
  return UNITY_END();
}
