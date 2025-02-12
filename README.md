# IoT Project

This project involves building an app to control and monitor an MSP432 microcontroller via Wi-Fi. Key features include tracking temperature, light intensity, noise levels, and controlling the LEDs on the MSP432.

Future enhancements include adding a user interface on the MSP432, integrating voice commands through Rasa for natural language processing, and controlling a smart light bulb based on environmental conditions.

---

## Getting Started

These instructions will help you set up the project on your local machine for development and testing.
**You can find the last APK in the root of project.**

### Prerequisites

Before getting started, ensure that you have the following installed:

- **Node.js** (version 12 or later) - [Download Node.js](https://nodejs.org/)
- **npm** (comes with Node.js)
- **Android Studio** (for Android development)
- **Xcode** (for iOS/macOS development)

---

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/Sara-uni/IoT-Project.git
   cd IoT-Project
   ```

2. Install dependecies:

   ```bash
   npm install
   ```
---

### Running the Project

#### Running the App

To run the app on Android or iOS/macOS, use the following commands:

- **For Android**:

   ```bash
   npm run android
   ```

   This command will:
   - Build the app using Vite
   - Sync with the Android platform using Capacitor
   - Open Android Studio for further testing and running

- **For iOS/macOS**:

   ```bash
   npm run ios
   ```

   This command will:
   - Build the app using Vite
   - Sync with the iOS platform using Capacitor
   - Open Xcode for testing and deployment on a device or simulator

#### Installing and Running Rasa server

To install the necessary dependency and run the rasa server used to respond to the user vocal commands there are some prerequisite:
- have python installed (ATTENTION: **version 3.10.x is required**, newer version have conflicts with some dependencies)
- have pip installed and updated (use the command "pip install --upgrade pip")

Installation procedure
- move in the "rasa-server" folder and create the virtual environment
   ```bash
      cd rasa-server
      python -m venv rasa-venv
   ```

- run the following commands to enter the virtual environment and install rasa with the necessary dependecies
   - for Linux:
      ```bash
      source rasa-venv/bin/activate
      pip install rasa
      ```
   - for Windows:
      ```bash
      rasa-venv\Scripts\activate
      pip install rasa
      ```

Now, for running the server, you have to (*optional*: train your model with ```rasa train``` and) go in rasa-core folder with
```bash
cd rasa-core
```
and run the following commands in two separate terminals:
- ```rasa run``` for run the model
- ```rasa run actions``` for the responses to the user commands

---

### Project Structure (Generic)

```
IoT-Project
├── mobile-app/
│   ├── android/         # Android project generated by Capacitor
│   ├── dist/            # Static resources
│   ├── src/             # Source code (Vue.js and Ionic)
│   │   ├── assets/      # Static files (images, etc.)
│   │   ├── components/  # Vue components
│   │   ├── views/       # Main pages of the app
│   │   └── main.js      # Entry point of the app
│   ├── capacitor.config.ts # Capacitor configuration file
│   ├── vite.config.js   # Vite configuration file
│   └── package.json     # Project dependencies
├── msp432_code/
│   └── main.c           # Main program of microcontroller
└── rasa-server/
    ├── rasa-core        # The folder containing the core of rasa
    └── rasa-venv        # The virtual environment for the dependency
```
