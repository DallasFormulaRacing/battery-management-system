#!/usr/bin/env python3
"""
How to use: 
1. Get the thermistor data sheet. 
    The info you need is the temperature approx graph, 
    the polynomial for that, and the min and max voltage values (in the domain)

2. Edit V_MIN and V_MAX appropriately.

3. Redefine thermistor_poly(v: float) -> float appropriately

4. Run generator

Thermistor LUT generator (ADC-driven, truncated)

Thermistor P/N: NCG18WF104F0SRB
"""

from typing import List, Tuple

# Configuration (user-editable)
V_MIN = 0.14
V_MAX = 2.91
DESIRED_RES = 10   # bits

# ADC characteristics (fixed)
ADC_RES = 16
ADC_LSB_V = 0.000150
ADC_OFFSET_V = 1.5
ADC_MIN_CODE = -32768
ADC_MAX_CODE = 32767
RSRV = ADC_RES - DESIRED_RES  # bits dropped

# Derived bounds

N_MIN = int((V_MIN - ADC_OFFSET_V) // ADC_LSB_V)
N_MAX = int((V_MAX - ADC_OFFSET_V) // ADC_LSB_V)

def adc16_to_adc10(code16: int) -> int:
    """Truncate ADC code by discarding LSBs (floor quantization)."""
    return code16 >> RSRV

N10_MIN = adc16_to_adc10(N_MIN)
N10_MAX = adc16_to_adc10(N_MAX)

# =========================
# Model
# =========================

def thermistor_poly(v: float) -> float:
    """Voltage (V) -> Temperature (°C). Valid only in [V_MIN, V_MAX]."""
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

# Table generation
def make_table() -> List[Tuple[int, float, float]]:
    """
    Returns (adc10_code, voltage, temperature)
    """
    data: List[Tuple[int, float, float]] = []

    for code10 in range(N10_MIN, N10_MAX + 1):
        code16 = code10 << RSRV
        v = adc16_to_voltage(code16)

        if V_MIN <= v <= V_MAX:
            t = thermistor_poly(v)
            data.append((code10, v, t))

    return data

# C header generation
def write_c_header(path: str, data: List[Tuple[int, float, float]]) -> None:
    base_code = data[0][0]
    size = len(data)

    with open(path, "w") as f:
        f.write("/* Auto-generated thermistor LUT */\n")
        f.write("/* DO NOT EDIT MANUALLY */\n\n")
        f.write("#ifndef THERM_LUT_H\n#define THERM_LUT_H\n\n")
        f.write("#include <stdint.h>\n\n")

        f.write(f"#define THERM_LUT_BASE_CODE {base_code}\n")
        f.write(f"#define THERM_LUT_SHIFT     {RSRV}\n")
        f.write("enum { THERM_LUT_SIZE = %d };\n\n" % size)

        f.write("static const float thermistor_lut[THERM_LUT_SIZE] = {\n")
        for _, _, t in data:
            f.write(f"    {t:.6f}F,\n")
        f.write("};\n\n")

        f.write("static inline float thermistor_from_adc(int16_t adc16)\n{\n")
        f.write("    int16_t adc10 = (int16_t)(adc16 >> THERM_LUT_SHIFT);\n")
        f.write("    int16_t idx = (int16_t)(adc10 - THERM_LUT_BASE_CODE);\n\n")
        f.write("    if (idx < 0 || idx >= THERM_LUT_SIZE) {\n")
        f.write("        return -273.15F; /* invalid */\n")
        f.write("    }\n\n")
        f.write("    return thermistor_lut[idx];\n}\n\n")
        f.write("#endif /* THERM_LUT_H */\n")

# Main
def main() -> None:
    data = make_table()

    print("ADC10 range :", data[0][0], "to", data[-1][0])
    print("Samples     :", len(data))
    print("Temp range  :", min(t for _, _, t in data),
          "to", max(t for _, _, t in data))

    write_c_header("App/algorithms/thermal/thermistor_lut.h", data)

if __name__ == "__main__":
    main()
