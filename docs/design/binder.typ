//bullet points

= overview
== jusitfying change of architecture
=== distributed vs centralized
=== comparisions of the orion bms vs ours
=== insufficient number of cell taps on orion
=== new tractive battery topology
=== improved cell voltage accuracy (1.8mV on ADBMS vs 12.5mV on Orion)
=== communicate BMS data via CAN to driver display
=== J1772 Charging integration

== hardware requirements

== software requirements

== isolation requirements


== selection of hardware components
=== TI vs AD vs 16 cell vs 18 cell
== selection of microcontroller and relevant internal peripherals
=== why stm32
=== why G474
== options considered


= hardware design
== powering the cell monitor chip
== open wire circuit
== RC filter
== stackable philosophy

= software design
== minimum viable product and safety features
==== rules (ov,uv,ow,temp,temp ow)

== fast temperature conversion algorithm


== charging and cell balancing
=== handling cell measurements during cell balancing and charging
==== maximum cell discharge current
==== pwm control
==== interfacing with the charger

== user interface (gui) and visualization of data
==== raspberry pi

== state of charge estimation
==== methods used
==== flux gate current sensor
==== high voltage bus sensor
==== handling current drift

== general firmware characteristics
==== integration of RTOS
==== use of Direct Memory Access
==== data error detection

== consideration of communication protocols with isoSPI
==== SPI and communicating via DMA controller
==== interrupt



= integration, testing, and validation
== v1 of boards in august
== v2 of boards in march/april
== cell simulator board
== use of software debugger

= shortcomings, successes and failures



