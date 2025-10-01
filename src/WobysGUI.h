#ifndef WOBYS_H
#define WOBYS_H

// --- PROJECT-WIDE CONFIGURATION HEADERS ---
// These define constants, pin assignments, UI styles, and fonts.
#include "Config.h"             // Main configuration file (includes all other Config*.h)
#include "ConfigHardwareUser.h" // User-configurable hardware pins & params
#include "ConfigLGFXUser.h"     // User-configurable LovyanGFX display/touch
#include "ConfigUIUser.h"       // User-configurable UI styles, colors, etc.
#include "ConfigFonts.h"        // Font declarations & static instances
#include "ConfigAudioUser.h"    // Audio output configuration
#include "LanguageAssets.h"     // UI text strings for localization

// --- COMMON DATA STRUCTURES ---
#include "ListItem.h"           // Data structures for lists (WiFi, BLE, generic)

// --- SYSTEM MANAGER APIs (ALL ARE OPEN SOURCE HEADERS) ---
#include "SettingsManager.h"    // Manages persistent application settings
#include "PowerManager.h"       // Manages battery & system power
#include "RFIDManager.h"        // Manages RFID reader operations
#include "LanguageManager.h"    // Manages multi-language support
#include "TimeManager.h"        // Manages system time & NTP sync
#include "BLEManager.h"         // Manages Bluetooth Low Energy connectivity (API)
#include "WifiManager.h"        // Manages Wi-Fi connectivity (API)
#include "AudioManager.h"       // Manages audio playback
#include "SDManager.h"          // Manages SD card operations
#include "ClickSoundData.h"     // Defines raw audio data for click sound

// --- BASE UI FRAMEWORK ELEMENTS (ALL ARE OPEN SOURCE HEADERS FOR API) ---
#include "UIElement.h"          // Base class for all UI elements
#include "UILayer.h"            // Manages collections of UI elements (as API)
#include "StatusbarElement.h"   // Base class for status bar elements
#include "ScreenManager.h"      // Manages UI layers and screen transitions (as API)

// --- GENERIC UI ELEMENTS (ALL ARE OPEN SOURCE HEADERS FOR API) ---
// (Note: While some .cpp implementations for these might be Closed Source,
// the .h files defining their public API are Open Source as per your architecture)
#include "ButtonUI.h"           // Basic interactive button
#include "TextUI.h"             // Basic text display
#include "ToggleButtonUI.h"     // Two-state toggle button
#include "ClickableListUI.h"    // Scrollable, clickable list
#include "KeyboardUI.h"         // On-screen virtual keyboard (as API)
#include "GridLayoutUI.h"       // Layout manager for grids (as API)
#include "ImageUI.h"            // Displays images (as API)
#include "MessageBoardElement.h"// Dynamic message display for status bar (as API)
#include "IconElement.h"        // Displays single icons in status bar (as API)
#include "ClockLabelUI.h"       // Specialized text element for time display (as API)
#include "SeekbarUI.h"          // Interactive slider functionality (as API)
#include "ToggleSwitchUI.h"     // Interactive switch component (as API)
#include "TimeElement.h"        // Specialized status bar element for time display
#include "StatusbarUI.h"        // Manages top status bar & panel (as API)

// --- HIGH-LEVEL UI CONTROLLERS (ALL ARE OPEN SOURCE HEADERS) ---
#include "BLEUI.h"              // Bluetooth UI screen controller
#include "WifiUI.h"             // Wi-Fi UI screen controller
#include "MainUI.h"             // Main application UI screen controller
#include "SettingsUI.h"         // Settings UI screen controller

// --- EXTERNAL FONT WRAPPERS (if not already handled by ConfigFonts.h) ---
// If ConfigFonts.h directly includes these, you don't need them here.
// Assuming ConfigFonts.h does the job for font data declarations.
// #include "profont.h"
// #include "adobex11font.h" // Assuming this is also a font wrapper if used

#endif // WOBYS_H