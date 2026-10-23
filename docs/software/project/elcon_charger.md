# Elcon TC HK-L charger CAN

Charger: Elcon TC HK-L 6.6 kW, standard CAN protocol No. 1000.
DBC: [`dbc/elcon_tc_hk_l.dbc`](dbc/elcon_tc_hk_l.dbc). Code: `bms/App/lib/elcon`.

## Bus

- **500 kbit/s.** The datasheet says 250 kbit/s, but our charger runs at 500k.
- Classic CAN 2.0B, 29-bit extended IDs, DLC 8, big-endian values.
- Lives on FDCAN1 (called `can2_*` in the code, shared with the IMD). FDCAN1 is
  160 MHz / 4 / 8 = 5 MHz, 1 + 4 + 5 tq = 500 kbit/s (sample point 50%).
- The charger does NOT have a CAN termination.
- Signal connector Molex 348302001: pin 11 CANH, pin 12 CANL, pin 17 GND.

## Frames

| Name                       | Hex      | Description                        | DLC   | Period | Data Description                    |
| -------------------------- | -------- |  ---------------------------------- | ----- | ----- | ----------------------------------- |
| BMS_TO_ELCON_CHARGER | `0x1806E5F4` | Send charge limits and start/stop controls to Elcon Charger | 8 | 200ms | `0-1` max voltage 0.1 V/bit, `2-3` max current 0.1 A/bit, `4` control (0 start, 1 stop), `5-7` reserved |
| ELCON_CHARGER_STATUS | `0x1806E5F4` | Elcon Charger's cyclical message power and status  | 8 | 1000ms |`0-1` output voltage 0.1 V/bit, `2-3` output current 0.1 A/bit (bit 15 = 1 discharging), `4` status, `5-7` reserved |
| -------- | -------- | Charger Status Byte, `1` = fault | 1 | 1000ms | `0` = HW Fault, `1` Over Temperature, `2` AC Voltage Wrong, `3` Battery not connected, `4` BMS Communication timeout (>5sec), `5`|


If the charger gets no request for 5sec, it sets sets comm fault status bit high, and stops charging.

## Firmware behavior

- The charging FSM runs every 200 ms and sends a request every tick (faster
  than the 1 s minimum, so stops reach the charger quickly).
- No power is requested until the GUI sends an in-range setpoint (see
  [gu_interface.md](gu_interface.md)). Pack limits (600 V / 10 A) and charger
  hardware limits live in `bms/App/config/bms_limits.h`.
- `elcon_pack_can` clamps every request to the charger rating (680 V / 14 A) as
  a last line of defense. The charger is power limited (6.6 kW on 220 VAC,
  3.3 kW on 110 VAC), so it derates current at high voltage on its own.
- When the BMS leaves the charging state (session disabled, GUI timeout, or
  BMS fault) `charger_supervisor_stop` sends an explicit stop frame instead of
  waiting for the charger's 5 s timeout.
- A status broadcast older than 5 s blocks charging (`is_elcon_ready`).
