# Commodore 64 PLA Tester

A static PLA tester written in **C++ for the Arduino UNO R3 (ATmega328P, 5 V)**, using **two cascaded SN74HC595N shift registers**. It checks an **N82S100N programmed as a standard C64 PLA**, removed from the computer. It does not program the chip.

Before powering the circuit, follow the [wiring guide](CONEXIONES.md). It covers components, power, resistors, every signal connection, and an optional HCT244 buffer for TTL input compatibility.

## Project structure

```text
.
├── README.md                         # Build, upload, and operation
├── CONEXIONES.md                     # Complete wiring guide (in English)
├── .gitignore                        # Excludes logs and build artifacts
└── firmware/
    └── pla_tester/
        ├── pla_tester.ino            # Arduino sketch
        └── pla_logic.h              # Expected combinational logic
```

The sketch directory and `.ino` file share a name so Arduino IDE and Arduino CLI recognize them. Test logs are stored locally in `resultados/`, which Git ignores.

## Requirements

- The circuit described in the [wiring guide](CONEXIONES.md) and a USB data cable.
- Arduino IDE with **Arduino AVR Boards**, or Arduino CLI with the `arduino:avr` core.
- A serial terminal. On Artix Linux, `picocom` can also record test output.

No additional Arduino libraries or Python installation are required.

## Build and upload

### Arduino IDE

Open `firmware/pla_tester/pla_tester.ino`, select **Arduino UNO** and its serial port, then upload the sketch.

### Arduino CLI

Install Arduino CLI using the [official instructions](https://docs.arduino.cc/arduino-cli/installation/). Run these commands from the repository root, where this README is located:

```bash
arduino-cli core update-index
arduino-cli core install arduino:avr
arduino-cli board list
arduino-cli compile --fqbn arduino:avr:uno firmware/pla_tester
# Replace this with the port shown by board list:
export PLA_PORT=/dev/ttyACM0
arduino-cli upload -p "$PLA_PORT" --fqbn arduino:avr:uno firmware/pla_tester
arduino-cli monitor -p "$PLA_PORT" --config baudrate=115200
```

Some boards appear as `/dev/ttyUSB0` instead. Close other serial terminals before uploading or opening the port.

On Artix, if access is denied, check `stat -c '%G %A' "$PLA_PORT"` and `id`. If the device belongs to group `uucp`, add your user with `sudo usermod -aG uucp "$(id -un)"`, then log out and back in.

## Run a test

Open the serial terminal at **115200 baud** and send `t`. Opening the port may reset the UNO; wait for its ready message before sending the command. An optional newline is harmless.

| Command | Action |
| --- | --- |
| `t` | Start a test, or run another test after completion. |
| `x` | Stop a running test and print a partial summary. |

Ctrl+C closes the terminal; it does not send `x` to the board. The firmware's serial messages remain in Spanish; the field names and statuses below match that output.

The tester sweeps all 65,536 input combinations in ascending and descending order: **131,072 vectors**. It waits 50 µs after updating the inputs, reads all eight outputs, waits another 50 µs, and reads them again. Execution takes longer than the 13 seconds spent on these delays because shifting, sampling, and serial output add overhead. Wait for the final summary.

Up to 100 failing vectors are printed in detail. All failures contribute to the counters, including those beyond the detail limit.

## Record the output

For example, on Artix Linux, close the Arduino serial monitor and use `picocom`:

```bash
sudo pacman -S --needed picocom
mkdir -p resultados
picocom --baud 115200 --logfile "resultados/pla-$(date +%Y%m%d-%H%M%S).log" "$PLA_PORT"
```

Send `t` after the terminal opens. To exit picocom, press Ctrl+A, then Ctrl+X.

The UNO **does not store persistent reports**; the log is saved on your computer. Do not open two serial terminals at once. A reset or disconnection interrupts the test. A log without a final summary is not a passing result.

## Interpret the results

| Status | Meaning |
| --- | --- |
| `PASS_ESTATICO` | Both readings matched the model for every vector. |
| `FAIL` | At least one mismatch occurred. Check wiring, power, contacts, and logic thresholds before blaming the PLA. |
| `INTERRUMPIDO` | The test was stopped with `x`; the results are partial. |

The summary reports `Vectores` (vectors tested), `fallidos` (failing vectors), `inestables` (vectors whose two readings differed), and `tiempo_ms` (elapsed milliseconds). A completed test must report 131,072 vectors.

Failure details use hexadecimal:

| Field | Meaning |
| --- | --- |
| `pasada` | Sweep number, 1 or 2. |
| `entrada` | Applied 16-bit input vector. |
| `esperado` | Expected eight output levels. |
| `lectura1`, `lectura2` | First and second output readings. |
| `xor` | Bits that differed from the expected result in either reading. |

For example, `xor=01` identifies output bit 0, `/CASRAM`; `xor=20` identifies bit 5, `/IO`. Per-output counters report errors and low/high levels; the low/high counts use the second reading.

This is a static logic test with the PLA's `/OE` tied to GND. It does not measure propagation delays, glitches, high impedance, temperature effects, or actual C64 bus loading. A pass does not guarantee operation inside a computer. Direct PLA-to-UNO readings also have the threshold limitation explained in the [wiring guide](CONEXIONES.md).

## Validation status

The firmware has been compiled and linked for an ATmega328P at 16 MHz using Arduino AVR core 1.8.8. The C++ model was previously compared with all 65,536 combinations of the published JEDEC data; that verifier is not included in this repository. These software checks do not validate the physical circuit.

## Repository contents

This directory is the repository root. Version-controlled files are `README.md`, `CONEXIONES.md`, `.gitignore`, and `firmware/`. Test logs and build artifacts are ignored. Files from the parent directory are not required to build or run the tester.

Technical sources are listed in the [wiring guide](CONEXIONES.md#references).
