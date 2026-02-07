## overview

this details how the thermistor look up table is to be implemented. recall our given input and desired output are voltage values and computed temperature, respectively.

regardless of implementation, this is always true:

a temperature measurement using thermistors is achieved by a function $f: V \rightarrow T$ and $f(V)$ is some polynomial from the datasheet. for demonstration purposes, we can use the 8th degree polynomial

$$
\begin{aligned}
f(x) = & - 64.372157 x^{8} + 870.54799 x^{7} - 4977.0485 x^{6} \\
& + 15653.825 x^{5} - 29509.773 x^{4} + 34000.193 x^{3} \\
& - 23267.623 x^{2} + 8547.5690 x - 1134.4749
\end{aligned}
$$

to achieve this, we have several options:

## option 1: on the go polynomial solving

solve the characteristic thermistor polynomial (note: this is not the Steinhart-Hart equation). for any given thermistor and its characteristic polynomial it's clear that a higher degree of said polynomial yields less error and a more accurate reading. no look up table implementation. computationally the most expensive, but requires little to no planning on our part

# option 2: linear interpolation with sparse LUT

take a sparsely populated look up table with linear interpolation for values in between elements of LUT. this is kind of like using the secant line's value to linearly estimate the value in the middle.

for example, lets say we have a voltage of 2.8839. our sparsely populated look up table has temperature-voltage ordered pair values for $(2.75V,40˚C)$ and $(3V,36˚C)$. in order to interpret this via linear interpolation, we would first calculate a secant line of the form $y=mx+b$ that intersects 2.75 and 3 (in the x axis, of course).

recall the point-slope formula gives a secant line of $y-40=m(x-2.75)$ with an $m$ value of $\frac{36-40}{3-2.75}=-16$. we now get the appropriate linear secant line in slope intercept form of $y=−16x+84$. plugging in our original voltage value of 2.8839 into $x$ we get a temperature of $37.8576˚C$.

clearly, having more elements in the sparsely populated array means our interpolation performs better. however, there is a searching aspect involved, which would be logarithmic at minimum. implementation-wise, this is the most complex; many moving parts. computationally this is the middle ground. and planning is again, very minimal.

# option 3: constant time look up; densely populated

recall that a floating-point number is represented in binary as defined by the IEEE 754 standard. if we were to cast said number into a fixed width integer, we would get the integral value of the binary that comprises the floating point number. note: not typecasting like programming, in this case, it is more similar to a `union` occupying the memory space of both a `float` and `int`, then reading the `int`.

the adbms returns values as raw 16bit packet which can be equivalently expressed as an integer or scaled into a floating point number, though this does not necessarily respect IEEE754. the point is that a bit pattern can be treated as a number without interpretation at runtime, and is free to be interpreted as anything as necessary.

for example, for the voltage value `2.568`:

```
Machine sees: 0x1bd0
Binary: 0b0001101111010000
Human reads: 2.568000
Equivalent integer of what machine sees (0x1bd0): 7120
```

the motivation of the above is that we can use the equivalent integer representation as the index, which has an equivalent floating point value for a voltage. then this cleanly maps to a temperature in the look up table.

without regard to relevant bounds or memory efficiency (yes i know at this moment we have 65 thousand indices), we can construct the look up table:

```
Integer index (recieved)
Temperature table[Integer index]
```

meaning that we can skip voltage parsing all together inside real-time firmware.

on the other hand, there is the matter of pre-generating the table externally with given values.

where we have all voltage values represented from `0x8000` to `0x7FFF`

for each index `0x8000` to `0x7FFF` we would calculate the equivalent float with a given characteristic polynomial

we are given a raw ADC code $N \in [-32768,32767]$, an interpreted voltage $V \in [−3.4152, 6.41505]$.

## real values we care about

from the thermistor datasheet, the voltage temperature curve is defined for $V \in [0.54,3]$ (generally, some arbitrary voltage values near the boundary of 0 and 3) which corresponds to a temperature range of $T \in [147.5˚C, -40˚C]$. range is a lot smaller than what we established with a 16bit resolution ADC value.

this gives us a raw ADC interval of $N \in [−6400, 10000]$ , which is correspondent to hexadecimal values of `0xE700` and `0x2710` respectively.

now at this point we could run through our temperature table generation as such:

```py
(pseudocode)
float temps[] = {...}

float thermistor_polynomial(float v)

for int i in 0xE700..0x2710
    v = convert_int_to_volt(i)
    t = thermistor_polynomial(v)
    temps[i - 0xE700] = t
```

this step is critical: we do not build a table for the full ADC range, only for the subset of codes that can actually occur for a valid thermistor measurement. everything outside this range is either unreachable or represents a fault condition (open circuit, short, saturation, etc.)

## table generation (offline)

the lookup table is generated offline, not in real-time firmware. For each valid raw ADC code:

1. The code is converted to a voltage using the function.
2. The datasheet-provided polynomial f(V) is evaluated.
3. The resulting temperature is stored in a table.

the subtraction of $Nmin⁡$ is purely an indexing convenience: C arrays are zero-based, so we shift the domain to make the first valid entry land at index 0. this offset has no physical meaning and is explicitly reversed at runtime.

### python implementation

Github link $\rightarrow$ [here](https://github.com/DallasFormulaRacing/battery-management-system/blob/feature/thermistors/bms/App/algorithms/thermal/gen_table.py)
File relpath $\rightarrow$ [here](App/algorithms/thermal/gen_table.py)
