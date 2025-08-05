# 🐠 Smart Fish Tank Auto-Cleaning System

An **IoT-based smart aquarium system** that detects unsafe water conditions and automatically refreshes the tank water to ensure a healthy environment for fish. The system is powered by **ESP32** and utilizes multiple sensors, a relay system, a cloud dashboard, and a mobile-controlled manual cleaning option.

---

## 🚀 Features

- 🔬 Monitors **pH**, **temperature**, and **water level**
- ⚠️ Detects **dangerous zones**:
  - pH < 6 or pH > 9
  - Temperature < 25°C or > 30°C
- 🔄 Automatically cleans water if unsafe conditions are detected:
  1. Starts the **water pump** to release 50% of the tank water
  2. Stops pump and opens the **inlet valve** to refill the tank to 100%
- 📲 **Cloud-based** monitoring and control using **Blynk**
- 📉 Real-time charts for:
  - Water Temperature
  - pH Level
  - Water Level
- 🧼 **Manual clean button**: Allows users to trigger a cleaning cycle at any time via mobile app

---

## ⚙️ Components Used

| Component              | Quantity | Description                                  |
|------------------------|----------|----------------------------------------------|
| ESP32 DevKit V1        | 1        | Main microcontroller unit                    |
| pH Sensor (Analog)     | 1        | Measures acidity/alkalinity of tank water    |
| Ultrasonic Sensor (HC-SR04) | 1  | Measures water level using sound waves       |
| Waterproof Temperature Sensor (DS18B20) | 1 | Measures water temperature       |
| Water Pump             | 1        | Drains tank water                            |
| Inlet Valve (Solenoid) | 1        | Refills tank with clean water                |
| Relay Module           | 2        | Controls pump and inlet valve                |
| 5V/12V Power Supply    | 1        | Powers sensors and actuators                 |
| Limit Switch (optional)| 1        | For safe cutoff control                      |
| Blynk IoT Cloud        | -        | Real-time dashboard and mobile app control   |

---
## 📊 System Architecture

```mermaid
flowchart TD
  A[User - Blynk App] --> B[Blynk Cloud]
  B --> C[ESP32 Microcontroller]

  C --> S1[pH Sensor]
  C --> S2[Temperature Sensor]
  C --> S3[Water Level Sensor]

  C -->|Danger Zone Detected| D[Start Auto-Cleaning]
  D --> E1[Turn ON Water Pump]
  E1 --> E2[Drain Water to 50%]
  E2 --> E3[Turn OFF Pump]
  E3 --> F1[Turn ON Inlet Valve]
  F1 --> F2[Fill to 100% Water Level]
  F2 --> F3[Turn OFF Inlet Valve]

  A -->|Manual Clean Button| D
```

## 📱 Mobile App (Blynk)

- 📍 View temperature, pH, water level in real-time
- 📈 Charts for last 24 hours
- 🔘 Manual clean button 
![WhatsApp Image 2025-08-05 at 21 02 07_fefa5887](https://github.com/user-attachments/assets/324e9205-1493-4e00-b8d2-71fa808bb431)
---


🛠️ How to Run This Project
1.Connect hardware as per circuit design  

2.Flash the ESP32 with Arduino code  

3.Setup Blynk project and get your Auth Token

4.Add widgets:

    -Value Display for Temp, pH, Water Level

    -Super Chart for real-time monitoring

    -Button for Manual Clean

5.Power up your ESP32 and start monitoring!
