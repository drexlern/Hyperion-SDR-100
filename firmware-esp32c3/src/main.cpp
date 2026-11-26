#include <Arduino.h>
#include <SPI.h>
#include <math.h>

// ============================================================================
// 1. Verified Hardware Pin Definitions (Seeed Studio XIAO ESP32-C3)
// ============================================================================
constexpr uint8_t PIN_V_FWD  = D0;   // GPIO2 (ADC1_CH2) - Physical Pin 1
constexpr uint8_t PIN_V_REV  = D1;   // GPIO3 (ADC1_CH3) - Physical Pin 2
constexpr uint8_t PIN_RCK    = D3;   // GPIO5            - Physical Pin 4 -> U4/U5 Pin 12
constexpr uint8_t PIN_SRCK   = D8;   // GPIO8 (SPI SCK)  - Physical Pin 9 -> U4/U5 Pin 13
constexpr uint8_t PIN_SER_IN = D10;  // GPIO10 (MOSI)    - Physical Pin 11 -> U4 Pin 3

// ============================================================================
// 2. RF Coupler & Calibration Constants
// ============================================================================
constexpr float COUPLER_RATIO      = 10.0f;  // 10:1 BN-43-202 Tandem Match
constexpr float ADC_DIVIDER_RATIO  = 2.0f;   // 32.4k / 32.4k 2:1 divider (R3-R6)
constexpr float DIODE_DROP_V       = 0.25f;  // 1N5711 Schottky forward drop
constexpr float ADC_CAL_SCALE      = 1.00f;  // Trim against dummy load + wattmeter
constexpr float MIN_TUNE_POWER_W   = 3.0f;   // Minimum carrier power to sense SWR
constexpr float MAX_TUNE_POWER_W   = 25.0f;  // Protect G2RL contacts during switching
constexpr float TARGET_SWR         = 1.35f;  // Stop auto-tune once SWR <= 1.35:1
constexpr uint16_t RELAY_SETTLE_MS = 8;      // Omron G2RL operate/release (~5-7ms)

// ============================================================================
// 3. ATU State (Matches desktop-gui RadioState bitmasks)
//    U4: Bit 0..6 = L1..L7 (0.05uH..4.5uH), Bit 7 = K16 (0: C on ANT, 1: C on TX)
//    U5: Bit 0..6 = C1..C7 (10pF..1000pF),  Bit 7 = K17 (0: Bypass,   1: Active)
// ============================================================================
struct AtuState {
    uint8_t u4_l_mask = 0x00;
    uint8_t u5_c_mask = 0x00;
    float fwd_watts   = 0.0f;
    float ref_watts   = 0.0f;
    float swr         = 1.0f;
    bool tuning       = false;
};

AtuState atu;
uint32_t last_telemetry_ms = 0;

// ============================================================================
// 4. Shift Register Driver (U4 -> U5 Daisy-Chained TPIC6B595N)
// ============================================================================
void applyRelays(uint8_t u4_mask, uint8_t u5_mask) {
    atu.u4_l_mask = u4_mask;
    atu.u5_c_mask = u5_mask;

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(PIN_RCK, LOW);
    // U5 is downstream of U4 (U4 SER_OUT -> U5 SER_IN), so clock U5 out first
    SPI.transfer(u5_mask);
    SPI.transfer(u4_mask);
    digitalWrite(PIN_RCK, HIGH);
    SPI.endTransaction();

    delay(RELAY_SETTLE_MS);
}

// ============================================================================
// 5. Tandem Match SWR & Power Measurement
// ============================================================================
void measureSwr() {
    constexpr int SAMPLES = 32;
    uint32_t sum_fwd_mv = 0;
    uint32_t sum_ref_mv = 0;

    for (int i = 0; i < SAMPLES; i++) {
        sum_fwd_mv += analogReadMilliVolts(PIN_V_FWD);
        sum_ref_mv += analogReadMilliVolts(PIN_V_REV);
    }

    float v_fwd_adc = (sum_fwd_mv / (float)SAMPLES) / 1000.0f;
    float v_ref_adc = (sum_ref_mv / (float)SAMPLES) / 1000.0f;

    float v_fwd_peak = (v_fwd_adc > 0.015f)
        ? ((v_fwd_adc * ADC_DIVIDER_RATIO) + DIODE_DROP_V) * COUPLER_RATIO * ADC_CAL_SCALE
        : 0.0f;

    float v_ref_peak = (v_ref_adc > 0.015f)
        ? ((v_ref_adc * ADC_DIVIDER_RATIO) + DIODE_DROP_V) * COUPLER_RATIO * ADC_CAL_SCALE
        : 0.0f;

    atu.fwd_watts = (v_fwd_peak * v_fwd_peak) / 100.0f;
    atu.ref_watts = (v_ref_peak * v_ref_peak) / 100.0f;

    if (atu.fwd_watts < 0.2f) {
        atu.swr = 1.0f;
    } else {
        float rho = sqrtf(fminf(atu.ref_watts / atu.fwd_watts, 0.99f));
        atu.swr = (1.0f + rho) / fmaxf(1.0f - rho, 0.01f);
    }
}

// ============================================================================
// 6. Two-Pass Coarse + Fine Binary L/C Auto-Tune Algorithm
// ============================================================================
void runAutoTune() {
    measureSwr();
    if (atu.fwd_watts < MIN_TUNE_POWER_W) {
        Serial.println("ERR:LOW_PWR");
        return;
    }
    if (atu.fwd_watts > MAX_TUNE_POWER_W) {
        Serial.println("ERR:HIGH_PWR_PROTECT");
        return;
    }

    atu.tuning = true;
    float best_swr = 99.0f;
    uint8_t best_u4 = 0x00;
    uint8_t best_u5 = 0x80; // Bit 7 = 1 engages ATU (K17 active)

    // Pass 1: Coarse sweep across both topologies (K16: 0 = C on ANT, 1 = C on TX)
    for (uint8_t topo = 0; topo <= 1; topo++) {
        uint8_t topo_bit = (topo << 7);

        for (uint8_t l_val = 0; l_val < 128; l_val += 4) {
            for (uint8_t c_val = 0; c_val < 128; c_val += 4) {
                applyRelays(topo_bit | l_val, 0x80 | c_val);
                measureSwr();

                if (atu.fwd_watts < MIN_TUNE_POWER_W) {
                    atu.tuning = false;
                    applyRelays(best_u4, best_u5);
                    Serial.println("ERR:TX_DROPPED");
                    return;
                }

                if (atu.swr < best_swr) {
                    best_swr = atu.swr;
                    best_u4 = topo_bit | l_val;
                    best_u5 = 0x80 | c_val;
                }
                if (best_swr <= TARGET_SWR) break;
            }
            if (best_swr <= TARGET_SWR) break;
        }
        if (best_swr <= TARGET_SWR) break;
    }

    // Pass 2: Fine-tune +/- 3 steps around best coarse L and C values
    uint8_t base_topo = best_u4 & 0x80;
    int base_l = best_u4 & 0x7F;
    int base_c = best_u5 & 0x7F;

    for (int dl = -3; dl <= 3; dl++) {
        int l_try = constrain(base_l + dl, 0, 127);
        for (int dc = -3; dc <= 3; dc++) {
            int c_try = constrain(base_c + dc, 0, 127);
            applyRelays(base_topo | (uint8_t)l_try, 0x80 | (uint8_t)c_try);
            measureSwr();

            if (atu.fwd_watts < MIN_TUNE_POWER_W) {
                atu.tuning = false;
                applyRelays(best_u4, best_u5);
                Serial.println("ERR:TX_DROPPED");
                return;
            }

            if (atu.swr < best_swr) {
                best_swr = atu.swr;
                best_u4 = base_topo | (uint8_t)l_try;
                best_u5 = 0x80 | (uint8_t)c_try;
            }
        }
    }

    applyRelays(best_u4, best_u5);
    atu.tuning = false;
    Serial.printf("TUNED:U4=%02X,U5=%02X,SWR=%.2f\n", best_u4, best_u5, best_swr);
}

// ============================================================================
// 7. Serial Command Handler (Matches desktop-gui atu_serial.py)
// ============================================================================
void handleSerialCommands() {
    if (!Serial.available()) return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd.startsWith("SET ")) {
        unsigned int u4 = 0, u5 = 0;
        if (sscanf(cmd.c_str() + 4, "%x %x", &u4, &u5) == 2) {
            applyRelays((uint8_t)u4, (uint8_t)u5);
            Serial.printf("ACK:SET,%02X,%02X\n", atu.u4_l_mask, atu.u5_c_mask);
        }
    } else if (cmd == "TUNE") {
        runAutoTune();
    } else if (cmd == "BYPASS") {
        applyRelays(0x00, 0x00);
        Serial.println("ACK:BYPASS");
    } else if (cmd == "PING") {
        Serial.println("PONG:HYPERION_ATU_V1");
    }
}

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);

    pinMode(PIN_RCK, OUTPUT);
    digitalWrite(PIN_RCK, LOW);

    SPI.begin(PIN_SRCK, -1, PIN_SER_IN, PIN_RCK);
    applyRelays(0x00, 0x00); // Safe power-up state: full tuner bypass
}

void loop() {
    handleSerialCommands();

    // Stream live telemetry at 20 Hz (every 50 ms) to desktop-gui
    uint32_t now = millis();
    if (now - last_telemetry_ms >= 50) {
        last_telemetry_ms = now;
        measureSwr();
        Serial.printf("TEL:%.1f,%.1f,%.2f,%02X,%02X,%d\n",
                      atu.fwd_watts,
                      atu.ref_watts,
                      atu.swr,
                      atu.u4_l_mask,
                      atu.u5_c_mask,
                      atu.tuning ? 1 : 0);
    }
}