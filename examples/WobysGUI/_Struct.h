PROJECT ARCHITECTURE OVERVIEW

This diagram illustrates the modular architecture of the WobysGUI firmware,
highlighting the distinction between Open Source (OS) components (source code provided)
and Closed Source (CS) components (distributed as pre-compiled .a binaries).

Key:
  [OS]   : Open Source (source code provided to user)
  [CS]   : Closed Source (compiled .a binary, source code not provided)
  --->   : "Includes" or "Depends on"
  ===>   : "Communicates with" (via API/callbacks)
  -----  : Logical separation / Code boundary

========================================================================================================================================================================

[ END-USER APPLICATION LAYER ]
                                        (User Compiles & Links)
                                                 |
                                                 |
                        +-----------------------------------------------------------------------------------------------------------------------------------------+
                        |                                                   `WobysGUI.ino`                                                                        |  [OS] (Template code for user modification)
                        |                                             (Main Application Entry Point)                                                              |
                        |     (Orchestrates Object Instantiation, System Setup, Initialization Flow, Main Loop Execution, and Top-Level Callbacks)                |
                        +-----------------------------------------------------------------------------------------------------------------------------------------+
                                                 |
                                                 |  Dependencies & Initialization Data Transfer
                                                 |
                       +-----------------------------------------------------------------------------------------------------------------------------------------+
                       |                                                   `SystemInitializer.h` (OS)                                                            |
                       |                          (Central Orchestrator for Comprehensive System & UI Setup)                                                     |
                       |              (Reads configurations from `Config.h`, then passes parameters to Manager and UI components' `init()` methods)              |
                       +-----------------------------------------------------------------------------------------------------------------------------------------+
                                                 |  Configuration Data Passed (via API)
                                                 |  References to Manager & UI Objects
                                                 V
+---------------------------------------------+
| `SystemInitializer.cpp` (OS)(D)             |  (Implementation - OS)
+---------------------------------------------+
                        |  (Sets Global Callbacks)
                        +===================>
                                          +---------------------------------------------+
                                          | `GlobalSystemEvents.h` (OS)                 |
                                          |  (Global System Events & Callbacks Decl.)   |
                                          |  - Provides common event interface for      |
                                          |    system services (e.g., audio, UI actions)|
                                          +---------------------------------------------+
                                                                    ^ (Declaration for definition)
                                                                    |
                                          +---------------------------------------------+
                                          | `GlobalSystemEvents.cpp` (OS)               | // DEFINITIONS for global system callbacks
                                          +---------------------------------------------+
                                                      ^ (Calls Global Callbacks)
                                                      |
+---------------------------------------------+       |
| `WobysGUI.ino`                              |       |
| (Main Application Entry Point)              |<======
+---------------------------------------------+


========================================================================================================================================================================
 [ GLOBAL CONFIGURATIONS & LANGUAGE STRINGS (OPEN SOURCE) ]
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 These .h files contain all user-configurable settings and static data.
 All other source code components (both OS .h files and CS .h files as API interfaces) reference these.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------

+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
| `Config.h` (OS)                             |      | `ConfigHardwareUser.h` (OS)                 |      | `ConfigLGFXUser.h` (OS)                     |
|   (Master Configuration Include File)       |      |  (User-Configurable Hardware Pins & Params) |      |  (User-Configurable LovyanGFX Display/Touch)|
|   - Serves as the primary include point for |      |                                             |      |                                             |
|     all other configuration sub-files.      |      |                                             |      |                                             |
|                                             |      |                                             |      |                                             |
|   ---> `ConfigHardwareUser.h`               |      |                                             |      |                                             |
|   ---> `ConfigLGFXUser.h`                   |      |                                             |      |                                             |
|   ---> `ConfigUIUser.h`                     |      |                                             |      |                                             |
|   ---> `ConfigFonts.h`                      |      |                                             |      |                                             |
|   ---> `ConfigAudioUser.h`                  |      |                                             |      |                                             |
|   ---> `LanguageAssets.h`                   |      |                                             |      |                                             |
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
                          ^                                                  ^                                                  ^
                          | (Dependency Chain)                               | (Dependency Chain)                               | (Dependency Chain)
                          |                                                  |                                                  |
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
| `ConfigUIUser.h` (OS)                       |      | `ConfigFonts.h` (OS)                        |      | `ConfigAudioUser.h` (OS)                    |
| (User-Configurable UI Styles, Colors, etc.) |      |  (Font Declarations & Static Instances)     |      |  (Audio Output Configuration)               |
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
                          ^
                          | (Dependency Chain)
                          V
+---------------------------------------------+      +---------------------------------------------+
| `LanguageAssets.h` (OS)                     |      | `LanguageAssets.cpp` (OS)                   | // DEFINITIONS for language strings
|  (UI Text Strings for Localization)         |      |                                             |
+---------------------------------------------+      +---------------------------------------------+


========================================================================================================================================================================
 [ COMMON DATA STRUCTURES (OPEN SOURCE) ]
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 These .h files define common data structures used across various OS and CS components.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------

+---------------------------------------------+
| `ListItem.h` (OS)                           |
|  (Data structures for lists, WiFi, BLE)     |
+---------------------------------------------+

========================================================================================================================================================================
 [ SYSTEM MANAGERS (MIXED SOURCE) ]
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 Core logic for Wifi/BLE are CS. Other manager implementations are OS.
 Only the .h API files are distributed as source code for user interaction.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------

+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
| `SettingsManager.h` (OS)                    |      | `PowerManager.h` (OS)                       |      | `RFIDManager.h` (OS)                        |
| `SettingsManager.cpp` (OS)                  |      | `PowerManager.cpp` (OS)                     |      | `RFIDManager.cpp` (OS)                      |
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
                        ^                                                  ^                                                      ^
             Configuration Passed                                Configuration Passed                                  Configuration Passed 
    (via API calls from `SystemInitializer`)            (via API calls from `SystemInitializer`)             (via API calls from `SystemInitializer`)
                        |                                                  |                                                      |
                        +--------------------------------------------------+------------------------------------------------------+
                        |                                                  |                                                      |
                        V                                                  V                                                      V
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
| `LanguageManager.h` (OS)                    |      | `TimeManager.h` (OS)                        |      | `BLEManager.h` (OS)                         |
| `LanguageManager.cpp` (OS)                  |      | `TimeManager.cpp` (OS)                      |      | `BLEManager.cpp` (CS)(D)                    | // Core BLE logic is proprietary.
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
                        ^ Communication (e.g., TimeManager asks WifiManager for time sync)                      ^ Communication (e.g., TimeManager asks WifiManager for time sync)
                        |                                            |
                        +---------------------------------------------+---------------------------------------------+
                        |                                            |
                        V                                            V
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
| `WifiManager.h` (OS)                        |      | `AudioManager.h` (OS)                       |      | `SDManager.h` (OS)                          |
| `WifiManager.cpp` (CS)(D)                   |      | `AudioManager.cpp` (OS)                     |      | `SDManager.cpp` (OS)                        |
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
                                                             ^ Communication (e.g., AudioManager accesses SD for sound files)
                                                             | (Calls global callbacks like g_playClickSound)
                                                             +=========================================>
+---------------------------------------------+
| `ClickSoundData.h` (OS)                     |
| `ClickSoundData.cpp` (OS)                   |
+---------------------------------------------+

========================================================================================================================================================================
 [ GENERAL UI FRAMEWORK & ELEMENTS (MIXED SOURCE - Bounded Extensibility Points are OPEN) ]
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 This layer contains the core UI building blocks. Base classes are OS for extensibility.
 Derived element implementations are CS to protect implementation details.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------

+---------------------------------------------+
| **`UIElement.h` (OS)**                      |
| `UIElement.cpp` (OS)                        | // Base class and its implementation are open and extensible.
+---------------------------------------------+
                ^ (Inheritance)
                |
                V
+---------------------------------------------+      +---------------------------------------------+
| `UILayer.h` (OS)                            |      | `StatusbarElement.h` (OS)                   |
| `UILayer.cpp` (CS)                          |      | `StatusbarElement.cpp` (OS)                 | // Base class and its implementation are open.
+---------------------------------------------+      +---------------------------------------------+
                ^                                            ^ (Inheritance)
                |                                            |
                V                                            V
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
| `ScreenManager.h` (OS)                      |      | `TextUI.h` (OS)                             |      | `ButtonUI.h` (OS)                           |
| `ScreenManager.cpp` (CS)(D)                 |      | `TextUI.cpp` (CS)(D)                        |      | `ButtonUI.cpp` (CS)(D)                      |
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
                ^                                            ^                                            ^ (Calls global callbacks like g_playClickSound)
                |                                            |                                            +=========================================>
                V                                            V                                            V
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+
| `StatusbarUI.h` (OS)                        |      | `ToggleButtonUI.h` (OS)                     |      | `ClickableListUI.h` (OS)                    |
| `StatusbarUI.cpp` (CS)(D)                   |      | `ToggleButtonUI.cpp` (CS)(D)                |      | `ClickableListUI.cpp` (CS)(D)               |
+---------------------------------------------+      +---------------------------------------------+      +---------------------------------------------+


/*
 * =============================================================================
 * [ SOURCE CODE LICENSING STATUS SUMMARY ]
 * =============================================================================
 *
 * This summary outlines the licensing status of the project's source code files
 * based on the PROJECT ARCHITECTURE OVERVIEW diagram and the Software Licenses
 * and Attributions section, incorporating your latest clarifications.
 *
 * NOTATIONS:
 *   [OS] - Open Source: Source code is available and may be distributed under
 *          permissive license terms (e.g., MIT, Apache 2.0, 2-Clause BSD, LGPL 2.1).
 *   [CS] - Closed Source: Source code is proprietary and confidential, not for
 *          redistribution. Typically distributed as pre-compiled .a binaries.
 *   (D)  - Demo Mode: This Closed Source (.cpp) file contains demo mode specific logic
 *          within `#ifdef DEMO_MODE` blocks.
 *
 * IMPORTANT NOTES:
 *   - As per the project architecture, ALL `.h` (HEADER) FILES are Open Source.
 *     This ensures public APIs, data structures, and configurations are accessible.
 *   - The `.cpp` implementations of high-level UI controllers and core UI framework
 *     elements are part of the project's core logic.
 *
 * --- [ OPEN SOURCE COMPONENTS (OS) ] ---
 *
 * ALL `.h` FILES:
 *   - All files with a `.h` extension in the project (e.g., Config.h, SystemInitializer.h,
 *     BLEManager.h, TextUI.h, GlobalSystemEvents.h, etc.) are Open Source. These define public APIs,
 *     data structures, and configurations.
 *
 * IMPLEMENTATION (`.cpp`) FILES:
 *   - `WobysGUI.ino` (OS) (Template code for user modification)
 *   - `SystemInitializer.cpp` (OS)(D) (Implementation)
 *   - `SettingsManager.cpp` (OS)
 *   - `PowerManager.cpp` (OS)
 *   - `RFIDManager.cpp` (OS)
 *   - `LanguageManager.cpp` (OS)
 *   - `LanguageAssets.cpp` (OS)
 *   - `TimeManager.cpp` (OS)
 *   - `AudioManager.cpp` (OS)
 *   - `SDManager.cpp` (OS)
 *   - `UIElement.cpp` (OS) (Base class and its implementation are open and extensible)
 *   - `StatusbarElement.cpp` (OS) (Base class and its implementation are open)
 *   - `ClickSoundData.cpp` (OS)
 *   - `MainUI.cpp` (OS)
 *   - `WifiUI.cpp` (OS)
 *   - `SettingsUI.cpp` (OS)
 *   - `BLEUI.cpp` (OS)
 *   - `GlobalSystemEvents.cpp` (OS) (Definitions for global system callbacks)
 *
 * --- [ CLOSED SOURCE COMPONENTS (CS) ] ---
 *
 * IMPLEMENTATION (`.cpp`) FILES:
 *   - `BLEManager.cpp` (CS)(D) (Core BLE logic is proprietary)
 *   - `WifiManager.cpp` (CS)(D) (Core Wifi logic is proprietary)
 *   - `UILayer.cpp` (CS)
 *   - `ScreenManager.cpp`(CS)(D)
 *   - `TextUI.cpp` (CS)(D)
 *   - `ButtonUI.cpp` (CS)(D)
 *   - `ToggleButtonUI.cpp` (CS)(D)
 *   - `ClickableListUI.cpp` (CS)(D)
 *   - `StatusbarUI.cpp` (CS)(D)
 *   - `ToggleSwitchUI.cpp` (CS)(D)
 *   - `SeekbarUI.cpp` (CS)(D)
 *   - `KeyboardUI.cpp` (CS)(D)
 *   - `GridLayoutUI.cpp` (CS)
 *   - `ImageUI.cpp` (CS)(D)
 *   - `MessageBoardElement.cpp` (CS)
 *   - `ScreenSaverManager.cpp` (CS)
 *   - `IconElement.cpp` (CS)
 *   - `ClockLabelUI.cpp` (CS)(D)
 *
 * =============================================================================