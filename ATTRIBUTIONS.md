# Third-Party Attributions & Dependency Ledger

Hyperion SDR-100 builds upon classic amateur radio engineering topologies and open-source software libraries. All PCB schematics, layouts, and application code in this repository are original works created for this project.

---

## 1. Hardware Circuit Topologies & EDA Libraries (`/hardware-atu`)

| Subsystem / Component | Reference / Source | License / IP Status | Notes & Compliance |
| :--- | :--- | :--- | :--- |
| **7×7 L-Network ATU Topology** | Classic Binary Switched L-Network / N7DDC ATU-100 Topology | Public Domain / Open Hardware | Original schematic and PCB layout drawn from scratch in KiCad. |
| **Tandem Match Directional Coupler** | Classic Stockton / Tandem Match SWR Bridge | Public Domain | 10:1 turns ratio on Fair-Rite BN43-202 binocular ferrite cores. |
| **KiCad Schematic Symbols & Footprints** | KiCad EDA Built-in Libraries | CC-BY-SA 4.0 (with KiCad Library Exception) | KiCad's library exception permits unrestricted manufacturing, distribution, and licensing of derivative PCBs. |
| **XIAO ESP32-C3 Footprint** | Seeed Studio OPL KiCad Library (`XIAO-ESP32-C3-DIP`) | MIT License | Used for physical footprint dimensions of the Seeed Studio XIAO ESP32-C3 module. |
| **Discrete ICs & Power Relays** | Texas Instruments (`TPIC6B595N`, `LM7805`), Espressif (`ESP32-C3`), Omron (`G2RL-1`, `G2RL-2`) | Standard Commercial Datasheets | Standard datasheet application circuits implemented in custom designs. |

---

## 2. Desktop GUI Software Dependencies (`/desktop-gui`)

| Library / Tool | Role in Project | License | Obligations & Notes |
| :--- | :--- | :--- | :--- |
| **Dear ImGui Bundle (`imgui-bundle`)** | 60 FPS OpenGL GUI, knobs, and ImPlot spectrum/waterfall rendering | MIT License | Wraps C++ Dear ImGui (Omar Cornut) and ImPlot (Evan Pezent). Retain copyright and permission notices in distributions. |
| **NumPy (`numpy`)** | Array math, FFT magnitude processing, and binary buffer handling | BSD 3-Clause License | Retain BSD copyright notice and disclaimer in distributions. |
| **pySerial (`pyserial`)** | USB Serial communication with the ESP32-C3 ATU controller | BSD 3-Clause License | Retain BSD copyright notice and disclaimer in distributions. |
| **Python 3.12 / 3.13** | Core runtime language | PSF License | Retain PSF copyright notice if bundling the Python runtime. |
