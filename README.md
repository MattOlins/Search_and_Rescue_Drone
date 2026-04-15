# Search and Rescue Drone

*This project was built as a final design project for an embedded systems class (ECE414)*
---

## Overview

This project is a custom built search and rescue drone designed to autonomously stabilize itself, detect people, and support emergency-response scenarios. The repository contains the full embedded flight-control stack, hardware schematics, sensor test programs, and a facial-recognition pipeline (which is run natively on the raspberry pi).

The system combines:

* Low-level flight control written in C/C++
* DShot-based ESC motor control
* Facial detection and recognition using Python
* Wiring schematics and hardware test utilities

---

## Features

* Autonomous quadcopter stabilization using PID control
* MPU6050-based orientation sensing
* Barometric altitude sensing
* DShot motor control for ESCs
* OpenCV Facial detection and recognition for locating people
* Separate development environments for both Raspberry Pi Pico and Pico 2
* Modular testing framework for motors and sensors

---

## Repository Structure

```text
Search_and_Rescue_Drone/
│
├── code/
│   ├── Flight_Controller/              # Original Raspberry Pi Pico flight controller
│   ├── Flight_Controller_Pico2/        # Updated controller for Raspberry Pi Pico 2
│   ├── Facial_Detection/               # OpenCV + facial recognition pipeline
│   ├── motor_test/                     # ESC and motor validation using DShot
│   └── sensor_test/                    # IMU and barometer test programs
│
├── schematics/
│   └── Drone_Wiring/                   # Wiring diagrams and hardware layouts
│
└── README.md
```

---

## Hardware Components

The drone platform is designed around the following hardware:

| Component                  | Purpose                           |
| -------------------------- | --------------------------------- |
| Raspberry Pi Pico / Pico 2 | Main flight controller            |
| MPU6050 IMU                | Gyroscope + accelerometer sensing |
| Barometer Sensor           | Altitude estimation               |
| Brushless Motors + ESCs    | Propulsion system                 |
| DShot-compatible ESCs      | Digital motor control             |
| Camera Module              | Facial detection and recognition  |
| LiPo Battery               | Portable power source             |

---

## Software Architecture

### 1. Flight Controller

The `Flight_Controller` and `Flight_Controller_Pico2` folders contain the embedded control software for the drone.

Core modules include:

* `controller.c` – high-level flight-state logic
* `pid.c` – PID control loops for stabilization
* `mixer.c` – converts roll/pitch/yaw corrections into motor outputs
* `dronempu.c` – IMU communication and filtering
* `bar180f.c` – altitude sensor integration
* `flight_core.c / flight_core.cpp` – overall flight-control loop

### 2. Motor Testing

The `motor_test` directory contains a standalone environment for validating motor outputs and DShot ESC communication before integrating the motors into the full controller.

### 3. Sensor Testing

The `sensor_test` directory contains separate test programs for:

* MPU6050 initialization and calibration
* Barometer readings
* Sensor debugging before full system integration

### 4. Facial Detection

The `Facial_Detection` directory contains the computer-vision pipeline used to detect and identify people.

Important files:

* `facial_recognition.py` – main real-time recognition script
* `model_training.py` – trains face encodings from the dataset
* `headshot.py` – captures images for training
* `encodings.pickle` – serialized face embeddings

The recognition system can be extended to:

* Identify missing persons
* Flag known rescue targets
* Trigger alerts when a face is detected

---

## Getting Started

### Clone the Repository

```bash
git clone https://github.com/your-username/Search_and_Rescue_Drone.git
cd Search_and_Rescue_Drone
```

---

## Building the Flight Controller

### Prerequisites

Install:

* Raspberry Pi Pico SDK
* CMake
* ARM GCC Toolchain
* Visual Studio Code or CLion (optional)

### Build Steps

```bash
cd code/Flight_Controller
mkdir build
cd build
cmake ..
make
```

For the Pico 2 version:

```bash
cd code/Flight_Controller_Pico2
mkdir build
cd build
cmake ..
make
```

---

## Running the Facial Recognition System

### Python Dependencies

```bash
pip install opencv-python face_recognition imutils numpy
```

### Train the Recognition Model

```bash
cd code/Facial_Detection
python model_training.py
```

### Start Real-Time Recognition

```bash
python facial_recognition.py
```

---

## Example Workflow

1. Test the sensors using the programs in `sensor_test/`
2. Validate ESCs and motors with `motor_test/`
3. Build and flash the flight controller onto the Pico
4. Train the facial-recognition model with target images
5. Launch the drone and begin search-and-rescue testing

---

## Future Improvements

Potential next steps for the project include:

* GPS waypoint navigation
* Thermal-camera integration
* Autonomous obstacle avoidance
* Integration with mapping software

---


## License

This project is licensed under the MIT License.

```text
MIT License
Copyright (c) 2026
```

---

## Author

**Matt Olins**
Electrical and Computer Engineering Student
Interested in embedded systems, robotics, autonomous vehicles, and search-and-rescue technology.

---