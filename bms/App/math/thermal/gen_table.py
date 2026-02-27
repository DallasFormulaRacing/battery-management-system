#!/usr/bin/env python3
"""
Thermistor LUT generator (ADC-driven, truncated)

Thermistor P/N: NCG18WF104F0SRB

Whenever you change thermistors,

change the following:
- VMIN
- VMAX
- the polynomial function
"""

from typing import List, Tuple

# =========================
# Configuration
# =========================

V_MIN = 0.14
V_MAX = 2.91
DESIRED_RES = 10   # bits

ADC_RES = 16
ADC_LSB_V = 0.000150
ADC_OFFSET_V = 1.5

RSRV = ADC_RES - DESIRED_RES  # bits dropped

# =========================
# Derived bounds
# =========================

N_MIN = int((V_MIN - ADC_OFFSET_V) // ADC_LSB_V)
N_MAX = int((V_MAX - ADC_OFFSET_V) // ADC_LSB_V)

def adc16_to_adc10(code16: int) -> int:
    return code16 >> RSRV

N10_MIN = adc16_to_adc10(N_MIN)
N10_MAX = adc16_to_adc10(N_MAX)

# =========================
# Model
# =========================

def thermistor_poly(v: float) -> float:
    return (
        2.3487131 * v**8
        - 35.359734 * v**7
        + 218.27577 * v**6
        - 724.54830 * v**5
        + 1417.8324 * v**4
        - 1687.9102 * v**3
        + 1225.0384 * v**2
        - 565.64244 * v
        + 209.04676
    )

def adc16_to_voltage(code16: int) -> float:
    return ADC_OFFSET_V + ADC_LSB_V * code16

# =========================
# Table generation
# =========================

def make_table() -> List[Tuple[int, float, float]]:
    data: List[Tuple[int, float, float]] = []

    for code10 in range(N10_MIN, N10_MAX + 1):
        code16 = code10 << RSRV
        v = adc16_to_voltage(code16)

        if V_MIN <= v <= V_MAX:
            t = thermistor_poly(v)
            data.append((code10, v, t))

    return data

# =========================
# C/H generation
# =========================

def write_c_and_h(base_path: str, data: List[Tuple[int, float, float]]) -> None:
    h_path = f"{base_path}.h"
    c_path = f"{base_path}.c"

    base_code = data[0][0]
    size = len(data)

    # ---------- HEADER ----------
    with open(h_path, "w") as h:
        h.write("/* Auto-generated thermistor LUT */\n")
        h.write("/* DO NOT EDIT MANUALLY */\n\n")
        h.write("#ifndef THERMISTOR_H\n")
        h.write("#define THERMISTOR_H\n\n")

        h.write("#include <stdint.h>\n\n")

        h.write(f"#define THERM_LUT_SHIFT      {RSRV}\n")
        h.write(f"#define THERM_LUT_BASE_CODE {base_code}\n")
        h.write(f"#define THERM_LUT_SIZE      {size}\n\n")

        h.write("extern const float thermistor_lut[THERM_LUT_SIZE];\n\n")
        h.write("float thermistor_from_adc(int16_t adc16);\n\n")

        h.write("#endif /* THERMISTOR_H */\n")

    # ---------- SOURCE ----------
    with open(c_path, "w") as c:
        c.write("/* Auto-generated thermistor LUT */\n")
        c.write("/* DO NOT EDIT MANUALLY */\n\n")

        c.write("#include \"thermistor.h\"\n\n")

        c.write("const float thermistor_lut[THERM_LUT_SIZE] = {\n")
        for _, _, t in data:
            c.write(f"    {t:.6f}F,\n")
        c.write("};\n\n")

        c.write("float thermistor_from_adc(int16_t adc16)\n{\n")
        c.write("    int16_t adc10 = (int16_t)(adc16 >> THERM_LUT_SHIFT);\n")
        c.write("    int16_t idx   = (int16_t)(adc10 - THERM_LUT_BASE_CODE);\n\n")
        c.write("    if (idx < 0 || idx >= THERM_LUT_SIZE) {\n")
        c.write("        return -273.15F; /* invalid */\n")
        c.write("    }\n\n")
        c.write("    return thermistor_lut[idx];\n")
        c.write("}\n")

# =========================
# Main
# =========================

def main() -> None:
    data = make_table()

    print("ADC10 range :", data[0][0], "to", data[-1][0])
    print("Samples     :", len(data))
    print("Temp range  :", min(t for _, _, t in data),
          "to", max(t for _, _, t in data))

    write_c_and_h("App/math/thermal/thermistor", data)

if __name__ == "__main__":
    main()
