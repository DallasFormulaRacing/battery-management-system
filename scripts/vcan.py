# CAN BUS Traffic simulator
import can
import time
import random
"""
typedef enum {
  CMD_ID_FIRST_24_CELLS = 0x00A0,
  CMD_ID_SECOND_24_CELLS = 0x00A1,
  CMD_ID_THIRD_24_CELLS = 0x00A2,
  CMD_ID_FOURTH_24_CELLS = 0x00A3,
  CMD_ID_FIFTH_24_CELLS = 0x00A4,
  CMD_ID_SIXTH_24_CELLS = 0x00A5,
  CMD_ID_FIRST_60_TEMPS = 0x00B0,
  CMD_ID_LAST_60_TEMPS = 0x00B1,
  CMD_ID_PACK_METADATA = 0x00C0,
  CMD_ID_IMD_DATA = 0x00D0,
} can_command_id_t;
"""
# needs to be extended
CMD_ID_FIRST_24_CELLS = 0x00A0
CMD_ID_SECOND_24_CELLS = 0x00A1
CMD_ID_THIRD_24_CELLS = 0x00A2
CMD_ID_FOURTH_24_CELLS = 0x00A3
CMD_ID_FIFTH_24_CELLS = 0x00A4
CMD_ID_SIXTH_24_CELLS = 0x00A5
CMD_ID_FIRST_60_TEMPS = 0x00B0
CMD_ID_LAST_60_TEMPS = 0x00B1
CMD_ID_PACK_METADATA = 0x00C0
CMD_ID_IMD_DATA = 0x00D0

bus = can.interface.Bus(channel='vcan0', bustype='socketcan')
arb_ids_list = [0x123, 0x124, 0x125, 0x126]
current_id = 0

while True:
    data = [0x00000000] + [random.randint(0, 255) for _ in range(4)]
    current_id += 1
    current_id = current_id % 4

    msg = can.Message(
        arbitration_id=arb_ids_list[current_id],
        data=data,
        is_extended_id=False
    )

    bus.send(msg)
    print(f"Sent: {data} to {arb_ids_list[current_id]}")
    time.sleep(0.05)


def generate_fake_cell_frame(number: int):
    return 0

def generate_fake_temp_frame(number: int):
    return 0

def generate_fake_pack_frame(number: int):
    return 0

def generate_fake_cell_value():
    return 0

def generate_fake_temp_value():
    return 0


