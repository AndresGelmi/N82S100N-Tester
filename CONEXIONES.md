# PLA Tester Wiring Guide

This guide covers the **5 V Arduino UNO R3**, two **SN74HC595N DIP-16** shift registers, and an **N82S100N DIP-28 programmed for the C64**. The tables match the [firmware](firmware/pla_tester/pla_tester.ino). See the [README](README.md) for building, uploading, and running tests.

## Components and power

- Arduino UNO R3, USB data cable, and two SN74HC595N DIP-16 chips (U1 and U2).
- PLA, wide DIP-28 socket, breadboard, and short wires.
- Three 100 nF capacitors, one next to each external chip, plus one 10 µF capacitor across the 5 V rail and GND. Observe polarity if using a polarized capacitor.
- One 10 kΩ resistor for the shift registers' `/OE` line, and sixteen 10 kΩ resistors for the PLA inputs.

Disconnect power before assembling or changing the circuit. The PLA must be **removed from the C64**. Power the complete circuit through the UNO's USB connection: connect **5V** to the 5 V rail and **GND** to common ground. Do not use VIN, 3.3 V, or a second supply at the same time. Check for 4.75–5.25 V under load and verify chip orientation before testing.

DIP pin numbering is shown from above: with the notch at the top, pin 1 is at the upper left and numbers run counterclockwise.

## Direct-reading threshold limitation

Using a common 5 V supply avoids a voltage-level overvoltage, but does not guarantee compatible logic thresholds. The 82S100 guarantees a TTL high output from 2.4 V, while these ATmega328P GPIO inputs require 0.6 × VCC (3 V at a 5 V supply). Direct wiring may work with a particular chip, but can produce false failures.

For guaranteed logic-level compatibility, add a **74HCT244 powered at 5 V** between the eight PLA outputs and the UNO. Use the **HCT** family, which has TTL input thresholds. The two 595 chips still drive the PLA inputs. No firmware changes are required.

| HCT244 pin | Connection |
| --- | --- |
| 20, VCC | 5 V |
| 10, GND | Common ground |
| 1 and 19, output enables | GND |
| 20 to 10 | Additional 100 nF decoupling capacitor |

Insert each buffer channel as follows, replacing the corresponding direct PLA-to-UNO wire:

| Output bit | PLA pin → HCT244 input pin | HCT244 output pin → UNO |
| --- | --- | --- |
| 0 | 18 → 2 | 18 → D2 |
| 1 | 17 → 4 | 16 → D3 |
| 2 | 16 → 6 | 14 → D4 |
| 3 | 15 → 8 | 12 → D5 |
| 4 | 13 → 11 | 9 → D6 |
| 5 | 12 → 13 | 7 → D7 |
| 6 | 11 → 15 | 5 → D8 |
| 7 | 10 → 17 | 3 → D9 |

## Control and power connections

U1 is the register connected to the UNO's serial data output. U2 receives serial data from U1.

| Source | Destination |
| --- | --- |
| UNO D11 | U1 SER, pin 14 |
| U1 QH′, pin 9 | U2 SER, pin 14 |
| UNO D13 | SRCLK, pin 11 on both 595 chips |
| UNO D10 | RCLK/latch, pin 12 on both 595 chips |
| UNO D12 | `/OE`, pin 13 on both 595 chips; one 10 kΩ resistor from this shared line to 5 V |
| 5 V | VCC pin 16 and `/SRCLR` pin 10 on both 595 chips |
| GND | Pin 8 on both 595 chips |
| U2 QH′, pin 9 | Leave unconnected |
| PLA pin 28 | 5 V |
| PLA pin 14 | GND |
| PLA pin 1, FE | GND |
| PLA pin 19, `/OE` | GND |

The `/OE` pull-up disables the 595 outputs during reset. Add a **10 kΩ resistor to GND at each of the sixteen PLA inputs** to keep those inputs defined during that interval. The firmware loads zero before enabling the registers. `/SRCLR` clears the shift register; it does not initialize the output latch by itself.

Place a 100 nF capacitor between VCC and GND close to each chip: pins 16–8 on each 595 and pins 28–14 on the PLA.

## PLA inputs

QA is the least significant bit of each byte. The firmware shifts out the high byte first, then the low byte, both MSB first, and finally updates the output latch. U1 holds bits 0–7 and U2 holds bits 8–15. Outputs remain stable while data is being shifted.

| Bit | Signal | PLA pin | Register | 595 output (pin) |
| --- | --- | --- | --- | --- |
| 0 | A13 | 2 | U1 | QA (15) |
| 1 | A14 | 3 | U1 | QB (1) |
| 2 | A15 | 4 | U1 | QC (2) |
| 3 | /VA14 | 5 | U1 | QD (3) |
| 4 | CHAREN | 6 | U1 | QE (4) |
| 5 | HIRAM | 7 | U1 | QF (5) |
| 6 | LORAM | 8 | U1 | QG (6) |
| 7 | /CAS | 9 | U1 | QH (7) |
| 8 | VA12 | 20 | U2 | QA (15) |
| 9 | VA13 | 21 | U2 | QB (1) |
| 10 | /GAME | 22 | U2 | QC (2) |
| 11 | /EXROM | 23 | U2 | QD (3) |
| 12 | R/W | 24 | U2 | QE (4) |
| 13 | /AEC | 25 | U2 | QF (5) |
| 14 | BA | 26 | U2 | QG (6) |
| 15 | A12 | 27 | U2 | QH (7) |

## PLA outputs

Use this table for direct wiring, or insert the HCT244 as described above. The firmware uses `INPUT` without internal pull-ups. D0 and D1 are reserved for serial communication.

| Bit | Signal | PLA pin | UNO |
| --- | --- | --- | --- |
| 0 | /CASRAM | 18 | D2 |
| 1 | /BASIC | 17 | D3 |
| 2 | /KERNAL | 16 | D4 |
| 3 | /CHAROM | 15 | D5 |
| 4 | /GRW | 13 | D6 |
| 5 | /IO | 12 | D7 |
| 6 | /ROML | 11 | D8 |
| 7 | /ROMH | 10 | D9 |

Vectors represent electrical pin levels. A `/` prefix means active low; the software does not automatically invert input signals.

## Before connecting USB

1. Verify the orientation and pin numbering of all external chips.
2. Check for shorts between 5 V and GND, and confirm that all grounds are connected.
3. Verify U1 pin 9 → U2 pin 14. Do not confuse serial output QH′ (pin 9) with parallel output QH (pin 7).
4. Check the sixteen input resistors to GND and the shared 595 `/OE` pull-up to 5 V.
5. Verify all eight PLA output paths to D2–D9, which the firmware configures as inputs.
6. Connect USB and check the supply voltage before sending `t`.

## References

- [Arduino UNO R3](https://docs.arduino.cc/hardware/uno-rev3/) and [ATmega328P datasheet](https://docs.arduino.cc/resources/datasheets/ATmega328P-datasheet.pdf).
- [Texas Instruments SN74HC595 datasheet](https://www.ti.com/lit/ds/symlink/sn74hc595.pdf): pinout, cascading, latch, and output enable.
- [Signetics 82S100 datasheet](https://s3.amazonaws.com/com.c64os.resources/weblog/versa64cart/82S100-datasheet.pdf): pinout and electrical levels.
- [C64 PLA equations and JEDEC data](https://www.zimmers.net/anonftp/pub/cbm/documents/chipdata/c64-pla.txt).
- [SN74HCT244 datasheet](https://www.ti.com/lit/ds/symlink/sn74hct244.pdf): optional buffer for TTL threshold compatibility.
