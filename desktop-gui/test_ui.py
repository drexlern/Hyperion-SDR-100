import numpy as np
from imgui_bundle import imgui, imgui_knobs, immapp, implot

# Simulated HF band state
vfo_mhz = 14.074
freqs = np.linspace(14.000, 14.150, 512, dtype=np.float32)


def gui():
    global vfo_mhz

    imgui.text("HF SDR Transceiver - Environment Check")
    imgui.separator()

    # Interactive VFO Tuning Knob
    changed, vfo_mhz = imgui_knobs.knob(
        "VFO (MHz)", vfo_mhz, 1.800, 30.000, speed=0.005, format="%.3f"
    )

    imgui.same_line()
    if imgui.button("Tune 7x7 ATU", size=imgui.ImVec2(140, 40)):
        print(f"ATU Tune requested at {vfo_mhz:.3f} MHz")

    # Simulated live HF noise floor + signal peak
    noise = -115.0 + np.random.normal(0, 2.5, size=512).astype(np.float32)
    signal = 45.0 * np.exp(-((freqs - vfo_mhz) ** 2) / (2 * 0.002**2)).astype(
        np.float32
    )
    spectrum = noise + signal

    # Real-time ImPlot Spectrum Display
    if implot.begin_plot("HF Panadapter (dBm)", size=imgui.ImVec2(-1, 300)):
        implot.setup_axes("Frequency (MHz)", "Signal (dBm)")
        implot.setup_axis_limits(implot.ImAxis_.x1, 14.000, 14.150, imgui.Cond_.always)
        implot.setup_axis_limits(implot.ImAxis_.y1, -130.0, -40.0, imgui.Cond_.always)
        implot.plot_line("RX Spectrum", freqs, spectrum)
        implot.end_plot()


if __name__ == "__main__":
    immapp.run(gui, with_implot=True, window_title="SDR Shack Test", window_size=(900, 520))