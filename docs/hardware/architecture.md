# Hardware Modules

## Datasheets

    https://www.analog.com/media/en/technical-documentation/data-sheets/adbms6830b.pdf​

    https://www.analog.com/media/en/technical-documentation/data-sheets/adbms6821-adbms6822.pdf​

    https://ds.murata.com/simsurfing/ntcthermistor.html?lcid=en-us&tab=ntcthermistor-car​

    https://www.ti.com/lit/ds/symlink/tmp61.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1770217686090&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Ftmp61 |       |

    https://www.hawkeyeinnovationsllc.com/uploads/1/1/5/5/115506993/4th_gen_6.6kw_tc_charger.pdf​

    https://www.ti.com/lit/ds/symlink/amc0381d-q1.pdf​

    https://www.lem.com/sites/default/files/products_datasheets/cab_500-c_spx_public_datasheet_v3.pdf

| Item Number | Component Name        | Quantity | Datasheet                                                                                                      | Notes              |
| ----------- | --------------------- | -------- | -------------------------------------------------------------------------------------------------------------- | ------------------ |
| 1           | isoSPI Transciever    | 1        | [ADBMS 6822/1](https://www.analog.com/media/en/technical-documentation/data-sheets/adbms6821-adbms6822.pdf​)   | Have eval board    |
| 2           | Cell Monitor          | 12       | [ADBMS 6830](https://www.analog.com/media/en/technical-documentation/data-sheets/adbms6830b.pdf)​              | Have eval board    |
| 3           | Thermistor            | 120      | See below                                                                                                      |
| 4           | Pack Voltage Sensor   | 1        | [AMC 0381Q1](https://www.ti.com/lit/ds/symlink/amc0381d-q1.pdf​)                                               | Can request sample |
| 5           | DC Bus Current Sensor | 1        | [CAB 500](https://www.lem.com/sites/default/files/products_datasheets/cab_500-c_spx_public_datasheet_v3.pdf)   |                    |
| 6           | Charger               | 1        | [ELCON TC HK-L](https://www.hawkeyeinnovationsllc.com/uploads/1/1/5/5/115506993/4th_gen_6.6kw_tc_charger.pdf)​ |                    |
| 7           | CAN Transcievers      | ?        | [TJA1462](https://www.nxp.com/docs/en/data-sheet/TJA1462.pdf)                                                  |                    |

### Thermistor Datasheets

| Document Name                 | Link                                                                                                                                                                                                                                                                                                          | Type      |
| ----------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------- |
| Murata Thermistor Simulator   | [Murata NTC Thermistor Simulator](https://ds.murata.com/simsurfing/ntcthermistor.html?lcid=en-us&tab=ntcthermistor-car)                                                                                                                                                                                       | Simulator |
| TI TMP61 Thermistor Datasheet | [TI TMP61 Datasheet](https://www.ti.com/lit/ds/symlink/tmp61.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1770217686090&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Ftmp61) | Datasheet |
