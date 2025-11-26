# State of Charge (SoC) Estimation for Electric Vehicle Battery Management Systems

```
Author(s): Will Kim     ...   <wkim@utdallas.edu>

           First Last   ...   <name@utdallas.edu> (for future maintainers)
```

## 1. Introduction

### Abstract

Accurate estimation of a battery's state of charge (SoC) and state of health (SoH) is important for the reliability, safety, and performance of modern battery management systems (BMS) in electric vehicles (EV). Limitations of standalone sensor and monitoring measurements include sensor noise, measurement drift and generally do not offer enough data to reliably report the charge and health of the batteries. Sensor fusion techniques, such as the extended Kalman filter (EKF), have become essential in addressing the limitations of individual sensing methods by integrating data from voltage, current, and temperature sensors with predictive battery models. This document provides a structured overview of SoC and SoH estimation methods, including both Coulomb Counting (CC) and EKF-based approaches. We begin with a summary of simpler approaches, such as coulomb counting and open-circuit voltage methods, highlighting their shortcomings in dynamic operating conditions. We then present advanced methods, including EKF, capable of fusing non-linear battery models with sensor data to continuously estimate internal states and adapt to varying operational scenarios. Practical implementation challenges, including initialization, model fidelity, noise characterization, and computational cost on embedded systems, are also addressed. This work aims to serve as a foundational guide for engineers seeking to implement or refine SoC estimation in BMS applications.

### Similar work and relevant literature

The extended Kalman filter has seen success in mission critical aerospace applications, such as NASA's Apollo lunar navigator and in modern spacecraft guidance systems [1]. Since then, NASA has used EKFs across dozens of missions, from Space Shuttle navigation to interplanetary robotic vehicles, without a single mission failure attributable to filter malfunction [1]. Drawing from this aerospace heritage, the EKF offers a strong foundation for accurately estimating key battery states, such as State of Charge (SoC) and State of Health (SoH) which, like spacecraft navigation systems, are nonlinear, dynamic, and susceptible to sensor uncertainty. Its proven track record in sensor fusion for such conditions supports its use in electric vehicle battery state estimation under real-world driving scenarios.

### Common battery models

In the surrounding work and literature, there are mainly three ways to model the battery [2].

#### a. Equivalent Circuit models such as

- **Rint Model** using series resistance.
- **Thevenin Model** using an equivalent resistance $R_{eq}$ in series with the Thevenin voltage $V_{th}$.
- **Dual Polarization Model** using open-circuit voltage and 2 RC system.

#### b. Electro-chemical models such as

- **Pseudo-2D Model** aka **Doyle-Fuller-Newman** model using electrode kinetics and ion diffusion.
- **Single Particle Model**, basically a reduced version of the P2D

#### c. Thermal models such as

- Lumped thermal models
- Coupled electro-thermal models: ECM and thermal model with mutual feedback

[TODO](pick a method and explain why it was chosen. use matlab to validate and justify.)

## 2. Coulomb Counting (CC)

### Principle

Coulomb Counting is a straightforward method for estimating SoC by integrating the battery current over time. The SoC is updated based on the charge entering or leaving the battery, starting from a known initial state.

### Limitations

- Sensitive to sensor drift and offset errors
- Accumulation of integration errors over time
- Requires accurate initial SoC
- Does not account for battery aging or temperature effects

### Implementation

- Current sensor selection and calibration
- Integration algorithm
- Periodic correction using reference measurements (e.g., open-circuit voltage)

### Validation

- Simulation and bench test results
- Error analysis and correction strategies

## 3. Extended Kalman Filter (EKF) for SoC Estimation

### Overview

The EKF is a robust sensor fusion technique that integrates data from multiple sensors with predictive battery models to estimate SoC and SoH in real time, even under nonlinear and dynamic conditions.

### Notation

### State vector

### Control input vector

### Process model function

### Measurement model function

### Jacobian of process model matrix

### Jacobian of measurement model matrix

### Error covariance matrix

### Process noise covariance matrix

### Measurement noise covariance matrix

### Kalman Gain

### Predict Equation

### Update Equation

### All together

## 4. Tuning and Parametrization

### Q and R Estimation

### SoC/SoH Initialization

### Model Identification Techniques

## 5. Validation

### Simulation Data or Bench Test Results

### Estimation Error and Convergence

### Real-Time Performance Metrics

## 6. Implementation on microcontroller using embedded C

### Microcontroller system architecture

### Individual sensor readings

### CMSIS Library

### Using RTOS for task scheduling

## 7. Future work

### Multiple motors

### Regenerative braking

## References

[1] https://www.nasa.gov/wp-content/uploads/2015/04/techbul_20-03-nav_filter_042920.pdf

[2] https://www.mdpi.com/1996-1073/16/23/7807
