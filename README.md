# Hyperion SDR-100

An open-source, homebrew **100W all-mode, all-band (160m–10m) HF Software-Defined Radio (SDR) transceiver** built around a **Xilinx Zynq SoC** architecture, an autonomous **7×7 L-network automatic antenna tuner (ATU)**, and a **60 FPS cross-platform desktop control head**.

---

## System Architecture

Hyperion SDR-100 is designed modularly so each RF, embedded, and software subsystem can be built, tested, and verified independently on the bench:

* **`/hardware-atu` — 100W 7×7 Automatic Antenna Tuner (KiCad):**
  * **Matching Network:** Binary-switched 7-inductor ($0.05\,\mu\text{H}$ – $4.5\,\mu\text{H}$ on Micrometals Type-2 powdered iron toroids) and 7-capacitor ($10\,\text{pF}$ – $1000\,\text{pF}$, $1\,\text{kV}$ C0G/NP0) L-network.
  * **Switching Matrix:** 16 Omron G2RL power relays ($14\times$ G2RL-1 SPDT for L/C banks, $1\times$ G2RL-2 DPDT for L/C topology reversal, $1\times$ G2RL-2 DPDT for full tuner bypass) driven via two daisy-chained **TI TPIC6B595N** open-drain shift registers.
  * **SWR & Power Sensing:** Built-in **Tandem Match (Stockton) directional coupler** wound on Fair-Rite BN43-202 binocular cores with 1N5711 Schottky peak detectors and $3.3\,\text{V}$ Zener ADC overvoltage protection[cite: 3, 4].
  * **Controller:** **Seeed Studio XIAO ESP32-C3** mounted via through-hole headers for autonomous SWR tuning and USB/UART telemetry.
* **`/desktop-gui` — Shack Control Head (Python / Dear ImGui):**
  * 60 FPS OpenGL panadapter and 2D waterfall display powered by `imgui-bundle` (Dear ImGui + ImPlot) and `numpy`[cite: 3, 5].
  * Live VFO tuning, band switching, real-time 16-relay ATU state visualizer, forward/reflected power & SWR metering, and a 180-second TX watchdog timer.
  * Designed for local Windows/Linux shack operation and low-latency remote streaming via **Parsec**[cite: 3, 5].
* **`/firmware-esp32c3` — ATU Firmware (C++ / PlatformIO):** *(In Development)*
  * High-speed SPI shift-register control, ADC forward/reflected power sampling, and automated L/C impedance matching algorithms.
* **`/zynq-soc` — SDR Baseband & DSP (HDL / ARM C++):** *(Planned)*
  * Xilinx Zynq-7000 FPGA fabric for Digital Down/Up Conversion (DDC/DUC) and dual-core ARM Cortex-A9 processing system for rig control and Ethernet IQ/FFT streaming.

---

## Repository Structure

```text
Hyperion-SDR-100/
├── hardware-atu/        # KiCad schematics, PCB layouts, and custom footprints
├── desktop-gui/         # Python/ImGui transceiver control head and ATU dashboard
├── firmware-esp32c3/    # XIAO ESP32-C3 ATU controller firmware
├── zynq-soc/            # Xilinx Zynq SoC HDL and embedded C/C++ software
├── README.md            # Project overview and architecture
├── ATTRIBUTIONS.md      # Third-party hardware references, libraries, and licenses
└── LICENSE.md           # Dual open-source hardware and software license terms
