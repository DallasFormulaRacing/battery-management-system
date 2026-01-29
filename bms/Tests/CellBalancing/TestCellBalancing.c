#include "../FFF/fff.h" // TODO: Fix this in CMake
#include "bms_types.h"
#include "cb.h"
#include "queue.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define TEST_NUM_ICS 4

static pcb_ctx_t testPcb;
static cell_asic_ctx_t testAsicCtx[TEST_NUM_ICS];

void setUp() {
  memset(&testPcb, 0, sizeof(pcb_ctx_t));
  memset(&testAsicCtx, 0, sizeof(testAsicCtx));
}

void tearDown() {}

void testInitCellBalancingPositive(void) {
  init_cell_balancing(&testPcb, 100);
  TEST_ASSERT_EQUAL_INT16(100, testPcb.maximum_cell_delta_allowed);
}

void testInitCellBalancingZero(void) {
  init_cell_balancing(&testPcb, 0);
  TEST_ASSERT_EQUAL_INT16(0, testPcb.maximum_cell_delta_allowed);
}

void testInitCellBalancingNegative(void) {
  init_cell_balancing(&testPcb, -3);
  TEST_ASSERT_EQUAL_INT16(-3, testPcb.maximum_cell_delta_allowed);
}

void testFindCellDeltasStart(void) {
  for (uint16_t i = 0; i < NUM_CELL_MAX; ++i) {
    testPcb.batteries[i].cell_voltage = (voltage_readings_t)(3000 + (10 * i));
    testPcb.batteries[i].cell_number = i;
  }
  find_cell_deltas(&testPcb);
  TEST_ASSERT_EQUAL_INT16(3000, testPcb.lowest_cell.cell_voltage);
}

void testFindCellDeltasEnd(void) {
  for (uint16_t i = 0; i < NUM_CELL_MAX; ++i) {
    testPcb.batteries[i].cell_voltage = (voltage_readings_t)(3000 - (10 * i));
    testPcb.batteries[i].cell_number = i;
  }
  find_cell_deltas(&testPcb);
  TEST_ASSERT_EQUAL_INT16(2890, testPcb.lowest_cell.cell_voltage);
}

void testFindCellDeltasMiddle(void) {
  for (uint16_t i = 0; i < NUM_CELL_MAX; ++i) {
    testPcb.batteries[i].cell_voltage = (voltage_readings_t)(3000 - (10 * i));
    testPcb.batteries[i].cell_number = i;
  }
  testPcb.batteries[7].cell_voltage = (voltage_readings_t)7;
  find_cell_deltas(&testPcb);
  TEST_ASSERT_EQUAL_INT16(7, testPcb.lowest_cell.cell_voltage);
}

void testCopyCellVoltages(void) {
  voltage_readings_t voltage = 2000;
  for (uint16_t i = 0; i < TEST_NUM_ICS; ++i) {
    for (uint16_t j = 0; j < NUM_CELLS_PER_SEGMENT; ++j) {
      testAsicCtx[i].cell.cell_voltages_array[j] = voltage;
      voltage += 10;
    }
  }
  copy_cell_voltages(testAsicCtx, &testPcb);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(testInitCellBalancingNegative);
  RUN_TEST(testInitCellBalancingPositive);
  RUN_TEST(testInitCellBalancingZero);
  RUN_TEST(testFindCellDeltasStart);
  RUN_TEST(testFindCellDeltasEnd);
  RUN_TEST(testFindCellDeltasMiddle);
  return UNITY_END();
}
