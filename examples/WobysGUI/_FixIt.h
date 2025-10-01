// This file describes a patch to address a compatibility issue with LovyanGFX 1.2.7
// when used with the ESP32 Arduino Core 3.3.0 (or newer ESP-IDF versions).

// Problem Description:
// The `gpio_hal_iomux_func_sel` function, used for configuring GPIO multiplexing,
// has undergone changes in its underlying implementation or signature in ESP-IDF v5 and later.
// Using the older function call with ESP32 Arduino Core 3.3.0 (which might integrate
// with newer ESP-IDF versions) can lead to compilation errors or incorrect GPIO behavior.

// Solution Description:
// Introduce a conditional compilation block (`#if ESP_IDF_VERSION_MAJOR >= 5`)
// to dynamically select the correct GPIO function setting API based on the detected
// ESP-IDF version. Additionally, ensure that the necessary header `<hal/gpio_types.h>`
// is included for required type definitions.

// Affected LovyanGFX Version: 1.2.7
// Affected Arduino Platform: esp32 by Espressif Systems 3.3.0

// Target File for Patch:
// ..\Arduino\libraries\LovyanGFX\src\lgfx\v1\platforms\esp32s3\Bus_RGB.cpp

// -----------------------------------------------------------------------------
// PATCH INSTRUCTIONS
// -----------------------------------------------------------------------------

// STEP 1: Add the following include statement at the beginning of the target file.
//         (Typically after existing #include directives, before any class definitions)
// -----------------------------------------------------------------------------
// >>> INSERT THIS LINE:
#include <hal/gpio_types.h>
// <<< END OF INSERTION
// -----------------------------------------------------------------------------


// STEP 2: Locate the incorrect line (approximately line 98) in the target file
//         and replace it with the provided conditional code block.
// -----------------------------------------------------------------------------
// >>> LOCATE THIS INCORRECT LINE:
// gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[pin], PIN_FUNC_GPIO);

// >>> REPLACE IT WITH THIS CORRECT CODE BLOCK:
#if ESP_IDF_VERSION_MAJOR >= 5
  gpio_ll_func_sel(GPIO_LL_GET_HW(pin), pin, PIN_FUNC_GPIO);
#else
  gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[pin], PIN_FUNC_GPIO);
#endif
// <<< END OF REPLACEMENT
// -----------------------------------------------------------------------------