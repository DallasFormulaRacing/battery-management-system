// --- PREAMBLE ---
#set page(
  paper: "us-letter",
  margin: (x: 0.65in, y: 0.75in),
  header: context {
    set text(9pt, style: "italic")
    grid(
      columns: (1fr, 1fr),
      align(left)[Formula SAE Electric | 2026], align(right)[BMS Final Report | Page #counter(page).display()],
    )
    v(-0.5em)
    line(length: 100%, stroke: 0.5pt)
  },
)

#set text(font: "New Computer Modern", size: 10pt) // IEEE standard is 10pt
#set par(justify: true, first-line-indent: 1em)

// IEEE Heading Styles
#set heading(numbering: (..n) => {
  let pos = n.pos()
  if pos.len() == 1 { numbering("I.", ..pos) } else { numbering("1.1", ..pos) }
})

#show heading: set block(above: 1.4em, below: 1em)
#show heading.where(level: 1): it => block(width: 100%, align(center, text(
  11pt,
  weight: "bold",
  features: ("smcp",),
  it,
)))

// Functions
#let section(it) = heading(level: 1)[#it]
#let subsection(it) = heading(level: 2)[#it]
#let subsubsection(it) = heading(level: 3)[#it]

// --- TITLE SECTION (Single Column) ---
#let title = "Design and Validation of a Tractive Battery Management System for a Formula-Style Electric Vehicle"
#let authors = ("William Kim", "Aiden Fenstermacher", "et al.")

#align(center)[
  #block(width: 90%)[
    #text(1.8em, weight: "bold")[#title] \
    #v(0.5em)
    #text(1.2em)[Dallas Formula Racing Electric] \
    #text(1.1em, style: "italic")[The University of Texas at Dallas] \
    #v(0.5em)
    #authors.join(", ")
  ]
]

#v(2em)

// Abstract & Keywords (Full Width)
#grid(
  columns: 1fr,
  gutter: 1em,
  [
    #set text(9pt)
    #strong[Abstract —] #lorem(50)

    #v(0.5em)
    #strong[Keywords —] BMS, FSAE, Tractive Battery, STM32, HV Safety, Li-ion.
  ]
)

#v(2em)

// --- BODY SECTION (Double Column) ---
#show: columns.with(2, gutter: 0.25in)

#section[Introduction]
#lorem(25)
#subsection[Justifying Change of Architecture]
#lorem(25)
#subsubsection[Distributed vs. Centralized]
#lorem(25)
#subsubsection[Comparison: Orion BMS vs. Custom BMS]
#lorem(25)
#subsubsection[Hardware Limitations: Orion Cell Tap Insufficiency]
#lorem(25)
#subsubsection[New Tractive Battery Topology]
#lorem(25)
#subsubsection[Improved Cell Voltage Accuracy]
#lorem(25)
#subsubsection[Telemetry: CAN Bus Integration for Driver Display]
#lorem(25)
#subsubsection[Charging Integration: J1772 Standards]
#lorem(25)

#section[Requirements]
#lorem(25)
#subsection[Hardware Requirements]
#lorem(25)
#subsection[Software Requirements]
#lorem(25)
#subsection[Isolation and High-Voltage Safety Requirements]
#lorem(25)

#section[System Overview]
#lorem(25)
#subsection[Selection of Hardware Components]
#lorem(25)
#subsubsection[ASIC Comparison: TI vs. AD (16-Cell vs. 18-Cell)]
#lorem(25)
#subsection[Selection of Microcontroller and Peripherals]
#lorem(25)
#subsubsection[Platform Selection: Why STM32 Ecosystem]
#lorem(25)
#subsubsection[Specific Part Selection: STM32G474 Features]
#lorem(25)
#subsection[Alternative Design Options Considered]
#lorem(25)

#section[Hardware Design]
#lorem(25)
#subsection[Powering the Cell Monitor Chip]
#lorem(25)
#subsection[Safety Circuitry: Open Wire Detection]
#lorem(25)
#subsection[Signal Integrity: RC Filter Design]
#lorem(25)
#subsection[Modular Design: Stackable Architecture Philosophy]
#lorem(25)

#section[Software Architecture]
#lorem(25)
#subsection[Minimum Viable Product (MVP) and Safety Features]
#lorem(25)
#subsubsection[Safety Rules: OV, UV, Open Wire, and Thermal Monitoring]
#lorem(25)
#subsection[Algorithms and Control]
#lorem(25)
#subsubsection[Optimized Temperature Conversion Algorithm]
#lorem(25)
#subsubsection[State of Charge (SoC) Estimation & Drift Compensation]
#lorem(25)
#subsection[Charging and Cell Balancing]
#lorem(25)
#subsubsection[Measurement Handling during Balancing Cycles]
#lorem(25)
#subsubsection[Thermal Management: Maximum Discharge Current & PWM Control]
#lorem(25)
#subsubsection[Charger Interface Protocols]
#lorem(25)
#subsection[Firmware Implementation]
#lorem(25)
#subsubsection[RTOS Integration and Task Scheduling]
#lorem(25)
#subsubsection[DMA Utilization for SPI/isoSPI Communication]
#lorem(25)
#subsubsection[Error Detection and Fault Handling]
#lorem(25)
#subsection[User Interface and Data Visualization]
#lorem(25)
#subsubsection[Raspberry Pi Integration for Real-Time Monitoring]
#lorem(25)

#section[Integration, Testing, and Validation]
#lorem(25)
#subsection[Revision History: V1 (August) vs. V2 (March/April)]
#lorem(25)
#subsection[Validation Tools: Cell Simulator Board Design]
#lorem(25)
#subsection[Debugging Methodologies]
#lorem(25)

#section[Safety, Hazards, and Failure Analysis]
#lorem(25)

#section[Results, Discussion, and Future Work]
#lorem(25)

#section[Conclusion]
#lorem(25)

#section[Acknowledgements]
#subsection[Sponsorships]
We thank Tesla, Inc., Analog Devices, Inc., and Altium for their contributions to Dallas Formula Racing.

#subsection[Team]
#lorem(25) // Placeholder for team list

#section[Appendix]
