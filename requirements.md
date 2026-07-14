# Requirements

## Hardware Requirements

| Component | Specification | Purpose |
|-----------|---------------|---------|
| ESP32 DevKit V1 | 240 MHz dual-core, 520 KB SRAM, Wi-Fi + Bluetooth | Main controller running PID control and IoT communication |
| IR Sensor Array | 5 × TCRT5000 reflective sensors | Line detection |
| HC-SR04 Ultrasonic Sensor | 2–400 cm range, ~3 mm resolution | Obstacle detection |
| L298N Motor Driver | Dual H-Bridge, 2 A/channel, 5–46 V | Controls motor speed and direction |
| DC Gear Motors (×2) | 6 V, 200 RPM | Differential drive |
| Li-ion Battery Pack | 7.4 V, 2 Ah | Robot power supply |
| Voltage Regulator | 5 V / 3.3 V output | Regulated power for ESP32 and sensors |
| Chassis + Caster | Two-wheel differential drive | Robot platform |

---

## Software Requirements

| Software | Version | Purpose |
|----------|---------|---------|
| Arduino IDE | 2.x or later | Firmware development |
| ESP32 Board Package | Latest | ESP32 support in Arduino IDE |
| Wire Library | Built-in | I2C communication |
| U8g2 Library | Latest | OLED display |
| Preferences Library | Built-in | Store PID parameters in flash memory |

---

## Optional Tools

- MQTT Broker (Mosquitto)
- Node-RED
- Grafana

These are required only if IoT telemetry and cloud monitoring features are enabled.
