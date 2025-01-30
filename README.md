# Engine Overheat Cooling System (EOCS)

## Overview
The **Engine Overheat Cooling System (EOCS)** is designed to monitor and control engine temperature to prevent overheating. The system integrates a **temperature sensor, cooling fan, LED alert, and data logging mechanism**.

### System Workflow
1. A **DHT20 temperature sensor** continuously monitors engine temperature.
2. When the temperature **exceeds a preset threshold**, the sensor sends data to the **LILYGO-TTGO ESP32 Board**.
3. The **ESP32 triggers a DC motor** to power a cooling fan to reduce the temperature.
4. Simultaneously, an **LED is activated** to alert the driver.
5. The **ESP32 transmits temperature data** to an **AWS cloud server** via **Wi-Fi**.
6. The data is analyzed for overheating trends, such as specific laps or track sections where overheating occurs.
7. The collected data is visualized in a **temperature vs. time graph** for deeper insights into vehicle performance optimization.

## Milestones
- **Component Selection & Integration**: Choosing and integrating temperature sensors, cooling fans, and LEDs.
- **Microcontroller Logic Design**: Programming ESP32 to trigger the fan and LED based on temperature readings.
- **Communication Protocol Setup**: Establishing a **Wi-Fi-based** data logging and remote monitoring system.
- **Data Analysis & Overheating Prediction**: Implementing data analytics to **predict overheating patterns** and improve performance.

## Hardware Components
| Component/Part | Quantity |
|---------------|----------|
| LILYGO-TTGO ESP32 Board | 1 |
| DC Motor + 9V Battery + L298N Control Board | 1 Set |
| LED | 1 |
| Wires | Many |
| Resistors | 3 |
| Breadboard | 1 |
| DHT20 Temperature Sensor | 1 |

## Future Enhancements
- Implement **real-time alerts via mobile notifications**.
- Enhance **predictive analysis using AI/ML** for better overheating prevention.
- Optimize **power consumption** for longer operational time.

---
**Contributors**: Team EOCS  
**License**: MIT

