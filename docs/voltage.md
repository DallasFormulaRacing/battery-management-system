# A developer note on the Analog Devices BMS 6830 and BMS 6833

## 1. Introduction

The voltage monitor stack we will use involves the ADBMS6830B and ADBMS6822. The ADBMS6830B is here on out referred to as the "6830", and the ADBMS6822 is here on out referred to as the "6822".

The 6830 is the IC that's doing the actual voltage monitoring by sending and recieving SPI commands. The 6822 is what translates microcontroller SPI messages into 6830-compantible isoSPI packets (further clarified later).

## 2. Architecture

Because of the nature of high voltage applications and how induced electromagnetic interference can be a common source for error, Analog Devices has provided an isoSPI transciever that translates normal SPI into isoSPI protocol. isoSPI protocol is an adaptation of SPI that highly resembles the physical signal archtecture of CAN using a differential pair.

Transformers are what enable the isoSPI communication protocol as this provides galvanic isolation (no current flow between the two) between the batteries and the host microcontroller. Galvanic isolation is necessary when two or more electric circuits must communicate, but their grounds may be at different potentials, as in our case, a 3v3 ground GLV system and the 600v (or some large number) HV accumulator.

Since SPI supports daisy chaining, it follows that isoSPI also supports daisy chaining. Daisy chaining is how we are able to send only one command to request multiple (in our case, ten) battery monitor ICs.

Each segment board will contain two BMS6830s. The first 6830 on the first segment board will receive an isoSPI signal directly from the 6822 tranciever. The rest of the 6830s will recieve the isoSPI signal from their chain predecessors.

Daisy chaining offers advantages as well as disadvantages. Mainly: as all sequence-dependent things go, if one IC is down, the others will soon follow. However, this decreases the physical wiring complexity and volume significantly. The ADBMS68XX series were designed with this purpose.

## 3. Communicating

The microcontroller is the SPI master, and is the recipient of all MOSI packets and the source of all MISO commands. The microcontroller emits the clock signal that all BMS6830 (slaves) must adhere to.

Data integrity is maintained by a cyclic redundancy function checking for packet errors. If the CRC packet recieved from a slave does not match the calculated CRC packet at the host MCU, the entire information packet is invalidated and discarded. The frequency at which this occurs is significantly attenuated by the engineering of the isoSPI tranceiver architecture. However, it would be wise to also design the hardware to minimize EMI-induced error, much like the software.

One might ask: What does the Chip Select (CS') line do if this is a daisy chained archtecture using isoSPI? The normal SPI daisy chain simply connects all slaves to the same CS' line and pulls the single line connected to every slave low. The same goes for the synchronous clock (SCLK) line. How does this change with the usage of isoSPI?

The answer is that the CS line coming from the microcontroller stops at the 6822 transciever. From there, only the 6822 is activated by the CS line being pulled low. The clock signal that follows will be encoded within the digital differential signal, done by the 6822 IC, and is then forwarded to the daisy chained links of the 6830 ICs.

## 4. Commands

Communication with the 6830 is done by sending structured SPI commands that follow Analog Devices' isoSPI protocol. All SPI packets maintain the following frame structure:

```
<command> + <data> + <checksum>
```

There are specific types of commands:

Broadcast commands are

### Example: A Simple Transaction

## Need to know

## 5. Cell Balancing

PWM on the ADBMS6830 is used primarily for passive cell balancing. It allows controlled switching of external balancing resistors to discharge individual cells and equalize voltages across the battery pack. Use PWM when implementing balancing logic to maintain cell voltage uniformity during charging or maintenance cycles.
