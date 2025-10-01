# WT32-SC01-Plus-Arduino-Wobys-GUI

Modular, touchscreen GUI framework for ESP32 (WT32-SC01 Plus). Features Wi-Fi, BLE, NTP, battery, RFID, audio management in modern C++. Demo has limited interactions. Video, guides & full version at wobys.com. Great base for embedded touchscreen projects!

## Overview

This project provides a comprehensive and modular Graphical User Interface (GUI) framework designed for the WT32-SC01 Plus development board, leveraging the ESP32 microcontroller and the Arduino framework. It offers a rich set of features for managing essential embedded system functionalities through an intuitive touchscreen interface.

### Key Features:

*   **Modular Architecture:** Built with a clean, manager-based design (e.g., `ScreenManager`, `WifiManager`, `BLEManager`) for enhanced reusability and maintainability.
*   **Dynamic UI:** Supports multiple UI layers, screen transitions, and adaptable layouts for various screen orientations.
*   **Connectivity:** Seamless Wi-Fi and Bluetooth Low Energy (BLE) management, including scanning, connection, and persistent settings.
*   **System Services:** Integrated NTP for time synchronization, comprehensive battery monitoring, and RFID tag reading capabilities.
*   **Audio Feedback:** Basic audio output for UI clicks and system sounds.
*   **Settings Management:** Persistent storage of user preferences and device configurations using LittleFS.
*   **Internationalization:** Multi-language support (English and Hungarian included).

## Demo Version Limitations

This repository hosts a **demo version** of the Wobys GUI firmware. It includes functional limitations to showcase the framework's capabilities without providing full, unrestricted access. These limitations include:

*   **Limited UI Interactions:** A maximum number of UI modifications (e.g., button presses, seekbar changes) are allowed.
*   **Limited Scans:** Restricted number of Wi-Fi and BLE scans.

Upon reaching these limits, the system will display a notification message. For the **full, unrestricted version** of the firmware and all its capabilities, please visit: **[wobys.com](https://wobys.com/)**

## Hardware

This project is specifically developed for the **WT32-SC01 Plus** development board. While the core framework might be adaptable to other ESP32 boards with a compatible display, the pin assignments and certain hardware-specific configurations are optimized for this board.

## Visuals (Video & Screenshots)

A demo video showcasing the Wobys GUI in action is available on YouTube:

**[Wobys GUI Demo Video](https://www.youtube.com/watch?v=vYAENE0a03o)**

Further screenshots, GIFs, and detailed visual demonstrations of the interface can be found on our official website: **[wobys.com](https://wobys.com/)**

## Installation & Build

Detailed instructions on how to set up your development environment (e.g., Arduino IDE, PlatformIO), build the firmware, and upload it to your WT32-SC01 Plus board are available on our official website: **[wobys.com](https://wobys.com/)**

## Licensing & Attribution

This project is a hybrid software product, incorporating components under various open-source licenses, alongside proprietary elements.

*   **Open Source Components (WobysGUI Firmware - `György Oberländer`'s Open Source contributions):**
    *   **License:** Apache License 2.0.
    *   This applies to header files (`.h`) and specific implementation files (`.cpp`, `.ino`) as detailed in the `LICENSES.txt` document.

*   **Proprietary and Confidential Components (WobysGUI Firmware - `György Oberländer`'s Closed Source contributions):**
    *   These components' source code is proprietary and confidential. They are typically distributed as pre-compiled `.a` binaries, but their `.o` object files are provided for LGPL compliance (see below).

*   **Third-Party Open Source Libraries:**
    *   This project integrates numerous third-party libraries, each governed by their respective open-source licenses (e.g., **GNU Lesser General Public License v2.1 (LGPL-2.1)** for ESP32 Arduino Core and MFRC522 Library Suite, **FreeBSD License** for LovyanGFX Library, **MIT License** for ArduinoJson, **Apache License 2.0** for NimBLE, etc.).

**For a complete and detailed list of all software components, their respective licenses, copyright notices, and specific compliance instructions (especially for LGPL v2.1 static linking requirements, including the provision of `.o` object files), please refer to the `LICENSES.txt` file located in the root of this repository.**

**NOTE ON LGPL COMPLIANCE:** Due to the static linking of LGPL-licensed libraries, this repository intentionally includes the compiled object files (`.o` files) of certain components. This is to ensure compliance with LGPL v2.1 Section 3, allowing recipients to modify the LGPL Library and relink it with the application's object code.

## Development Status

This project is actively maintained. Feedback, bug reports, and suggestions are welcome. Please note that direct contributions (pull requests) to this demo repository may be limited due to the hybrid nature of the project.

## Contact

For any inquiries, support, or information regarding the full version of the Wobys GUI firmware, please contact:

György Oberländer
[support@wobys.com](mailto:support@wobys.com)
**[wobys.com](https://wobys.com/)**

## Internal Files (for context/development)

The files `_FixIt.h` and `_Struct.h` are included in this repository primarily for historical context or specific internal development purposes, even though they may not directly represent functional parts of the final firmware. Their presence is maintained to provide a complete picture of the project's evolution.
