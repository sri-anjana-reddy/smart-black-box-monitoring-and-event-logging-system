# Smart Black Box Monitoring and Event Logging System Using ARM Cortex LPC1768

## Overview
This project implements a Smart Black Box Monitoring and Event Logging System for vehicle safety applications using ARM Cortex LPC1768.

## Features
- Temperature Monitoring
- Pressure Monitoring
- Humidity Monitoring
- Voltage Monitoring
- Current Monitoring
- Vibration Monitoring
- RPM Monitoring
- Fuel Level Monitoring
- LCD Display Interface
- 4x4 Keypad Parameter Selection
- UART Event Logging
- RTC Support
- Buzzer Alert System
- LED Alert Indicators

## Hardware Used
- LPC1768 Development Board
- 16x2 LCD
- 4x4 Keypad
- RTC Module
- LEDs
- Buzzer
- UART Interface

## Software Used
- Keil uVision
- Embedded C
- Flash Magic

## Working
The user selects a parameter through the keypad. The LPC1768 generates or reads the parameter value and displays it on the LCD. If the value crosses the threshold, LEDs and buzzer are activated and an event is logged through UART.

## Author
Sri Anjana Reddy
