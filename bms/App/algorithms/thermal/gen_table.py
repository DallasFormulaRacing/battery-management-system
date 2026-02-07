#!/usr/bin/env python3
"""
Thermistor voltage-to-temperature model (ADC-driven)

Thermistor P/N: NCG18WF104F0SRB

Model:
  - Input: ADC code (signed integer)
  - Voltage: V = 1.5 + 0.000150 * code
  - Output: temperature (°C)
  - Polynomial valid only for V in [0.14, 2.91] volts

This script generates temperatures by iterating over ADC codes,
which guarantees contiguous indexing and avoids quantization jumps.
"""

from typing import List, Tuple

# USER EDITABLE
V_MIN = 0.14
V_MAX = 2.91

# USER EDITABLE
ADC_LSB_V = 0.000150
ADC_OFFSET_V = 1.5

ADC_MIN_CODE = -32768
ADC_MAX_CODE = 32767

ADC_RES = 16
DESIRED_RES = 10

N_MIN = int((V_MIN - ADC_OFFSET_V) // ADC_LSB_V)   # floor
N_MAX = int((V_MAX - ADC_OFFSET_V) // ADC_LSB_V)   # floor

RSRV = ADC_RES - DESIRED_RES

def thermistor_poly(v: float) -> float:
    """
    Voltage (V) -> Temperature (°C)
    Valid only for V in [V_MIN, V_MAX].
    """
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

def trunc(val: int) -> int:
    return val >> RSRV


def adc_code_to_voltage(code: int) -> float:
    if not (ADC_MIN_CODE <= code <= ADC_MAX_CODE):
        raise ValueError("ADC code out of int16 range")
    return ADC_OFFSET_V + ADC_LSB_V * code


def make_table() -> List[Tuple[int, float, float]]:
    """
    Returns a list of:
      (adc10_code, voltage, temperature)
    """
    data: List[Tuple[int, float, float]] = []

    N10_MIN = trunc(N_MIN)
    N10_MAX = trunc(N_MAX)

    for code10 in range(N10_MIN, N10_MAX + 1):
        code16 = code10 << RSRV
        v = adc_code_to_voltage(code16)

        if V_MIN <= v <= V_MAX:
            t = thermistor_poly(v)
            data.append((code10, v, t))

    return data

def c_header(HEADER_FILE: str, data: List[Tuple[int, float, float]]) -> None:
    base_code = data[0][0]          # lowest 10-bit ADC code
    table_len = len(data)

    with open(HEADER_FILE, "w") as f:
        f.write("/* Auto-generated thermistor lookup table */\n")
        f.write("/* DO NOT EDIT MANUALLY */\n\n")

        f.write("#ifndef THERM_LUT_H\n")
        f.write("#define THERM_LUT_H\n\n")

        f.write("#include <stdint.h>\n")
        f.write("#include <math.h>\n\n")

        f.write(f"#define THERM_LUT_BASE_CODE   ({base_code})\n")
        f.write(f"#define THERM_LUT_SIZE        ({table_len})\n")
        f.write(f"#define THERM_LUT_SHIFT       ({RSRV})\n\n")

        # ---- LUT declaration FIRST (must exist before use) ----
        f.write("static const float thermistor_lut[THERM_LUT_SIZE] = {\n")
        for _, _, t in data:
            f.write(f"    {t:.6f}F,\n")
        f.write("};\n\n")

        # ---- Runtime conversion function ----
        f.write("static inline float get_temp(int16_t adc16)\n")
        f.write("{\n")
        f.write("    int16_t adc10 = adc16 >> THERM_LUT_SHIFT;\n")
        f.write("    int32_t idx = adc10 - THERM_LUT_BASE_CODE;\n\n")
        f.write("    if (idx < 0 || idx >= THERM_LUT_SIZE) {\n")
        f.write("        return NAN;\n")
        f.write("    }\n\n")
        f.write("    return thermistor_lut[idx];\n")
        f.write("}\n\n")

        f.write("#endif /* THERM_LUT_H */\n")



def main() -> None:
    data = make_table()

    codes = [n for n, _, _ in data]
    volts = [v for _, v, _ in data]
    temps = [t for _, _, t in data]

    print("ADC code range        :", min(codes), "to", max(codes))
    print("Voltage range (V)     :", min(volts), "to", max(volts))
    print("Temperature range (°C):", min(temps), "to", max(temps))
    print("Total samples         :", len(data))

    print("\nFirst point:", data[0])
    print("Last point :", data[-1])

    # =========================
    # File output
    # =========================

    OUTPUT_FILE = "App/algorithms/thermal/table.txt"

    base = data[0][0]

    with open(OUTPUT_FILE, "w") as f:
        f.write("index,         voltage_V,          temperature_C\n")
        for n, v, t in data:
            f.write(f"{n - base},           {v:.6f},            {t:.6f}\n")

    print(f"\nWrote {len(data)} entries to {OUTPUT_FILE}")

    c_header("App/algorithms/thermal/thermistor_lut.h", data)

# =========================
# Entry point
# =========================

if __name__ == "__main__":
    main()
