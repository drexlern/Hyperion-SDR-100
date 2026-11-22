# Third-Party Attributions & Dependency Ledger

Hyperion SDR-100 builds upon classic amateur radio engineering topologies and open-source software libraries[cite: 3, 4]. All PCB schematics, layouts, and application code in this repository are original works created for this project[cite: 3, 4].

---

## 1. Hardware Circuit Topologies & EDA Libraries (`/hardware-atu`)

| Subsystem / Component | Reference / Source | License / IP Status | Notes & Compliance |
| :--- | :--- | :--- | :--- |
| **7×7 L-Network ATU Topology** | Classic Binary Switched L-Network / N7DDC ATU-100 Topology[cite: 3] | Public Domain / Open Hardware[cite: 3] | Original schematic and PCB layout drawn from scratch in KiCad[cite: 3]. |
| **Tandem Match Directional Coupler** | Classic Stockton / Tandem Match SWR Bridge[cite: 3] | Public Domain[cite: 3] | 10:1 turns ratio on Fair-Rite BN43-202 binocular ferrite cores[cite: 3]. |
| **KiCad Schematic Symbols & Footprints** | KiCad EDA Built-in Libraries[cite: 3] | CC-BY-SA 4.0 (with KiCad Library Exception)[cite: 3] | KiCad's library exception permits unrestricted manufacturing, distribution, and licensing of derivative PCBs[cite: 3]. |
| **XIAO ESP32-C3 Footprint** | Seeed Studio OPL KiCad Library (`XIAO-ESP32-C3-DIP`)[cite: 3] | MIT License[cite: 3] | Used for physical footprint dimensions of the Seeed Studio XIAO ESP32-C3 module[cite: 3]. |
| **Discrete ICs & Power Relays** | Texas Instruments (`TPIC6B595N`, `LM7805`), Espressif (`ESP32-C3`), Omron (`G2RL-1`, `G2RL-2`)[cite: 3] | Standard Commercial Datasheets[cite: 3] | Standard datasheet application circuits implemented in custom designs[cite: 3]. |

---

## 2. Desktop GUI Software Dependencies (`/desktop-gui`)

| Library / Tool | Role in Project | License | Obligations & Notes |
| :--- | :--- | :--- | :--- |
| **Dear ImGui Bundle (`imgui-bundle`)** | 60 FPS OpenGL GUI, knobs, and ImPlot spectrum/waterfall rendering[cite: 3] | MIT License[cite: 3] | Wraps C++ Dear ImGui (Omar Cornut) and ImPlot (Evan Pezent)[cite: 3]. Retain copyright and permission notices in distributions[cite: 3]. |
| **NumPy (`numpy`)** | Array math, FFT magnitude processing, and binary buffer handling[cite: 3] | BSD 3-Clause License[cite: 3] | Retain BSD copyright notice and disclaimer in distributions[cite: 3]. |
| **pySerial (`pyserial`)** | USB Serial communication with the ESP32-C3 ATU controller[cite: 3] | BSD 3-Clause License[cite: 3] | Retain BSD copyright notice and disclaimer in distributions[cite: 3]. |
| **Python 3.12 / 3.13** | Core runtime language[cite: 3] | PSF License[cite: 3] | Retain PSF copyright notice if bundling the Python runtime[cite: 3]. |