# CAN2 to FD-VCAN Forwarding Policy

## Purpose

- Make sure VCAN can see TCAN messages for logging and maybe debugging

## Frame Mapping

- `IdType`: All must be extended for CAN2B
- `Identifier`: Need to change to adapt to overarching DFR VCAN standard
- `DLC`: stay same
- `Payload bytes`: stay same

## Timestamp Source

- Timestamp is captured at BMS receive time (software tick when frame is accepted on CAN2 RX path)

## Drop and Error Handling

- If forwarding cannot be queued/transmitted, increment drop counter
- RX handling on CAN2 must continue (do not block RX path indefinitely)
- Queue overflow increments queue-overrun counter
