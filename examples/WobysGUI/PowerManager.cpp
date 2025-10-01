/**
 * @file PowerManager.cpp
 * @brief Implementation of the PowerManager class.
 *
 * This file contains the detailed implementation of the system's power management
 * and battery monitoring logic. It is responsible for monitoring battery voltage,
 * determining the charge level, and controlling safe system shutdown in case of low battery.
 *
 * @version 1.0.7
 * @date 2025-08-15
 * @author György Oberländer
 * @contact support@wobys.com
 *
 * @copyright (c) 2025 György Oberländer. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * IMPORTANT NOTE ON THIRD-PARTY LICENSES:
 * This software utilizes various third-party open-source components.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations
 * (e.g., source code availability for LGPL components) required for compliance.
 */

#include "PowerManager.h"
// Config.h include is already handled via PowerManager.h
#include <Arduino.h>     // For millis(), pinMode, digitalWrite, analogReadMilliVolts, delay
#include "IconElement.h" // For IconElement class definition
// SystemInitializer.h is included here for the full PowerManagerConfig struct definition.
#include "SystemInitializer.h" // For PowerManagerConfig struct definition

/**
 * @brief Constructs a new PowerManager object.
 * Initializes pointers and default values for internal state.
 *
 * @param statusbar A pointer to the `StatusbarUI` instance for displaying messages.
 * @param batteryIconElement A pointer to the `IconElement` responsible for displaying the battery icon.
 */
PowerManager::PowerManager(StatusbarUI* statusbar, IconElement* batteryIconElement)
  : _statusbarPtr(statusbar),
    _batteryIconElement(batteryIconElement),
    _lastBatteryCheckMillis(0),
    _lowBatteryShutdownArmed(false),
    _currentBatteryVoltage(0.0f),
    _currentBatteryLevelIcon('?'), // Initial unknown icon, init() will set the first real value.
    _batteryLevelChangedCallback(nullptr),
    _shutdownWarningCallback(nullptr),
    _performShutdownCallback(nullptr),
    _batteryVoltageUpdateCallback(nullptr),
    _battAdcPin(0), _powerCtrlPin(0), _r1ValueOhm(0.0f), _r2ValueOhm(0.0f),
    _batteryCheckIntervalMs(0), _lowThresholdPowerOffVolts(0.0f), _hysteresisVolts(0.0f),
    _battIconLevel6('?'), _battIconLevel5('?'), _battIconLevel4('?'), _battIconLevel3('?'),
    _battIconLevel2('?'), _battIconLevel1('?'), _battIconLevel0('?'), _battIconLevelUnknown('?'),
    _battVoltageLevel6(0.0f), _battVoltageLevel5(0.0f), _battVoltageLevel4(0.0f), _battVoltageLevel3(0.0f),
    _battVoltageLevel2(0.0f), _battVoltageLevel1(0.0f)
{
  DEBUG_INFO_PRINTLN("PowerManager: Constructor called.");
}

/**
 * @brief Initializes the PowerManager with hardware-specific configurations.
 * This method must be called after constructing the object.
 * It sets up ADC and power control pins, voltage divider parameters,
 * and battery level thresholds based on the provided configuration.
 *
 * @param config A constant reference to the `PowerManagerConfig` struct
 *               containing all necessary hardware configuration parameters.
 */
void PowerManager::init(const PowerManagerConfig& config) {
  DEBUG_INFO_PRINTLN("PowerManager: init() starting (based on ConfigStruct)...");

  // Set member variables from the config struct
  _battAdcPin = config.battAdcPin;
  _powerCtrlPin = config.powerCtrlPin;
  _r1ValueOhm = config.r1ValueOhm;
  _r2ValueOhm = config.r2ValueOhm;
  _batteryCheckIntervalMs = config.batteryCheckIntervalMs;
  _lowThresholdPowerOffVolts = config.lowThresholdPowerOffVolts;
  _hysteresisVolts = config.hysteresisVolts;

  _battIconLevel6 = config.battIconLevel6;
  _battIconLevel5 = config.battIconLevel5;
  _battIconLevel4 = config.battIconLevel4;
  _battIconLevel3 = config.battIconLevel3;
  _battIconLevel2 = config.battIconLevel2;
  _battIconLevel1 = config.battIconLevel1;
  _battIconLevel0 = config.battIconLevel0;
  _battIconLevelUnknown = config.battIconLevelUnknown;
  _battVoltageLevel6 = config.battVoltageLevel6;
  _battVoltageLevel5 = config.battVoltageLevel5;
  _battVoltageLevel4 = config.battVoltageLevel4;
  _battVoltageLevel3 = config.battVoltageLevel3;
  _battVoltageLevel2 = config.battVoltageLevel2;
  _battVoltageLevel1 = config.battVoltageLevel1;

  pinMode(_powerCtrlPin, OUTPUT);
  digitalWrite(_powerCtrlPin, HIGH);
  DEBUG_INFO_PRINTF("PowerManager: POWER_CTRL_PIN (%d) set to HIGH.\n", _powerCtrlPin);

  pinMode(_battAdcPin, INPUT);
  DEBUG_INFO_PRINTF("PowerManager: BATT_ADC_PIN (%d) set to INPUT.\n", _battAdcPin);

  // First battery check and UI update
  _currentBatteryVoltage = readBatteryVoltage();
  _currentBatteryLevelIcon = determineBatteryLevelIcon(_currentBatteryVoltage);
  if (_batteryIconElement) { // Null pointer check
    _batteryIconElement->setIcon(_currentBatteryLevelIcon);
  }
  if (_batteryLevelChangedCallback) { // Null pointer check
    _batteryLevelChangedCallback(_currentBatteryLevelIcon);
  }
  _lastBatteryCheckMillis = millis();
  DEBUG_INFO_PRINTF(
    "PowerManager: Initial battery voltage: %.2fV, Icon: '%c'\n",
    _currentBatteryVoltage, _currentBatteryLevelIcon);
  DEBUG_INFO_PRINTLN("PowerManager: init() completed.");
}

/**
 * @brief Enables the system power relay, allowing the user to release the power button.
 * This method should be called by the `SystemInitializer` at the beginning of its `init()`
 * function to maintain power after the device has booted.
 * @param powerCtrlPin The GPIO pin number controlling the power relay.
 */
void PowerManager::enablePowerRelay(int powerCtrlPin) {
  pinMode(powerCtrlPin, OUTPUT);
  digitalWrite(powerCtrlPin, HIGH);
  DEBUG_INFO_PRINTF("PowerManager: enablePowerRelay(): POWER_CTRL_PIN (%d) set to HIGH, push button can be released.\n", powerCtrlPin);
}

/**
 * @brief Sets the `IconElement` instance for UI updates related to the battery.
 * This method is public so `SystemInitializer` can set it dynamically.
 * @param element A pointer to the `IconElement` instance to be used for battery display.
 */
void PowerManager::setBatteryIconElement(IconElement* element) {
    _batteryIconElement = element;
    if (element) {
        // Immediately update the icon if an element is provided and we have current data
        element->setIcon(_currentBatteryLevelIcon);
        DEBUG_INFO_PRINTLN("PowerManager: Battery icon element set and updated with current icon.");
    } else {
        DEBUG_WARN_PRINTLN("PowerManager: Attempted to set BatteryIconElement to nullptr.");
    }
}

/**
 * @brief Main loop function for PowerManager.
 * Should be called repeatedly in the Arduino loop(). Monitors battery and manages power.
 */
void PowerManager::loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - _lastBatteryCheckMillis >= _batteryCheckIntervalMs) {
    _lastBatteryCheckMillis = currentMillis;
    checkBatteryStatus();
  }
}

/**
 * @brief Reads the raw battery voltage from the ADC pin and converts it to Volts.
 * This method takes multiple readings and averages them for accuracy.
 * @return The calculated battery voltage in Volts.
 */
float PowerManager::readBatteryVoltage() {
  int numReadings = 10;
  long sum_mV = 0;
  for (int i = 0; i < numReadings; i++) {
    sum_mV += analogReadMilliVolts(_battAdcPin);
    delay(1); // Small delay between readings for stability
  }
  float adcVoltage_mV = (float)sum_mV / numReadings;
  float adcVoltage = adcVoltage_mV / 1000.0f; // Convert mV to V

  // Voltage divider calculation: V_batt = V_adc * ((R1 + R2) / R2)
  const float voltageDividerRatio = (_r1ValueOhm + _r2ValueOhm) / _r2ValueOhm;
  float batteryVoltage = adcVoltage * voltageDividerRatio;
  return batteryVoltage;
}

/**
 * @brief Determines the appropriate battery icon character based on the provided voltage.
 * This method uses the configured voltage thresholds to map voltage to an icon.
 * @param voltage The current battery voltage.
 * @return The character icon representing the battery level.
 */
char PowerManager::determineBatteryLevelIcon(float voltage) {
  // Now uses member variables set by init() from the config struct
  if (voltage >= _battVoltageLevel6) return _battIconLevel6;
  else if (voltage >= _battVoltageLevel5) return _battIconLevel5;
  else if (voltage >= _battVoltageLevel4) return _battIconLevel4;
  else if (voltage >= _battVoltageLevel3) return _battIconLevel3;
  else if (voltage >= _battVoltageLevel2) return _battIconLevel2;
  else if (voltage >= _battVoltageLevel1) return _battIconLevel1;
  else if (voltage < _battVoltageLevel1) { // If below the lowest threshold
    return _battIconLevel0;
  }
  return _battIconLevelUnknown; // Fallback for unexpected values
}

/**
 * @brief Checks the current battery status and triggers warnings or shutdown if necessary.
 * This is the core logic for battery monitoring, invoked periodically by `loop()`.
 */
void PowerManager::checkBatteryStatus() {
  _currentBatteryVoltage = readBatteryVoltage();
  char newLevelIcon = determineBatteryLevelIcon(_currentBatteryVoltage);

  if (_batteryVoltageUpdateCallback) { // Null pointer check
    _batteryVoltageUpdateCallback(_currentBatteryVoltage);
  }

  // Check if battery level icon has changed
  if (_currentBatteryLevelIcon != newLevelIcon) {
    _currentBatteryLevelIcon = newLevelIcon;
    DEBUG_INFO_PRINTF(
      "PowerManager: Battery level changed! New voltage: %.2fV, New icon: '%c'\n",
      _currentBatteryVoltage, _currentBatteryLevelIcon);

    if (_batteryIconElement) { // Null pointer check
      _batteryIconElement->setIcon(_currentBatteryLevelIcon);
    }
    if (_batteryLevelChangedCallback) { // Null pointer check
      _batteryLevelChangedCallback(_currentBatteryLevelIcon);
    }
  }

  // Low battery shutdown logic
  // Also check _currentBatteryVoltage > 0.5f to prevent triggering on 0V readings (e.g., disconnected battery)
  if (_currentBatteryVoltage < _lowThresholdPowerOffVolts &&
      _currentBatteryVoltage > 0.5f) {
    if (!_lowBatteryShutdownArmed) {
      DEBUG_WARN_PRINTLN(
        "PowerManager: Battery voltage low, shutdown armed.");
      _lowBatteryShutdownArmed = true;
      if (_shutdownWarningCallback) { // Null pointer check
        _shutdownWarningCallback("POWER_WARN_LOW_BATT");
      }
    }
  } else if (_currentBatteryVoltage >
             (_lowThresholdPowerOffVolts + _hysteresisVolts)) {
    // If voltage recovers above threshold + hysteresis
    if (_lowBatteryShutdownArmed) {
      DEBUG_INFO_PRINTLN(
        "PowerManager: Battery voltage recovered, shutdown disarmed.");
      _lowBatteryShutdownArmed = false;
    }
  }
}

/**
 * @brief Requests a graceful system power-off.
 *
 * This method triggers the shutdown sequence, including calling registered
 * shutdown warning and perform shutdown callbacks, before initiating the
 * actual power-off procedure.
 */
void PowerManager::requestSystemPowerOff() {
  DEBUG_INFO_PRINTLN("PowerManager: System power-off requested.");

  if (_shutdownWarningCallback) { // Null pointer check
    _shutdownWarningCallback("POWER_INFO_SHUTDOWN_INIT");
  }

  if (_performShutdownCallback) { // Null pointer check
    DEBUG_INFO_PRINTLN("PowerManager: Calling _performShutdownCallback...");
    _performShutdownCallback();
    DEBUG_INFO_PRINTLN("PowerManager: _performShutdownCallback RETURNED. Now calling performActualPowerOff().");
    performActualPowerOff();
  } else {
    DEBUG_WARN_PRINTLN("PowerManager: No _performShutdownCallback registered, directly calling performActualPowerOff().");
    performActualPowerOff();
  }
}

/**
 * @brief Executes the actual system power-off sequence.
 * This method sets the power control pin to LOW, effectively cutting power to the system.
 */
void PowerManager::performActualPowerOff() {
  DEBUG_INFO_PRINTLN("PowerManager: Performing actual power-off...");
  DEBUG_INFO_PRINTLN("PowerManager: Setting POWER_CTRL_PIN to LOW.");
  digitalWrite(_powerCtrlPin, LOW);

  // Small delay to ensure the command is sent before power is cut
  unsigned long powerOffStartTime = millis();
  while (millis() - powerOffStartTime < 300) {
    delay(10);
  }

  DEBUG_INFO_PRINTLN("PowerManager: System shut down. Goodbye!");
  // Infinite loop as the system should be off, but some minimal CPU activity might still occur
  // before the actual power cut by the relay/MOSFET.
  while (true) {
    delay(1000);
  }
}

/**
 * @brief Retrieves the current battery voltage.
 * @return The current battery voltage in Volts.
 */
float PowerManager::getCurrentVoltage() const {
  return _currentBatteryVoltage;
}

/**
 * @brief Retrieves the character representing the current battery level icon.
 * @return The character icon corresponding to the current battery level.
 */
char PowerManager::getCurrentBatteryLevelIcon() const {
  return _currentBatteryLevelIcon;
}

/**
 * @brief Sets the callback function to be invoked when the battery level icon changes.
 *
 * @param callback The `std::function` to register, which takes a `char` (new level icon) as an argument.
 */
void PowerManager::setOnBatteryLevelChangedCallback(
  std::function<void(char newLevelIcon)> callback) {
  _batteryLevelChangedCallback = callback;
  DEBUG_INFO_PRINTLN("PowerManager: OnBatteryLevelChanged callback registered.");
}

/**
 * @brief Sets the callback function to be invoked when the raw battery voltage is updated.
 *
 * @param callback The `std::function` to register, which takes a `float` (new voltage) as an argument.
 */
void PowerManager::setOnBatteryVoltageUpdateCallback(std::function<void(float newVoltage)> callback) {
  _batteryVoltageUpdateCallback = callback;
  DEBUG_INFO_PRINTLN("PowerManager: OnBatteryVoltageUpdate callback registered.");
}

/**
 * @brief Sets the callback function to be invoked for shutdown warnings (e.g., low battery).
 *
 * @param callback The `std::function` to register, which takes a `const std::string&` (message key) as an argument.
 */
void PowerManager::setOnShutdownWarningCallback(
  std::function<void(const std::string& messageKey)> callback) {
  _shutdownWarningCallback = callback;
  DEBUG_INFO_PRINTLN("PowerManager: OnShutdownWarning callback registered.");
}

/**
 * @brief Sets the callback function to be invoked just before the actual system power-off.
 *
 * This allows for final cleanup or saving operations before power is cut.
 * @param callback The `std::function<void()>` to register.
 */
void PowerManager::setOnPerformShutdownCallback(std::function<void()> callback) {
    _performShutdownCallback = callback;
    DEBUG_INFO_PRINTLN("PowerManager: OnPerformShutdown callback registered.");
}