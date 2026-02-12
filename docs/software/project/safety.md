# Safety Requirements

## Protection Mechanisms

-   Undervoltage
-   Overvoltage
-   Overtemperature
-   Over-current sensoring (open wire detection on fusible links)

## Relevant rules

### **EV.7.3.1 — Mandatory monitoring conditions**

The BMS **must monitor tractive battery voltage and temperature** when

-   Tractive System is Active (EV.11.5)
-   Tractive Battery is connected to a charger (EV.8.3)

### **EV.7.3.2 — Galvanic isolation**

-   The BMS **must have galvanic isolation at each module-to-module boundary**
-   Isolation design **must be documented and approved in the ESF**

### **EV.7.3.3 — Cell balancing restriction**

-   **Cell balancing is prohibited** when the Shutdown Circuit is open  
     (EV.7.2, EV.8.4)

### **EV.7.3.4 — Mandatory fault monitoring**

The BMS **must monitor for all of the following**:

-   a) Cell voltage outside permitted range (EV.7.4.2)
-   b) Blown or tripped voltage sense overcurrent protection
-   c) Temperature outside permitted range (EV.7.5.2)
-   d) Missing or interrupted voltage or temperature measurements
-   e) Internal BMS fault

### **EV.7.3.5 — Required BMS response to faults**

If **any** EV.7.3.4 fault is detected, the BMS **must**:

-   a) Open the Shutdown Circuit (EV.7.2.2)
-   b) Turn on:
    -   BMS Indicator Light
    -   Tractive System Status Indicator (EV.5.11.5)

### **EV.7.3.6 — BMS Indicator Light**

The BMS indicator light must:

-   Be **red**
-   Be **clearly visible to the seated driver in bright sunlight**
-   Be labeled **“BMS”**

### **EV.7.4 Tractive Battery Voltage (BMS Voltage Rules)**

### **EV.7.4.1 — Cell voltage measurement**

-   The BMS **must measure every cell voltage**
-   Parallel-connected cells require only **one measurement**

### **EV.7.4.2 — Permitted voltage range**

-   Cell voltage **must remain within datasheet min/max limits**
-   **Measurement accuracy must be considered**  
     (i.e., limits must include margin)

### **EV.7.4.3 — Voltage sense wire protection**

All voltage sense wires **must have overcurrent protection** that:

-   a) Is in the conductor directly connected to the cell tab
-   b) Has a voltage rating ≥ maximum module voltage

### **EV.7.5 Tractive Battery Temperature (BMS Temperature Rules)**

### **EV.7.5.1 — Temperature measurement**

-   BMS **must measure temperatures at critical points** (20% of the tractive battery)

### **EV.7.5.2 — Maximum temperature**

Temperatures (including sensor error) **must stay below the lower of**:

-   Cell datasheet maximum temperature
-   **60 °C**

### **EV.7.5.3 — Sensor location**

-   Cell temperatures **must be measured at the negative terminal**

### **EV.7.5.4 — Physical sensor placement**

Temperature sensor must be in **direct contact with**:

-   Negative terminal, or
-   Negative terminal busbar **within 10 mm** of weld/clamp

### **EV.7.5.5 — Lithium-based cell coverage**

For lithium cells:

-   a) **At least 20% of cells must be monitored**
-   b) Sensors must be **equally distributed**
-   Monitoring **every cell is recommended**

### **EV.7.5.6 — Shared sensors**

-   One sensor may monitor multiple cells **only if all EV.7.5 rules are met**

### **EV.7.5.7 — Temperature sensor isolation**

Temperature sensors **must be electrically isolated**, either:

-   Between sensor and cell, or
-   In the sensing circuit
    Isolation must account for:
-   GLV/TS isolation
-   Common-mode voltage differences

### **EV.8.3 Charging Shutdown Circuit — BMS Role**

### **EV.8.3.1 — BMS as part of charging safety**

Charging Shutdown Circuit **includes**:

-   Charger Shutdown Button
-   **BMS**
-   IMD

### **EV.8.3.2 — BMS contact requirements**

BMS charging shutdown interface must:

-   Be **normally open**
-   Be **electrically independent** from IMD
-   Prevent any failure from feeding power back into the circuit

### **EV.8.4 Charging Operation — BMS Responsibilities**

### **EV.8.4.1 — Charging monitoring**

While charging, the BMS **must**:

-   Monitor the tractive battery
-   Open the Charging Shutdown Circuit upon fault detection

### **EV.8.4.2 — Required result of BMS trip during charging**

If the Charging Shutdown Circuit opens:

-   All charging current **must stop immediately**
-   TS voltage must drop to **Low Voltage within 5 seconds**
-   Charger must turn off
-   Charger must remain disabled until **manual reset**
