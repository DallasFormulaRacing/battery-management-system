# CAN BUS Traffic simulator
import can
import time
import random
import fixedInt

# DFR CAN Standard — BMS GUI Application Commands
BMS_BATTERY_PACK_DATA = 0xB000
BMS_CELL_VOLTAGES_PACK_1 = 0xB101
BMS_CELL_VOLTAGES_PACK_2 = 0xB102
BMS_CELL_VOLTAGES_PACK_3 = 0xB103
BMS_CELL_VOLTAGES_PACK_4 = 0xB104
BMS_CELL_VOLTAGES_PACK_5 = 0xB105
BMS_CELL_VOLTAGES_PACK_6 = 0xB106
BMS_SEGMENT_TEMPS_HALF_1 = 0xB111
BMS_SEGMENT_TEMPS_HALF_2 = 0xB112
BMS_IMD_DATA = 0xBA01

bus = can.interface.Bus(channel='vcan0', bustype='socketcan')
# arb_ids_list = [0x123, 0x124, 0x125, 0x126]
# current_id = 0


while True:
    send(generate_fake_cell_frame(), BMS_CELL_VOLTAGES_PACK_1)
    send(generate_fake_cell_frame(), BMS_CELL_VOLTAGES_PACK_2)
    send(generate_fake_cell_frame(), BMS_CELL_VOLTAGES_PACK_3)
    send(generate_fake_cell_frame(), BMS_CELL_VOLTAGES_PACK_4)
    send(generate_fake_cell_frame(), BMS_CELL_VOLTAGES_PACK_5)
    send(generate_fake_cell_frame(), BMS_CELL_VOLTAGES_PACK_6)
    send(generate_fake_temp_frame(), BMS_SEGMENT_TEMPS_HALF_1)
    send(generate_fake_temp_frame(), BMS_SEGMENT_TEMPS_HALF_2)
    send(generate_fake_pack_frame(), BMS_BATTERY_PACK_DATA)
    

def send(data_in, CAN_ID):
    msg = can.Message(
        arbitration_id=CAN_ID,
        data=data_in,
        is_extended_id=True
    )
    bus.send(msg)
    print(f"Sent: {data} to {CAN_ID}")
    time.sleep(0.05)

# 48 bytes, 2 bytes per cell
def generate_fake_cell_frame():
    data_array = [0x00,0x00,0x00,0x00]
    for i in range(60):
        j = i+4
        data_array[4] = generate_fake_cell_value()
        
    data_buffer = bytes(data_array) 
    return data_buffer

# 64 bytes, 4 byte padding, 1 byte per temp
def generate_fake_temp_frame():
    return 0

# 24 bytes, 2, 2, 2, 18
def generate_fake_pack_frame():
    return 0

def f2i(n: float):
    return fixedInt.Int16(n/0.00015 - 1.5)


# return float (actual voltage) as int16_t with lsb = 150microVolt +/- 1.5
def generate_fake_cell_value():
    val = f2i(random.randint(2900, 4100)/1000.0)
    return fixedInt.Int16()

#return uint8_t
def generate_fake_temp_value():
    random.randint(2900, 4100)  
    return 0



# while True:
#     data = [0x00000000] + [random.randint(0, 255) for _ in range(4)]
#     current_id += 1
#     current_id = current_id % 4

#     msg = can.Message(
#         arbitration_id=arb_ids_list[current_id],
#         data=data,
#         is_extended_id=False
#     )

#     bus.send(msg)
#     print(f"Sent: {data} to {arb_ids_list[current_id]}")
#     time.sleep(0.05)