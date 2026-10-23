# Graphical User Interface

Information pertaining to communication between the host computer GUI and the BMS embedded system.

## DFR FIRMWARE CAN Standard

## BMS GUI CAN Protocol

### Charging commands

29-bit extended ID per the DFR CAN standard (`can_id_build`, target `NODE_BMS`).

| Command | ID field | Payload |
|---|---|---|
| `BMS_GUI_CONNECT` | `0xBB01` | none, starts a charging session |
| `BMS_GUI_CHARGING_REQUEST` | `0xBB02` | bytes 0-1 voltage setpoint in whole volts, bytes 2-3 current setpoint in whole amps, big-endian. Also acts as the session keepalive (3 s timeout) |
| `BMS_GUI_DISCONNECT` | `0xBB03` | none, ends the session |
| `BMS_GUI_CHARGING_SOFTSTOP` | `0xBBFF` | none, ends the session |

The BMS never requests power from the charger until it has received an in-range
setpoint in the current session. In range means
`LIMIT_CHARGER_VOLTAGE_MIN_V` <= V <= `LIMIT_PACK_CHARGE_VOLTAGE_MAX_V` and
`LIMIT_PACK_CHARGE_CURRENT_MIN_A` <= A <= `LIMIT_PACK_CHARGE_CURRENT_MAX_A`
(`bms/App/config/bms_limits.h`). An out-of-range setpoint is never clamped: the BMS
replies with `ERROR_ID_INVALID_SETPOINT` (`0xBE04`), cancels any active setpoint and
stops the charger. Ending the session also clears the setpoint.
