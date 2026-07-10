# Microclimate Weather Station

An ultra-low-power, autonomous meteorological station designed for microclimate monitoring and real-time local weather forecasting.

---

## 🚀 Features

### 📡 Smart Environmental Monitoring & Forecasting
* **Zambretti Forecast Engine:** Implements a scientifically adjusted Zambretti algorithm tailored for localized thermal baselines, utilizing a rolling 3-hour barometric pressure and humidity window.
* **Microclimate Tracking:** High-precision barometric pressure readings combined with real-time relative humidity analytics.
* **Core & Ambient Temperature:** Dedicated thermistor integration for accurate internal systems thermal management and ice/frost safety warnings.

### 🔋 Advanced Power & Energy Management
* **Deep Sleep Infrastructure:** Hardware-timed deep sleep cycles minimizing current consumption to microamps ($\mu\text{A}$) during idle states.
* **RTC-Driven Hardware Interrupts:** Real-Time Clock (RTC) wake-up schedules for precise, time-triggered sensor sampling loops.
* **Manual User Override:** Asynchronous hardware switch enabling the user to wake the system on-demand for immediate display readouts.

### 💾 Data Persistence & UI
* **Non-Volatile Storage (EEPROM):** Persistent logging of historical telemetry data, including absolute minimum/maximum values for temperature and humidity across reboots.
* **Volatile Storage:** Persistent logging of daily telemetry data, including absolute minimum/maximum values for temperature and humidity.
* **Field-Ready Display:** Rich UI rendering comprehensive real-time diagnostics, system health metrics, forecasts, and timeframe windows.

---

## 🛠️ Hardware Architecture

The system utilizes a balanced, highly efficient component chain:

* **Microcontroller:** ATmega328P (Arduino Nano architecture optimized for low-power operation)
* **Primary Sensors:** BME280 (Pressure), DHT22 (Humidity), and NTC Thermistor (System Temperature)
* **Current Sensor:** ACS712 and Voltage Divider
* **Timekeeper:** DS3231 RTC Module

---

## ⚙️ Firmware Architecture & Compilation

The project structure is split into modular C++ classes to enforce strict separation of concerns, technical accuracy, and flash memory efficiency.
