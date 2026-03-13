#include "imd.h"

/*
The ISO175 Insulation Monitoring Device constantly measures for
insulation between our Low Voltage Chassis and High Voltage Battery.

For the IMD safety circuit, it is solely controlled by the IMD's
~12V analog output nFault.

The IMD needs to be programmed over CAN, to establish fault thresholds,
and to set the "Power-On / Active Profile."

Default Profile:
    Baud rate: 500 kBaud
    Response value:
    100 kΩ (error)
    500 kΩ (warning)

Relevant rules:
1. 500 ohms of resistance per volt in HV pack: R_min = 500Ω * 600V = 300kΩ
2. Fault must be detected within 30 seconds
3. IMD's fault output can be ignored for a few seconds on startup, due to
   indeterminate state.
4. ??

Relevant CAN info
- Baud = 500kbit/s (default)
-


*/

const uint16_t min_insulation_resistance = 300000; // 300kΩ, maybe lower for FoS