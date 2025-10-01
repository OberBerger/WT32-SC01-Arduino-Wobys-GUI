Software Licenses and Attributions

This product incorporates software components licensed under various open-source licenses.
Your use of this product is subject to the terms of these licenses.

**For detailed information regarding LGPL v2.1 compliance, including object code distribution and relinking instructions for statically linked LGPL libraries (such as the ESP32 Arduino Core and MFRC522 Library Suite), please refer to the "APPENDIX A: LGPL v2.1 Compliance Instructions for WobysGUI Firmware" section at the end of this document.**

Please find below the list of software components, their respective licenses,
and the required copyright notices and attribution information.

---

1.  SOFTWARE ("WobysGUI Firmware")
    ---------------------------------------------
    Version: 1.0.0
    Copyright (c) 2025 György Oberländer. All Rights Reserved.

    This product, the WobysGUI Firmware, is comprised of both proprietary and open-source software components developed by György Oberländer,
    alongside various third-party open-source libraries.

    --- LICENSING APPROACH ---
    The WobysGUI Firmware implements the following licensing model for components developed by György Oberländer:

    -   **Proprietary and Confidential Components (Closed Source):**
        These components' source code is proprietary and confidential. Unauthorized copying or redistribution of their source code,
        via any medium, is strictly prohibited. They are typically distributed as pre-compiled `.a` binaries.
        **Affected `.cpp` files (as per summary):** `BLEManager.cpp`, `WifiManager.cpp`, `UILayer.cpp`, `ScreenManager.cpp`, `TextUI.cpp`, `ButtonUI.cpp`, `ToggleButtonUI.cpp`, `ClickableListUI.cpp`, `StatusbarUI.cpp`, `ToggleSwitchUI.cpp`, `SeekbarUI.cpp`, `KeyboardUI.cpp`, `GridLayoutUI.cpp`, `ImageUI.cpp`, `MessageBoardElement.cpp`, `ScreenSaverManager.cpp`, `IconElement.cpp`, `ClockLabelUI.cpp`.

    -   **Open Source Components:**
        These components are released under a permissive Open Source Apache License 2.0,
        allowing for their free use, modification, and redistribution according to the terms of their respective licenses.
        These files explicitly carry their own license headers.
        **Affected files (as per summary):**
        *   **All `.h` files** (e.g., `Config.h`, `SystemInitializer.h`, `BLEManager.h`, `TextUI.h`, etc.)
        *   **Implementation (`.cpp` and `.ino`) files:** `WobysGUI.ino`, `SystemInitializer.cpp`, `SettingsManager.cpp`, `PowerManager.cpp`, `RFIDManager.cpp`, `LanguageManager.cpp`, `LanguageAssets.cpp`, `TimeManager.cpp`, `AudioManager.cpp`, `SDManager.cpp`, `ClickSoundData.cpp`, `MainUI.cpp`, `WifiUI.cpp`, `SettingsUI.cpp`, `BLEUI.cpp`, `UIElement.cpp`, `StatusbarElement.cpp`, `GlobalSystemEvents.cpp`.

    --- DISCLAIMER OF WARRANTY AND LIMITATION OF LIABILITY FOR PROPRIETARY COMPONENTS ---
    The proprietary components of the WobysGUI Firmware developed by György Oberländer are provided "AS IS",
    without warranty of any kind, either express or implied, including, but not limited to,
    the implied warranties of merchantability, fitness for a particular purpose, and non-infringement.
    The entire risk as to the quality and performance of these proprietary software parts is with the User.

    --- GENERAL DISCLAIMER REGARDING THIRD-PARTY COMPONENTS ---
    This firmware integrates various third-party open-source components, which are subject to their own distinct licenses.
    Any warranties and disclaimers of such warranties provided by these components, as well as liability arising therefrom,
    are governed by the specific license terms of the respective components. György Oberländer makes no warranties
    and assumes no liability for the flawless operation, performance, or suitability for a particular purpose
    of these third-party components, nor for any direct, indirect, incidental, or consequential damages
    resulting from the operation or integration of these third-party components.

    The User fully acknowledges and agrees that the liability of György Oberländer, if any, is limited solely
    to the proprietary software parts developed by György Oberländer, and the entire risk and responsibility
    regarding the operation of the product as a whole rests with the User. For legal advice concerning distribution,
    please consult with a legal professional.

---

2.  LovyanGFX Library
    -----------------
    Version: 1.2.7
    Copyright (c) 2024 Lovyan

    License: FreeBSD License

    https://github.com/lovyan03/LovyanGFX/blob/master/license.txt

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    1.  Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.
    2.  Redistributions in binary form must reproduce the above copyright notice,
        this list of conditions and the following disclaimer in the documentation
        and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

---

3.  Arduino Core for ESP32
    -----------------------
    Version: 3.3.1
    Copyright (c) 2016-2024 Espressif Systems (Shanghai) Co., Ltd.
    Portions Copyright (c) 2015-2016 Ivan Grokhotkov.

    License: LGPL 2.1 (GNU Lesser General Public License, Version 2.1)

    https://github.com/espressif/arduino-esp32/blob/master/LICENSE.md

    Compliance Note: This library is statically linked. Please refer to "APPENDIX A: LGPL v2.1 Compliance Instructions for WobysGUI Firmware" for detailed relinking instructions and compliance strategy.

---

4.  ArduinoJson Library
    -------------------
    Version: 7.4.2
    Copyright (c) 2014-2024 Benoit Blanchon

    License: MIT License

    https://arduinojson.org/?utm_source=meta&utm_medium=library.properties

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

---

5.  LittleFS Filesystem Library
    ------------------------------------------------------------------
    Version: Integrated with Arduino Core for ESP32 v3.3.0
    Copyright (c) 2017-2020 ARM Limited (original LittleFS by Gordon Williams)
    Portions Copyright (c) 2019-2020 Espressif Systems (Shanghai) Co., Ltd. (for ESP-IDF integration)

    License: BSD 3-Clause License

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    3.  Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

---

6.  MFRC522 Library Suite (MFRC522v2, MFRC522DriverSPI, MFRC522DriverPinSimple, MFRC522Debug)
    -----------------------------------------------------------------------------------------
    Version: 2.0.6
    Copyright (c) 2013-2023 Miguel Balboa, F. de Waard, and others.

    License: GNU Lesser General Public License v2.1

    https://github.com/OSSLibraries/Arduino_MFRC522v2/blob/master/LICENSE

    Compliance Note: This library is statically linked. Please refer to "APPENDIX A: LGPL v2.1 Compliance Instructions for WobysGUI Firmware" for detailed relinking instructions and compliance strategy.

---

7. NimBLE Bluetooth Low Energy Host Stack (Integrated with Arduino Core for ESP32)
    -------------------------------------------------------------------------------
    Version: 2.3.6 (As integrated with Arduino Core for ESP32 v3.3.0 or later)
    Copyright (c) 2017 - 2021 Apache Software Foundation (ASF)
    Copyright (c) 2017 - 2024 Espressif Systems (Shanghai) Co., Ltd.
    Portions Copyright (c) 2015-2016 Ivan Grokhotkov.
    Portions Copyright (c) 2019-2024 h2zero (for NimBLE-Arduino modifications)

    License: Apache License 2.0 (A full copy of the Apache License 2.0 can be found at: https://www.apache.org/licenses/LICENSE-2.0)

    Compliance Requirements:
    - You must provide a copy of this License to recipients of your software.
    - You must retain all copyright, patent, trademark, and attribution notices.
    - If the Work includes a "NOTICE" text file, you must include a readable copy of its attribution notices.
    - You must cause any modified files to carry prominent notices stating that You changed the files.
    - Disclaimer of Warranty and Limitation of Liability apply.

    Please refer to the official Apache License 2.0 text for full details and obligations.

---

8.  ProFont
    ---------
    Version: ProFont Distribution 2.2
    Copyright (c) 2014 Carl Osterwald, Stephen C. Gilardi, Andrew Welch

    License: MIT License

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

---

9.  Open Iconic Fonts
    -------------------------------------------------------------
    Version: 2
    Copyright (c) 2016, olikraus@gmail.com

    License: 2-Clause BSD License

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
        and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

---

10.  Adobe / Digital Equipment Corporation Fonts (e.g., Helvetica, Courier, Times, New Century Schoolbook)
    ------------------------------------------------------------------------------------------------------
    Version: 1984-1989, 1994
    Copyright (c) 1984-1989, 1994 Adobe Systems Incorporated.
    Copyright (c) 1988, 1994 Digital Equipment Corporation.
    Font Copyright Statement: "Copyright (c) 1984, 1987 Adobe Systems Incorporated. All Rights Reserved.
                              Copyright (c) 1988, 1991 Digital Equipment Corporation. All Rights Reserved."

    License: Custom Permissive License (as provided in your code snippet)

    Permission to use, copy, modify, distribute and sell this software and its documentation for any purpose and without fee is hereby granted,
    provided that the above copyright notices appear in all copies and that both those copyright notices
    and this permission notice appear in supporting documentation, and that the names of Adobe Systems
    and Digital Equipment Corporation not be used in advertising or publicity pertaining to distribution of the software without specific, written prior permission.

    Adobe Systems and Digital Equipment Corporation make no representations about the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    Special Note: This license explicitly states that the names of "Adobe Systems" and "Digital Equipment Corporation"
    cannot be used in advertising or publicity without specific prior written permission.

---


**IMPORTANT DISCLAIMER:**
This document is generated based on provided code snippets and common open-source license practices. It is intended for informational purposes only and does not constitute legal advice. It is your responsibility to:
1.  Verify the exact version and license of every third-party component used in your final product.
2.  Consult with a legal professional to ensure full compliance with all applicable license terms, especially when distributing a commercial product.
3.  Include all required license texts and copyright notices in your product's documentation or user interface as mandated by each license.

---
---

## APPENDIX A: LGPL v2.1 Compliance Instructions for WobysGUI Firmware

This appendix provides instructions necessary for compliance with the GNU Lesser General Public License, Version 2.1 (LGPL v2.1) as applied to the WobysGUI Firmware. Our firmware utilizes LGPL v2.1-licensed libraries, notably the ESP32 Arduino Core and MFRC522 Library Suite, which are statically linked into the final executable.

Under LGPL v2.1 Section 3, recipients of a derivative work that statically links an LGPL-licensed library must be provided with the object code of the application, allowing them to modify the LGPL Library and then relink to produce a modified executable containing the modified Library.

---

### IMPORTANT DISCLAIMER:
This document is for informational purposes only and does not constitute legal advice. It is your responsibility to consult with a legal professional to ensure full compliance with all applicable license terms, especially when distributing a commercial product.

---

### WobysGUI Firmware Components & Licensing Overview

The WobysGUI Firmware is composed of the following:

*   **Proprietary and Confidential Components (Closed Source):**
    *   **Source Code:** NOT PROVIDED. These are proprietary code developed by György Oberländer.
    *   **Provided form:** Object files (`.o` files).
    *   **Proprietary License:** Governed by separate proprietary license terms provided with the product.
    *   **List of proprietary `.o` files included in this package:**
        *   `BLEManager.cpp.o`
        *   `WifiManager.cpp.o`
        *   `UILayer.cpp.o`
        *   `ScreenManager.cpp.o`
        *   `TextUI.cpp.o`
        *   `ButtonUI.cpp.o`
        *   `ToggleButtonUI.cpp.o`
        *   `ClickableListUI.cpp.o`
        *   `StatusbarUI.cpp.o`
        *   `ToggleSwitchUI.cpp.o`
        *   `SeekbarUI.cpp.o`
        *   `KeyboardUI.cpp.o`
        *   `GridLayoutUI.cpp.o`
        *   `ImageUI.cpp.o`
        *   `MessageBoardElement.cpp.o`
        *   `ScreenSaverManager.cpp.o`
        *   `IconElement.cpp.o`
        *   `ClockLabelUI.cpp.o`

*   **Open Source Components (Apache License 2.0):**
    *   **Source Code:** PROVIDED in this package.
    *   **License:** Apache License 2.0.
    *   **List of Apache 2.0 source files included in this package:**
        *   `WobysGUI.ino`
        *   `SystemInitializer.cpp`, `SystemInitializer.h`
        *   `SettingsManager.cpp`, `SettingsManager.h`
        *   `PowerManager.cpp`, `PowerManager.h`
        *   `RFIDManager.cpp`, `RFIDManager.h`
        *   `LanguageManager.cpp`, `LanguageManager.h`
        *   `LanguageAssets.cpp`, `LanguageAssets.h`
        *   `TimeManager.cpp`, `TimeManager.h`
        *   `AudioManager.cpp`, `AudioManager.h`
        *   `SDManager.cpp`, `SDManager.h`
        *   `ClickSoundData.cpp`, `ClickSoundData.h`
        *   `MainUI.cpp`, `MainUI.h`
        *   `WifiUI.cpp`, `WifiUI.h`
        *   `SettingsUI.cpp`, `SettingsUI.h`
        *   `BLEUI.cpp`, `BLEUI.h`
        *   `UIElement.cpp`, `UIElement.h`
        *   `StatusbarElement.cpp`, `StatusbarElement.h`
        *   `GlobalSystemEvents.cpp`, `GlobalSystemEvents.h`
        *   `Config.h`, `ConfigAudioUser.h`, `ConfigFonts.h`, `ConfigHardwareUser.h`, `ConfigLGFXUser.h`, `ConfigUIUser.h`
        *   `ListItem.h`, `_FixIt.h`, `_Licenses.h`, `_Struct.h`

*   **Third-Party LGPL v2.1-licensed Libraries (Source Code links provided):**
    *   **ESP32 Arduino Core:**
        *   **Source Code:** [https://github.com/espressif/arduino-esp32](https://github.com/espressif/arduino-esp32)
        *   **Version used:** 3.3.0
        *   **License:** LGPL v2.1 ([https://github.com/espressif/arduino-esp32/blob/master/LICENSE.md](https://github.com/espressif/arduino-esp32/blob/master/LICENSE.md))
    *   **MFRC522 Library Suite:**
        *   **Source Code:** [https://github.com/OSSLibraries/Arduino_MFRC522v2](https://github.com/OSSLibraries/Arduino_MFRC522v2)
        *   **Version used:** 2.0.6
        *   **License:** LGPL v2.1 ([https://github.com/OSSLibraries/Arduino_MFRC522v2/blob/master/LICENSE](https://github.com/OSSLibraries/Arduino_MFRC522v2/blob/master/LICENSE))

*   **Third-Party Permissively-licensed Libraries (Source Code links provided):**
    *   **LovyanGFX Library:**
        *   **Source Code:** [https://github.com/lovyan03/LovyanGFX](https://github.com/lovyan03/LovyanGFX)
        *   **Version used:** 1.2.7
        *   **License:** FreeBSD License ([https://github.com/lovyan03/LovyanGFX/blob/master/license.txt](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt))
    *   **ArduinoJson Library:**
        *   **Source Code:** [https://arduinojson.org/](https://arduinojson.org/) (Link from library.properties)
        *   **Version used:** 7.4.2
        *   **License:** MIT License
    *   **LittleFS Filesystem Library:**
        *   **Source Code:** Integrated within ESP32 Arduino Core. Refer to ESP32 Arduino Core source link.
        *   **Version used:** Integrated with Arduino Core for ESP32 v3.3.0
        *   **License:** BSD 3-Clause License
    *   **NimBLE Bluetooth Low Energy Host Stack:**
        *   **Source Code:** Integrated within ESP32 Arduino Core (often a specific NimBLE-Arduino fork). Refer to ESP32 Arduino Core source link.
        *   **Version used:** 2.3.6 (As integrated with Arduino Core for ESP32 v3.3.0 or later)
        *   **License:** Apache License 2.0
    *   **ProFont:**
        *   **Source Code:** (Often distributed as a `.c` file, no central repo)
        *   **Version used:** ProFont Distribution 2.2
        *   **License:** MIT License
    *   **Open Iconic Fonts:**
        *   **Source Code:** (Often distributed as a `.c` file, e.g., by U8g2)
        *   **Version used:** 2
        *   **License:** 2-Clause BSD License
    *   **Adobe / Digital Equipment Corporation Fonts:**
        *   **Source Code:** (Often distributed as `.c` files in graphics libraries)
        *   **Version used:** 1984-1989, 1994
        *   **License:** Custom Permissive License (as provided in the `adobex11font.c` header)

---

### Step-by-Step Relinking Instructions:

**(Note: This is a general guide. Specific linker flags and paths may vary slightly based on your exact environment and chosen build system. These instructions assume you are working from a directory where all necessary `.o` files are generated.)**

1.  **Obtain LGPL Library Source Code:**
    *   Download the source code for ESP32 Arduino Core v3.3.0 from its GitHub repository: [https://github.com/espressif/arduino-esp32/releases/tag/3.3.0](https://github.com/espressif/arduino-esp32/releases/tag/3.3.0)
    *   Download the source code for ESP8266Audio (specify version if known, e.g., 2.3.0, otherwise latest): [https://github.com/earlephilhower/ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio)
    *   Download the source code for MFRC522 Library Suite (specify version if known, e.g., 2.0.6, otherwise latest): [https://github.com/OSSLibraries/Arduino_MFRC522v2](https://github.com/OSSLibraries/Arduino_MFRC522v2)

2.  **(Optional) Modify LGPL Library Source:**
    *   Make your desired changes to the LGPL library source files.

3.  **Compile LGPL Library Source Files:**
    *   Compile the LGPL library source files (your modified ones, or the original if no modifications) into `.o` object files. This typically involves using `xtensa-esp32-elf-g++` with specific flags matching the Arduino Core build environment (e.g., `-Os -std=gnu++20 -ffunction-sections -fdata-sections` etc.).
    *   You will also need to compile any other LGPL object files from the ESP32 Arduino Core that your project uses, if they are not already provided by the core in `.a` archives. A full Arduino build will typically generate these `.o` files and then archive them.

4.  **Prepare Your Application's Object Files:**
    *   For your **Proprietary Components:** Use the provided `.o` files in this package.
    *   For your **Open Source (Apache 2.0) Components:** You can either compile them yourself from the provided source code (using `xtensa-esp32-elf-g++`) or use their `.o` files if also provided in this package.

5.  **Relink All Object Files:**
    *   Identify all `.o` files from your application (proprietary, Apache 2.0), and all `.o` files (or `.a` archives) from the LGPL libraries (your modified versions, or the originals from the Arduino Core installation).
    *   Use the `xtensa-esp32-elf-g++` command to link them together. The order of linking the libraries (`-l` flags) is important. You might need to use `--whole-archive` for certain LGPL `.a` files to ensure all necessary `.o` files from those archives are included, especially if they have reverse dependencies.

    **Example Linker Command (Simplified, actual command will be much longer):**

    ```bash
    # Assuming all .o files are in the current directory or specified via -L/-I
    # You will need to determine the full set of .o files and archives
    # from your Arduino build process that are part of the final firmware.

    xtensa-esp32-elf-g++ \
        <YOUR_PROPRIETARY_OBJ_FILE_1>.o \
        <YOUR_PROPRIETARY_OBJ_FILE_2>.o \
        ... # all your proprietary .o files

        <YOUR_APACHE_2_0_OBJ_FILE_1>.o \
        <YOUR_APACHE_2_0_OBJ_FILE_2>.o \
        ... # all your Apache 2.0 .o files (or source if you compile them)

        # LGPL libraries (replace with your compiled LGPL .o files or reference .a archives)
        # Use -L and -l for archives, or list individual .o files
        -L<PATH_TO_ESP32_ARDUINO_CORE_LIBS> -l<arduino_core_lib1> -l<arduino_core_lib2> ... \
        -L<PATH_TO_ESP8266AUDIO_LIBS> -l<esp8266audio_lib> \
        -L<PATH_TO_MFRC522_LIBS> -l<mfrc522_lib> \
        # ... other LGPL .o files or archives you want to relink ...

        # Other permissively licensed libraries (replace with their .a files or list .o files)
        -L<PATH_TO_LOVYANGFX_LIBS> -l<lovyangfx_lib> \
        -L<PATH_TO_ARDUINOJSON_LIBS> -l<arduinojson_lib> \
        # ... etc.

        -o <YOUR_FIRWARE_EXECUTABLE>.elf \
        -nostdlib \
        -Wl,--gc-sections \
        -T <PATH_TO_LINKER_SCRIPT>.ld \
        # ... other linker flags from your Arduino build ...
    ```

    **Finding the Full Linker Command:** The easiest way to get the exact linker command is to compile your project once in the Arduino IDE with "Show verbose output during compilation" enabled in preferences. The last few lines of the output will show the full `xtensa-esp32-elf-g++` command used for linking. Copy this command and replace the LGPL library references (e.g., specific LGPL `.a` files) with your modified ones.

---

### Contact for LGPL Compliance Questions

If you have any questions regarding this LGPL Compliance Package or require assistance with relinking, please contact:

György Oberländer
support@wobys.com

---

---

**IMPORTANT DISCLAIMER:**
This document is generated based on provided code snippets and common open-source license practices. It is intended for informational purposes only and does not constitute legal advice. It is your responsibility to:
1.  Verify the exact version and license of every third-party component used in your final product.
2.  Consult with a legal professional to ensure full compliance with all applicable license terms, especially when distributing a commercial product.
3.  Include all required license texts and copyright notices in your product's documentation or user interface as mandated by each license.