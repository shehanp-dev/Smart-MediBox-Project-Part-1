# MediBox – Smart Medication Reminder System

![ESP32](https://img.shields.io/badge/Platform-ESP32-blue)  
![Language](https://img.shields.io/badge/Language-Arduino-green)  
![Simulation](https://img.shields.io/badge/Simulation-Wokwi-orange)  
![License](https://img.shields.io/badge/License-MIT-brightgreen)

---

## 📌 Overview
MediBox is a smart medication reminder system designed to assist patients in taking medications on time. It uses an ESP32 microcontroller, OLED display, and DHT22 sensor to provide real-time monitoring and notifications.

**Key Features:**
- Medication reminders with buzzer and LED alerts
- Time synchronization via NTP server
- Temperature & humidity monitoring
- Wi-Fi connectivity
- Interactive menu with push buttons

---

## 🛠 Hardware Components
- ESP32 Development Board  
- OLED Display (128x64)  
- DHT22 Temperature & Humidity Sensor  
- Buzzer  
- LEDs  
- Push Buttons  

---

## 📂 Repository Structure
```
MediBox/
 ├── sketch.ino           # Main ESP32 code
 ├── diagram.json         # Wokwi circuit diagram
 ├── libraries.txt        # Required Arduino libraries
 ├── wokwi-project.txt    # Wokwi simulation metadata
 ├── README.md            # Project documentation
 ├── LICENSE              # MIT License
 ├── .gitignore           # Ignore unnecessary files
 └── images/              # Circuit and PCB diagrams
      ├── circuit-diagram.png
      ├── pcb-layout.png
      └── device-photo.jpg
```

---

## ✅ Functionalities
- Time Synchronization: Syncs with NTP server for accurate timekeeping.  
- Alarm System: Set multiple alarms for medication reminders.  
- Temperature & Humidity Monitoring: Display readings and alert on threshold breach.  
- User Interaction: Configure alarms and time zones via push buttons.  
- Notifications: Audible and visual alerts for medication time.  

---

## 🔧 Setup & Usage
### 1. Hardware Setup
- Connect components as per circuit diagram.  

### 2. Install Libraries
Refer to `libraries.txt`:
```
Wire.h
Adafruit_GFX.h
Adafruit_SSD1306.h
DHTesp.h
WiFi.h
```

### 3. Upload Code
- Open `sketch.ino` in Arduino IDE.
- Select Board: ESP32 Dev Module.
- Upload to ESP32.

### 4. Connect to Wi-Fi
- Ensure Wi-Fi credentials are updated in the code.
- Device auto-connects and fetches time via NTP.

### 5. Set Time Zone & Alarms
- Navigate menu using push buttons.
- Configure time zone offsets and alarm times.

---

## 🖼 Circuit & Layouts
![Circuit Diagram](images/circuit-diagram.png)  


---

## 🌐 Simulation
View and test this project on Wokwi:  
👉 [Open Wokwi Simulation](#)

---

## 📜 License
This project is licensed under the MIT License – see LICENSE for details.

---

## 🔮 Future Improvements
- Add a mobile app for remote monitoring
- Implement voice notifications
- Support for multiple users and medicine schedules
