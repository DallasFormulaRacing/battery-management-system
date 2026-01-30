#include "../FFF/fff.h" // TODO: Fix this in CMake
#include "bms_enums.h"
#include "bms_types.h"
#include "cb.h"
#include "parse.h"
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

/**
  * @brief Tests init_cell_balancing by setting the max delta to a positive 
  * number
  * 
  * @details A positive number is passed into init_cell_balancing. The 
  * maximum_cell_delta_allowed in the PCB struct should reflect the same 
  * number passed in.
  */
void testInitCellBalancingPositive(void) {
  init_cell_balancing(&testPcb, 100);
  TEST_ASSERT_EQUAL_INT16(100, testPcb.maximum_cell_delta_allowed);
}

/**
  * @brief Tests init_cell_balancing by setting the max delta to zero 
  * 
  * @details 0 is passed into init_cell_balancing. The 
  * maximum_cell_delta_allowed in the PCB struct should be 0
  */
void testInitCellBalancingZero(void) {
  init_cell_balancing(&testPcb, 0);
  TEST_ASSERT_EQUAL_INT16(0, testPcb.maximum_cell_delta_allowed);
}

/**
  * @brief Tests init_cell_balancing by setting the max delta to a negative 
  * number
  * 
  * @details A negative number is passed into init_cell_balancing. The 
  * maximum_cell_delta_allowed in the PCB struct should reflect the same 
  * number passed in.
  */
void testInitCellBalancingNegative(void) {
  init_cell_balancing(&testPcb, -3);
  TEST_ASSERT_EQUAL_INT16(-3, testPcb.maximum_cell_delta_allowed);
}

/**
  * @brief Tests function to find the lowest cell voltage in the PCB struct
  * if the first cell is the lowest voltage.
  *
  * @details The cell_voltage array is filled with voltage values starting
  * with 3000 and incrementing by 10 for however many cells are in a segment. 
  * The test will pass when the lowest cell found is 3000.
*/
void testFindCellDeltasStart(void) {
  for (uint16_t i = 0; i < NUM_CELL_MAX; ++i) {
    testPcb.batteries[i].cell_voltage = (voltage_readings_t)(3000 + (10 * i));
    testPcb.batteries[i].cell_number = i;
  }
  find_cell_deltas(&testPcb);
  TEST_ASSERT_EQUAL_INT16(3000, testPcb.lowest_cell.cell_voltage);
}

/**
  * @brief Tests function to find the lowest cell voltage in the PCB struct
  * if the last cell is the lowest voltage.
  *
  * @details The cell_voltage array is filled with voltage values starting
  * with 3000 and decrementing by 10 for however many cells are in a segment. 
  * The test will pass when the lowest cell found is 2890 in the last array 
  * element.
*/
void testFindCellDeltasEnd(void) {
  for (uint16_t i = 0; i < NUM_CELL_MAX; ++i) {
    testPcb.batteries[i].cell_voltage = (voltage_readings_t)(3000 - (10 * i));
    testPcb.batteries[i].cell_number = i;
  }
  find_cell_deltas(&testPcb);
  TEST_ASSERT_EQUAL_INT16(2890, testPcb.lowest_cell.cell_voltage);
}

/**
  * @brief Tests function to find the lowest cell voltage in the PCB struct
  * if the lowest voltage is in the middle of the array.
  *
  * @details The cell_voltage array is filled with voltage values starting
  * with 3000 and decrementing by 10 for however many cells are in a segment. 
  * Element 7 in the array will be set to 7, which is lower than the other 
  * voltages. The test will pass when the lowest cell found is 7 in the 7th 
  * element.
*/
void testFindCellDeltasMiddle(void) {
  for (uint16_t i = 0; i < NUM_CELL_MAX; ++i) {
    testPcb.batteries[i].cell_voltage = (voltage_readings_t)(3000 - (10 * i));
    testPcb.batteries[i].cell_number = i;
  }
  testPcb.batteries[7].cell_voltage = (voltage_readings_t)7;
  find_cell_deltas(&testPcb);
  TEST_ASSERT_EQUAL_INT16(7, testPcb.lowest_cell.cell_voltage);
}

/**
  * @brief TODO:
*/
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

static int16_t convert_human_readable_to_voltage(float voltage) {
  return (int16_t)((voltage - 1.5F) / 0.000150F);
}

/**
  * @brief Tests map_delta_to_pwm_discretize to find the appropriate PWM duty
  * cycle for the cell delta for when the cell delta is 0.
  *
  * @details Passes in a voltage of 0 and expects and duty cycle of 0.0%.
*/
void testMapDeltaToPwmDiscretizeZero(void) {
  testPcb.maximum_cell_delta_allowed = convert_human_readable_to_voltage(0.05F);
  TEST_ASSERT_EQUAL_INT(PWM_0_0_PERCENT_DUTY_CYCLE,
                        map_delta_to_pwm_discretize(
                            &testPcb, convert_human_readable_to_voltage(0.0F)));
}

/**
  * @brief Tests map_delta_to_pwm_discretize to find the appropriate PWM duty
  * cycle for the cell delta for when the cell delta is 70 mV.
  *
  * @details Passes in a voltage of 0.07V and expects and duty cycle of 66.0%.
*/
void testMapDeltaToPwmDiscretizeOne(void) {
  testPcb.maximum_cell_delta_allowed = convert_human_readable_to_voltage(0.05F);
  TEST_ASSERT_EQUAL_INT(
      PWM_66_0_PERCENT_DUTY_CYCLE,
      map_delta_to_pwm_discretize(
          &testPcb, (int16_t)convert_human_readable_to_voltage(0.07F)));
}

/**
  * @brief Tests map_delta_to_pwm_discretize to find the appropriate PWM duty
  * cycle for the cell delta for when the cell delta is greater than 100 mV.
  *
  * @details Passes in a voltage of 0.11V and expects and duty cycle of 100.0%.
*/
void testMapDeltaToPwmDiscretizeMax(void) {
  testPcb.maximum_cell_delta_allowed = convert_human_readable_to_voltage(0.01F);
  TEST_ASSERT_EQUAL_INT(
      PWM_100_0_PERCENT_DUTY_CYCLE,
      map_delta_to_pwm_discretize(
          &testPcb, (int16_t)convert_human_readable_to_voltage(0.11F)));
}

/**
  * @brief Tests map_delta_to_pwm_discretize to stop bleeding the cell when the 
  * input is lower than the max cell delta, signifying that the cell voltage is 
  * within the range of the max delta
  *
  * @details Passes in a voltage of 0.03V and expects and duty cycle of 0.0%.
*/
void testMapDeltaToPwmDiscretizeLowerThanMax(void) {
  testPcb.maximum_cell_delta_allowed = convert_human_readable_to_voltage(0.05F);
  TEST_ASSERT_EQUAL_INT(
      PWM_0_0_PERCENT_DUTY_CYCLE,
      map_delta_to_pwm_discretize(
          &testPcb, (int16_t)convert_human_readable_to_voltage(0.03)));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(testInitCellBalancingNegative);
  RUN_TEST(testInitCellBalancingPositive);
  RUN_TEST(testInitCellBalancingZero);
  RUN_TEST(testFindCellDeltasStart);
  RUN_TEST(testFindCellDeltasEnd);
  RUN_TEST(testFindCellDeltasMiddle);
  RUN_TEST(testMapDeltaToPwmDiscretizeZero);
  RUN_TEST(testMapDeltaToPwmDiscretizeOne);
  RUN_TEST(testMapDeltaToPwmDiscretizeMax);
  RUN_TEST(testMapDeltaToPwmDiscretizeLowerThanMax);
  return UNITY_END();
}
