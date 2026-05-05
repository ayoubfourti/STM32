# Smart AquaCare
Smart AquaCare is an academic embedded systems project developed at **Esprit**. It implements a smart fish tank using an $${\color{blue}STM32F4 \space microcontroller}$$.
The system integrates multiple sensors and actuators to $${\color{yellow}monitor}$$ and  $${\color{yellow}regulate}$$ aquarium parameters in real time. It also demonstrates basic predictive anomaly detection using an $${\color{yellow}embedded \space AI \space model}$$.
## Hardware Architecture
| Component                     | Purpose                              |
| ----------------------------- | ------------------------------------ |
| STM32F407VG                   | Main microcontroller                 |
| DFRobot Gravity I2C LCD 16x2  | Real-time data and alert display    |
| Mini Water Pump               | Automatic water refill               |
| Water Level Sensor (Analog)   | Water level measurement  
| YL-69 Humidity Sensor         | Ambient humidity measurement         |
| LDR Sensor                    | Day/Night detection                  |
| LM35                          | Temperature measurement              |
| DC Fan                        | Thermal regulation                   |
| LEDs                          | Status indication                    |
| Push Buttons                  | Manual control / diagnostics         |
| KY-006 Buzzer                 | Audio alerts                         |

## Software Architecture
### 1. Sensor Acquisition
- ADC + DMA triggered by TIM3 every 400 ms
- Reads sensors: Water level & Temperature 
- Conversion complete triggers an ADC interrupt to update system values in real time.
### 2. Actuator Control
- TIM7 used for pump control, configured for 10 s activation cycles
- PWM controls the back blue LEDs for night mode
- DC fan control based on temperature thresholds
### 3. User Interaction
EXTI interrupts handle push buttons:
- Manual diagnostics
- Emergency actions (pump/fan override)
- Enable/disable AI-based automatic monitoring and regulation
### 4.  Time-Based Automation
- RTC implements a day/night ecosystem simulation
### 5. Embedded AI
- X-CUBE-AI used to integrate a TensorFlow Lite Micro model on STM32F4
- Performs predictive anomaly detection using sensor trends: Temperature & Water level
- AI output can trigger preventive actuator actions and alerts
## Demo on Youtube
<a href="https://www.youtube.com/watch?v=8aVTfaoqnGI">
  <img
    src="https://github.com/user-attachments/assets/1a499402-d511-4a64-959d-2551cc9109f7"
    alt="▶ Watch Demo"
    width="800"
  />
</a>
