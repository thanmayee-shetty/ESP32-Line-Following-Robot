# ESP32 Line Following Robot using PID Control

An autonomous **ESP32-based Line Following Robot** that utilizes a **PID (Proportional–Integral–Derivative) controller** for accurate and stable line tracking. The robot integrates an IR sensor array, ultrasonic obstacle detection, OLED-based runtime configuration, and optional IoT telemetry for real-time monitoring.

---

## Overview

This project was developed as part of the **Embedded Systems and IoT (ESIoT)** course.

The robot continuously detects a black line using an 8-channel IR sensor array and adjusts the speed of its motors using a PID control algorithm. Additional features such as OLED menu navigation, runtime PID tuning, obstacle detection, and recovery from sharp turns and lost tracks make the robot more reliable than traditional line followers.

---

## Features

- PID-based line following
- ESP32 DevKit V1 microcontroller
- 8-channel IR sensor array
- HC-SR04 ultrasonic obstacle detection
- OLED display interface
- Interactive menu system
- Runtime tuning of PID parameters
- Flash memory storage using ESP32 Preferences
- Motor testing mode
- Sensor calibration mode
- Junction detection
- 90° turn handling
- Loop detection and recovery
- Lost-line recovery algorithm
- Differential drive motor control

---

## Hardware Components

| Component | Specification |
|-----------|---------------|
| ESP32 DevKit V1 | 240 MHz Dual-Core MCU |
| IR Sensor Array | 8-channel reflective sensor array |
| HC-SR04 | Ultrasonic distance sensor |
| L298N Motor Driver | Dual H-Bridge |
| DC Gear Motors | 6 V geared motors |
| Li-ion Battery | 7.4 V rechargeable battery |
| OLED Display | SSD1306 128×32 I²C |
| Push Buttons | Menu navigation |

---

## Software Requirements

- Arduino IDE 2.x
- ESP32 Board Package
- Wire Library
- U8g2 Library
- Preferences Library

See **requirements.md** for complete hardware and software requirements.

---

## Working Principle

1. Read the IR sensor array.
2. Calculate the line position.
3. Compute the positional error.
4. Apply the PID algorithm.
5. Generate motor speed corrections.
6. Detect obstacles using the HC-SR04 sensor.
7. Display robot status on the OLED display.
8. Handle special track conditions such as:
   - Junctions
   - 90° turns
   - Loops
   - Lost line recovery

---

## PID Controller

The robot uses a PID controller consisting of:

- **Proportional (P):** Corrects the current tracking error.
- **Integral (I):** Reduces accumulated steady-state error.
- **Derivative (D):** Predicts future error to minimize oscillations.

PID parameters can be adjusted directly from the OLED menu without modifying the source code.

---

## Repository Structure

```text
ESP32-Line-Following-Robot/
│
├── docs/
│   ├── EIOT_Project_Report.pdf
│   ├── results_video.mp4
│
├── ESP32_Line_Following_Robot.ino
├── README.md
└── requirements.md
```

---

## Getting Started

1. Install Arduino IDE.
2. Install the ESP32 Board Package.
3. Install the required libraries listed in **requirements.md**.
4. Open **ESP32_Line_Following_Robot.ino**.
5. Select your ESP32 board.
6. Upload the firmware.
7. Calibrate the sensors using the OLED menu.
8. Tune the PID parameters if required.
9. Start the robot.

---

## Project Files

| File | Description |
|------|-------------|
| `ESP32_Line_Following_Robot.ino` | Complete ESP32 firmware |
| `EIOT_Project_Report.pdf` | Detailed project report |
| `results_video.mp4` | Demonstration video |
| `requirements.md` | Hardware and software requirements |

---

## Applications

- Autonomous Mobile Robots (AMRs)
- Warehouse Automation
- Industrial Material Handling
- Robotics Education
- Embedded Systems Learning
- Research on Mobile Robotics

---

## Future Improvements

- Automatic PID tuning
- Computer vision-based line detection
- Mobile app integration
- Cloud monitoring dashboard
- Machine learning-based path optimization

---

## Authors

- Thanmayee Shetty
- Sara Patil
- Anagha Nadgouda
- Sairam H

---

## License

This project is released for educational and academic purposes.
