## Overview

This details how the thermistor look up table is to be implemented. Recall our given input and desired output are voltage values and computed temperature, respectively.

Regardless of implementation, this is always true:

A temperature measurement using thermistors is achieved by a function $f: v \rightarrow t$ and $f(v)$ is some polynomial from the datasheet. For demonstration purposes, we can use the 8th degree polynomial

$$
\begin{aligned}
F(x) = & - 64.372157 x^{8} + 870.54799 x^{7} - 4977.0485 x^{6} \\
& + 15653.825 x^{5} - 29509.773 x^{4} + 34000.193 x^{3} \\
& - 23267.623 x^{2} + 8547.5690 x - 1134.4749
\end{aligned}
$$

To achieve this, we have several options:

# Option 1: on the go polynomial solving

Solve the characteristic thermistor polynomial (note: this is not the steinhart-hart equation). For any given thermistor and its characteristic polynomial it's clear that a higher degree of said polynomial yields less error and a more accurate reading. No look up table implementation. Computationally the most expensive, but requires little to no planning on our part

# Option 2: linear interpolation with sparse lut

Take a sparsely populated look up table with linear interpolation for values in between elements of lut. This is kind of like using the secant line's value to linearly estimate the value in the middle.

For example, lets say we have a voltage of 2.8839. Our sparsely populated look up table has temperature-voltage ordered pair values for $(2.75v,40˚c)$ and $(3v,36˚c)$. In order to interpret this via linear interpolation, we would first calculate a secant line of the form $y=mx+b$ that intersects 2.75 and 3 (in the x axis, of course).

Recall the point-slope formula gives a secant line of $y-40=m(x-2.75)$ with an $m$ value of $\frac{36-40}{3-2.75}=-16$. We now get the appropriate linear secant line in slope intercept form of $y=−16x+84$. Plugging in our original voltage value of 2.8839 into $x$ we get a temperature of $37.8576˚c$.

Clearly, having more elements in the sparsely populated array means our interpolation performs better. However, there is a searching aspect involved, which would be logarithmic at minimum. Implementation-wise, this is the most complex; many moving parts. Computationally this is the middle ground. And planning is again, very minimal.

# Option 3: constant time look up; densely populated

Recall that a floating-point number is represented in binary as defined by the ieee 754 standard. If we were to cast said number into a fixed width integer, we would get the integral value of the binary that comprises the floating point number. Note: not typecasting like programming, in this case, it is more similar to a `union` occupying the memory space of both a `float` and `int`, then reading the `int`.

The adbms returns values as raw 16bit packet which can be equivalently expressed as an integer or scaled into a floating point number, though this does not necessarily respect ieee754. The point is that a bit pattern can be treated as a number without interpretation at runtime, and is free to be interpreted as anything as necessary.

For example, for the voltage value `2.568`:

```
Machine sees: 0x1bd0
Binary: 0b0001101111010000
Human reads: 2.568000
Equivalent integer of what machine sees (0x1bd0): 7120
```

The motivation of the above is that we can use the equivalent integer representation as the index, which has an equivalent floating point value for a voltage. Then this cleanly maps to a temperature in the look up table.

Without regard to relevant bounds or memory efficiency (yes I know at this moment we have 65 thousand indices), we can construct the look up table:

```
Integer index (recieved)
Temperature table[integer index]
```

Meaning that we can skip voltage parsing all together inside real-time firmware.

On the other hand, there is the matter of pre-generating the table externally with given values.

Where we have all voltage values represented from `0x8000` to `0x7fff`

For each index `0x8000` to `0x7fff` we would calculate the equivalent float with a given characteristic polynomial

We are given a raw adc code $n \in [-32768,32767]$, an interpreted voltage $v \in [−3.4152, 6.41505]$.

## Real values we care about

From the thermistor datasheet, the voltage temperature curve is defined for $v \in [0.54,3]$ (generally, some arbitrary voltage values near the boundary of 0 and 3) which corresponds to a temperature range of $t \in [147.5˚c, -40˚c]$. Range is a lot smaller than what we established with a 16bit resolution adc value.

This gives us a raw adc interval of $n \in [−6400, 10000]$ , which is correspondent to hexadecimal values of `0xe700` and `0x2710` respectively.

Now at this point we could run through our temperature table generation as such:

```py
(pseudocode)
Float temps[] = {...}

Float thermistor_polynomial(float v)

For int I in 0xe700..0x2710
    V = convert_int_to_volt(I)
    T = thermistor_polynomial(v)
    Temps[I - 0xe700] = t
```

This step is critical: we do not build a table for the full adc range, only for the subset of codes that can actually occur for a valid thermistor measurement. Everything outside this range is either unreachable or represents a fault condition (open circuit, short, saturation, etc.)

## Table generation (offline)

The lookup table is generated offline, not in real-time firmware. For each valid raw adc code:

1. The code is converted to a voltage using the function.
2. The datasheet-provided polynomial f(v) is evaluated.
3. The resulting temperature is stored in a table.

The subtraction of $nmin⁡$ is purely an indexing convenience: c arrays are zero-based, so we shift the domain to make the first valid entry land at index 0. This offset has no physical meaning and is explicitly reversed at runtime.

### Python implementation

Github link $\rightarrow$ [here](https://github.Com/dallasformularacing/battery-management-system/blob/feature/thermistors/bms/app/algorithms/thermal/gen_table.Py)

File relpath $\rightarrow$ [here](app/algorithms/thermal/gen_table.Py)

## Open-Wire Fault

> [!NOTE]
> TODO: Which GPIO?

In the event of a thermistor being disconnected or broken in an open-wire fault, a `GPIO pin` will be set to LOW,
triggering the shutdown circuit and shutting down the high-voltage components of the car.

The `therm_ow_pull_up_array` inside `aux_openwire_t` in the `cell_asic_ctx_t` struct is an array of bool values
indicating which thermistors on the AUX ADC lines have had an open-wire fault, with true elements representing a fault
in its representative AUX ADC line, and the default values being false for no open-wire fault.

In the wake of an open-wire fault, you can view the `therm_ow_pull_up_array` and see which elements are set to true. 
Keep in mind that the array elements start with 0 and end with 9, with every element representing its thermistor - 1.
For example, if element 6 in `therm_ow_pull_up_array` was set to true, then thermistor 7 recently had an open-wire 
fault.

`therm_ow_pull_up_array` will only be cleared upon restarting the BMS.
