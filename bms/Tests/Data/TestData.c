#include "../FFF/fff.h"
#include "bms_enums.h"
#include "command_list.h"
#include "config.h"
#include "data.h"
#include "unity.h"
#include "unity_internals.h"

DEFINE_FFF_GLOBALS;

cell_asic_ctx_t asic_ctx[IC_COUNT_CHAIN];
uint8_t write_buffer[WRITE_SIZE];

// Mock HAL types
typedef struct {
  int dummy;
} SPI_HandleTypeDef;
typedef struct {
  int dummy;
} I2C_HandleTypeDef;
typedef struct {
  int dummy;
} UART_HandleTypeDef;

// Mock HAL handles
SPI_HandleTypeDef hspi1;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef hlpuart1;

// Define FFF fakes for HAL functions
FAKE_VOID_FUNC1(HAL_Delay, uint32_t);
FAKE_VOID_FUNC3(HAL_GPIO_WritePin, void *, uint16_t, uint8_t);
FAKE_VOID_FUNC4(HAL_SPI_Transmit, SPI_HandleTypeDef *, uint8_t *, uint16_t,
                uint32_t);
FAKE_VOID_FUNC4(HAL_SPI_Receive, SPI_HandleTypeDef *, uint8_t *, uint16_t,
                uint32_t);
FAKE_VOID_FUNC4(HAL_UART_Transmit, UART_HandleTypeDef *, uint8_t *, uint16_t,
                uint32_t);

void setUp() {
  RESET_FAKE(HAL_Delay);
  RESET_FAKE(HAL_GPIO_WritePin);
  RESET_FAKE(HAL_SPI_Transmit);
  RESET_FAKE(HAL_SPI_Receive);
  RESET_FAKE(HAL_UART_Transmit);
}

void tearDown() {}

void test_switch_group_cfg(void) {
  TEST_ASSERT_EQUAL_INT(CFG_REG_GROUP_A, switch_group_cfg(REG_GROUP_A));
  TEST_ASSERT_EQUAL_INT(CFG_REG_GROUP_B, switch_group_cfg(REG_GROUP_B));
  TEST_ASSERT_EQUAL_INT(CFG_REG_GROUP_C, switch_group_cfg(REG_GROUP_C));
  TEST_ASSERT_EQUAL_INT(CFG_REG_GROUP_D, switch_group_cfg(REG_GROUP_D));
  TEST_ASSERT_EQUAL_INT(CFG_REG_GROUP_E, switch_group_cfg(REG_GROUP_E));
  TEST_ASSERT_EQUAL_INT(CFG_REG_GROUP_F, switch_group_cfg(REG_GROUP_F));
  TEST_ASSERT_EQUAL_INT(ALL_CFG_REG_GROUPS, switch_group_cfg(ALL_REG_GROUPS));
  // TODO: Guard againt implicit conversions
  // TEST_ASSERT_EQUAL_INT(NO_CFG_REG_GROUP,
  // switch_group_cfg(NO_PWM_REG_GROUP));
  TEST_ASSERT_EQUAL_INT(NO_CFG_REG_GROUP, switch_group_cfg(NO_REG_GROUP));
}

void test_bms_read_data(void) {
  TEST_ASSERT_EQUAL_INT(
      COMM_OK, bms_read_data(asic_ctx, BMS_CMD_RDCVALL, RDCVALL, REG_GROUP_A));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_switch_group_cfg);
  RUN_TEST(test_bms_read_data);
  return UNITY_END();
}
