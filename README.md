# 🚗 PID Line Follower Robot

A high-speed **PID-based Line Follower Robot** built using the **ESP32 NodeMCU** and an **8-array IR sensor**. The robot uses a PID (Proportional-Integral-Derivative) control algorithm to achieve smooth and accurate line tracking at high speeds.

---

## 📌 Features

- High-speed line following using PID control
- 8-array IR sensor for precise line detection
- Differential drive with N20 geared motors
- Adjustable PID parameters for tuning
- Sensor calibration and start buttons
- Compact and competition-ready design

---

## 🛠️ Components Used

| Component | Quantity |
|-----------|-----------|
| ESP32 NodeMCU (38-pin) | 1 |
| TB6612FNG Motor Driver | 1 |
| Smart ELX RLS08 (8-Array IR Sensor) | 1 |
| Smart ELX 8-Channel Multiplexer | 1 |
| N20 DC Gear Motors (6V, 600 RPM) | 2 |
| Wheels | 2 |
| Caster Wheel | 1 |
| 3.7V Li-ion Batteries | 2 |
| MP1584 Buck Converter | 1 |
| HW133A Buck Converter | 1 |
| Push Buttons | 2 |
| Capacitors (2200 µF and 0.1 µF) | As Required |
| Robot Chassis and Connecting Wires | 1 Set |

---

## ⚙️ Working Principle

1. The 8-array IR sensor continuously reads the track.
2. The ESP32 calculates the position error.
3. The PID controller computes the correction value.
4. The TB6612FNG adjusts motor speeds accordingly.
5. The robot follows the line smoothly with minimal oscillation.

---

## 🔋 Power Distribution

- **Battery:** 7.4V (2 × 3.7V Li-ion cells)
- **Motor Driver (VM):** Connected directly to the battery.
- **ESP32:** Powered through a buck converter regulated to 5V.

---

## 🔌 Pin Connections

### Motor Driver Connections

| TB6612FNG Pin | ESP32 GPIO |
|--------------|-------------|
| PWMA | 4 |
| AIN1 | 17 |
| AIN2 | 16 |
| PWMB | 21 |
| BIN1 | 18 |
| BIN2 | 19 |
| STBY | 5 |

### Multiplexer Connections

| Multiplexer Pin | ESP32 GPIO |
|----------------|-------------|
| S0 | 27 |
| S1 | 26 |
| S2 | 25 |
| Signal | 33 |

### Buttons

| Function | ESP32 GPIO |
|----------|-------------|
| Start Button | 13 |
| Calibration Button | 14 |

---

## 🎯 PID Parameters

```cpp
double Kp = 65;
double Ki = 0.003;
double Kd = 18;
```

> These values may need tuning depending on the track and robot speed.

---

## 🚀 Future Improvements

- Automatic PID tuning
- Junction and maze solving
- Wireless parameter tuning using Bluetooth/Wi-Fi
- OLED display for debugging and calibration

---

## 👨‍💻 Author

yo5on

Built for robotics competitions and high-speed autonomous navigation.

⭐ If you like this project, don't forget to star the repository!
