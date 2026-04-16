#include "elcon_comms.h"
#include "elcon_types.h"

void elcon_send_command(elcon_command_t *command_profile) {
  //
}

static inline void elcon_pack_can(uint8_t *msg) {
  //
}

static inline void elcon_unpack_status_byte(uint8_t stb, elcon_status_t *stat) {
  //
  stat->hw = stb & (1 << 0);
  stat->temp = stb & (1 << 1);
  stat->input_voltage = stb & (1 << 2);
  stat->starting_state = stb & (1 << 3);
  stat->comm_state = stb & (1 << 4);
}