# CPC357 IOT ARCHITECTURE AND SMART APPLICATIONS – Project: Smart Street Lighting with Environmental Sensing and Fault Detection

## Group Members 
* Naomi Tham Kah Mun (164854) 
* Mursyidah binti Mat Jusoh (162897) 

## Project Overview  
This project implements an Automatic Street Light IoT System that operates based on ambient light and rain conditions. The system uses multiple sensors to automate street lighting, detect LED faults, and display real-time monitoring data through a web-based dashboard. Sensor data is transmitted via MQTT, stored in MongoDB, and visualized using interactive charts.

## Repository Structure  

arduino/  
streetlight.ino – Arduino code for ESP32 / Maker Feather handling sensors and MQTT publishing  

mqtt/  
iot_mqtt_mongo.py – Python MQTT subscriber that stores incoming sensor data into MongoDB  

dashboard/  
index.html – Web dashboard layout  
style.css – Dashboard styling  
script.js – MQTT subscription and chart logic  

README.md – Project documentation  

## System Architecture  

ESP32 / Maker Feather  
→ MQTT Broker (HiveMQ Public Broker)  
→ Python MQTT Subscriber  
→ MongoDB Database  
→ Web Dashboard  

## MQTT Topics  

streetlight/ldr_ambient – Ambient light LDR readings  
streetlight/rain – Rain sensor readings  
streetlight/ldr_led – LDR under LED (fault detection)  
streetlight/led_fault – LED fault status  

## System Features  

• Automatic street light control based on ambient light  
• Rain detection for environmental awareness  
• LED fault detection using LDR under LED  
• Real-time dashboard visualization  
• Data storage using MongoDB  
• Maintenance section for LED monitoring  

## Dashboard Components  

• Light condition status  
• Rain status  
• Street light ON/OFF status  
• Ambient LDR value display  
• LDR and rain sensor graphs  
• Ambient LDR vs LED LDR comparison graph  
• Maintenance section with LED fault detection  

## Fault Detection Logic  

If the street light is ON but the LED LDR value is significantly lower than the ambient LDR value, an LED fault is detected.  
LED 1 and LED 2 are displayed as static NORMAL status, while LED 3 fault status is dynamically updated via MQTT.

## Hardware Environment  

Microcontroller: ESP32 / Maker Feather  
Sensors:  
• Ambient LDR  
• LDR under LED  
• Rain sensor  
Actuators:  
• LEDs  

## Software Environment  

Operating System: Debian GNU/Linux (Bookworm)  
Programming Languages:  
• C++ (Arduino)  
• Python  
• HTML, CSS, JavaScript  

## Libraries & Tools  

Python Libraries:  
• paho-mqtt  
• pymongo  

Web Libraries:  
• Chart.js  
• MQTT.js  

## How to Run  

1. Upload Arduino Code  
Open `streetlight.ino` using Arduino IDE, select the correct board and port, and upload the code to the ESP32 / Maker Feather.

2. Start MongoDB  
```bash
sudo systemctl start mongod
