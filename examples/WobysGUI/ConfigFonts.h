/**
 * @file ConfigFonts.h
 * @brief Font declarations and static instances for all fonts used in the WobysGUI UI.
 *
 * This file centralizes all font-related definitions, including both
 * the external declarations of raw font data arrays and the static
 * instances of LovyanGFX U8g2font wrappers. These fonts are essential
 * for consistent typography throughout the user interface.
 *
 * @version 1.0.6
 * @date 2025-08-21
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
 * This product incorporates software components licensed under various open-source components.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */

#pragma once

#ifndef CONFIG_FONTS_H
#define CONFIG_FONTS_H

// Required for LGFX font types (lgfx::U8g2font) and basic types like uint8_t.
// LovyanGFX.hpp already brings in uint8_t and its own font types.
#include <LovyanGFX.hpp> 

/**
 * @brief External Font Data Array Declarations.
 *
 * Declares the raw font data arrays that are defined in separate .c files (e.g., profont.c, u8g2_fonts.c).
 * These declarations are needed for any code that uses these fonts.
 * They are centralized here for global access by font instances.
 */

// Profont Fonts
extern const uint8_t u8g2_font_profont10_tr[]; ///< ProFont 10 Regular, transparent.
extern const uint8_t u8g2_font_profont11_tr[]; ///< ProFont 11 Regular, transparent.
extern const uint8_t u8g2_font_profont12_tr[]; ///< ProFont 12 Regular, transparent.
extern const uint8_t u8g2_font_profont15_tr[]; ///< ProFont 15 Regular, transparent.
extern const uint8_t u8g2_font_profont17_tr[]; ///< ProFont 17 Regular, transparent.
extern const uint8_t u8g2_font_profont22_tr[]; ///< ProFont 22 Regular, transparent.
extern const uint8_t u8g2_font_profont29_tr[]; ///< ProFont 29 Regular, transparent.

// Courier Bold Fonts
extern const uint8_t u8g2_font_courB08_tr[]; ///< Courier Bold 08, transparent.
extern const uint8_t u8g2_font_courB10_tr[]; ///< Courier Bold 10, transparent.
extern const uint8_t u8g2_font_courB12_tr[]; ///< Courier Bold 12, transparent.
extern const uint8_t u8g2_font_courB14_tr[]; ///< Courier Bold 14, transparent.
extern const uint8_t u8g2_font_courB18_tr[]; ///< Courier Bold 18, transparent.
extern const uint8_t u8g2_font_courB24_tr[]; ///< Courier Bold 24, transparent.

// Courier Regular Fonts
extern const uint8_t u8g2_font_courR08_tr[]; ///< Courier Regular 08, transparent.
extern const uint8_t u8g2_font_courR10_tr[]; ///< Courier Regular 10, transparent.
extern const uint8_t u8g2_font_courR12_tr[]; ///< Courier Regular 12, transparent.
extern const uint8_t u8g2_font_courR14_tr[]; ///< Courier Regular 14, transparent.
extern const uint8_t u8g2_font_courR18_tr[]; ///< Courier Regular 18, transparent.
extern const uint8_t u8g2_font_courR24_tr[]; ///< Courier Regular 24, transparent.

// Helvetica Bold Fonts
extern const uint8_t u8g2_font_helvB08_tr[]; ///< Helvetica Bold 08, transparent.
extern const uint8_t u8g2_font_helvB10_tr[]; ///< Helvetica Bold 10, transparent.
extern const uint8_t u8g2_font_helvB12H_tf[]; ///< Helvetica Bold 12, translucent font (used for UI elements with specific backgrounds).
extern const uint8_t u8g2_font_helvB14H_tf[]; ///< Helvetica Bold 14, translucent font.
extern const uint8_t u8g2_font_helvB18H_tf[]; ///< Helvetica Bold 18, translucent font.
extern const uint8_t u8g2_font_helvB24_tr[]; ///< Helvetica Bold 24, transparent.

// Helvetica Regular Fonts
extern const uint8_t u8g2_font_helvR08_tr[]; ///< Helvetica Regular 08, transparent.
extern const uint8_t u8g2_font_helvR10H_tf[]; ///< Helvetica Regular 10, translucent font.
extern const uint8_t u8g2_font_helvR12_tr[]; ///< Helvetica Regular 12, transparent.
extern const uint8_t u8g2_font_helvR14H_tf[]; ///< Helvetica Regular 14, transparent.
extern const uint8_t u8g2_font_helvR18_tr[]; ///< Helvetica Regular 18, transparent.
extern const uint8_t u8g2_font_helvR24_tr[]; ///< Helvetica Regular 24, transparent.

// New Century Schoolbook Bold Fonts
extern const uint8_t u8g2_font_ncenB08_tr[]; ///< New Century Schoolbook Bold 08, transparent.
extern const uint8_t u8g2_font_ncenB10_tr[]; ///< New Century Schoolbook Bold 10, transparent.
extern const uint8_t u8g2_font_ncenB12_tr[]; ///< New Century Schoolbook Bold 12, transparent.
extern const uint8_t u8g2_font_ncenB14_tr[]; ///< New Century Schoolbook Bold 14, transparent.
extern const uint8_t u8g2_font_ncenB18_tr[]; ///< New Century Schoolbook Bold 18, transparent.
extern const uint8_t u8g2_font_ncenB24_tr[]; ///< New Century Schoolbook Bold 24, transparent.

// New Century Schoolbook Regular Fonts
extern const uint8_t u8g2_font_ncenR08_tr[]; ///< New Century Schoolbook Regular 08, transparent.
extern const uint8_t u8g2_font_ncenR10_tr[]; ///< New Century Schoolbook Regular 10, transparent.
extern const uint8_t u8g2_font_ncenR12_tr[]; ///< New Century Schoolbook Regular 12, transparent.
extern const uint8_t u8g2_font_ncenR14_tr[]; ///< New Century Schoolbook Regular 14, transparent.
extern const uint8_t u8g2_font_ncenR18_tr[]; ///< New Century Schoolbook Regular 18, transparent.
extern const uint8_t u8g2_font_ncenR24_tr[]; ///< New Century Schoolbook Regular 24, transparent.

// Times Bold Fonts
extern const uint8_t u8g2_font_timB08_tr[]; ///< Times Bold 08, transparent.
extern const uint8_t u8g2_font_timB10_tr[]; ///< Times Bold 10, transparent.
extern const uint8_t u8g2_font_timB12_tr[]; ///< Times Bold 12, transparent.
extern const uint8_t u8g2_font_timB14_tr[]; ///< Times Bold 14, transparent.
extern const uint8_t u8g2_font_timB18_tr[]; ///< Times Bold 18, transparent.
extern const uint8_t u8g2_font_timB24_tr[]; ///< Times Bold 24, transparent.

// Times Regular Fonts
extern const uint8_t u8g2_font_timR08_tr[]; ///< Times Regular 08, transparent.
extern const uint8_t u8g2_font_timR10_tr[]; ///< Times Regular 10, transparent.
extern const uint8_t u8g2_font_timR12_tr[]; ///< Times Regular 12, transparent.
extern const uint8_t u8g2_font_timR14_tr[]; ///< Times Regular 14, transparent.
extern const uint8_t u8g2_font_timR18_tr[]; ///< Times Regular 18, transparent.
extern const uint8_t u8g2_font_timR24_tr[]; ///< Times Regular 24, transparent.

// Special Purpose Fonts (e.g., icons, custom glyphs)
extern const uint8_t u8g2_font_battery19[]; ///< Font for battery level icons.
extern const uint8_t u8g2_font_open_iconic_arrow_4x_t[]; ///< Open Iconic Arrow font at 4x scale.
extern const uint8_t u8g2_font_open_iconic_all_2x_t[]; ///< Open Iconic All font at 2x scale.

/**
 * @brief Font Instance Definitions.
 *
 * Creates static instances of LovyanGFX U8g2font wrappers from raw font data.
 * These instances are used throughout the UI for consistent typography.
 */

static const lgfx::U8g2font battery(u8g2_font_battery19);          ///< Static instance for battery level icon font.
static const lgfx::U8g2font arrow4x(u8g2_font_open_iconic_arrow_4x_t); ///< Static instance for arrow icons at 4x scale.
static const lgfx::U8g2font iconic_all2x(u8g2_font_open_iconic_all_2x_t); ///< Static instance for general purpose iconic font.

static const lgfx::U8g2font profont10(u8g2_font_profont10_tr); ///< Static instance for ProFont 10.
static const lgfx::U8g2font profont11(u8g2_font_profont11_tr); ///< Static instance for ProFont 11.
static const lgfx::U8g2font profont12(u8g2_font_profont12_tr); ///< Static instance for ProFont 12.
static const lgfx::U8g2font profont15(u8g2_font_profont15_tr); ///< Static instance for ProFont 15.
static const lgfx::U8g2font profont17(u8g2_font_profont17_tr); ///< Static instance for ProFont 17.
static const lgfx::U8g2font profont22(u8g2_font_profont22_tr); ///< Static instance for ProFont 22.
static const lgfx::U8g2font profont29(u8g2_font_profont29_tr); ///< Static instance for ProFont 29.

static const lgfx::U8g2font courB08(u8g2_font_courB08_tr); ///< Static instance for Courier Bold 08.
static const lgfx::U8g2font courB10(u8g2_font_courB10_tr); ///< Static instance for Courier Bold 10.
static const lgfx::U8g2font courB12(u8g2_font_courB12_tr); ///< Static instance for Courier Bold 12.
static const lgfx::U8g2font courB14(u8g2_font_courB14_tr); ///< Static instance for Courier Bold 14.
static const lgfx::U8g2font courB18(u8g2_font_courB18_tr); ///< Static instance for Courier Bold 18.
static const lgfx::U8g2font courB24(u8g2_font_courB24_tr); ///< Static instance for Courier Bold 24.

static const lgfx::U8g2font courR08(u8g2_font_courR08_tr); ///< Static instance for Courier Regular 08.
static const lgfx::U8g2font courR10(u8g2_font_courR10_tr); ///< Static instance for Courier Regular 10.
static const lgfx::U8g2font courR12(u8g2_font_courR12_tr); ///< Static instance for Courier Regular 12.
static const lgfx::U8g2font courR14(u8g2_font_courR14_tr); ///< Static instance for Courier Regular 14.
static const lgfx::U8g2font courR18(u8g2_font_courR18_tr); ///< Static instance for Courier Regular 18.
static const lgfx::U8g2font courR24(u8g2_font_courR24_tr); ///< Static instance for Courier Regular 24.

static const lgfx::U8g2font helvB08(u8g2_font_helvB08_tr); ///< Static instance for Helvetica Bold 08.
static const lgfx::U8g2font helvB10(u8g2_font_helvB10_tr); ///< Static instance for Helvetica Bold 10.
static const lgfx::U8g2font helvB12(u8g2_font_helvB12H_tf); ///< Static instance for Helvetica Bold 12 (translucent font).
static const lgfx::U8g2font helvB14(u8g2_font_helvB14H_tf); ///< Static instance for Helvetica Bold 14 (translucent font).
static const lgfx::U8g2font helvB18(u8g2_font_helvB18H_tf); ///< Static instance for Helvetica Bold 18 (translucent font).
static const lgfx::U8g2font helvB24(u8g2_font_helvB24_tr); ///< Static instance for Helvetica Bold 24.

static const lgfx::U8g2font helvR08(u8g2_font_helvR08_tr); ///< Static instance for Helvetica Regular 08.
static const lgfx::U8g2font helvR10(u8g2_font_helvR10H_tf); ///< Static instance for Helvetica Regular 10 (translucent font).
static const lgfx::U8g2font helvR12(u8g2_font_helvR12_tr); ///< Static instance for Helvetica Regular 12.
static const lgfx::U8g2font helvR14(u8g2_font_helvR14H_tf); ///< Static instance for Helvetica Regular 14.
static const lgfx::U8g2font helvR18(u8g2_font_helvR18_tr); ///< Static instance for Helvetica Regular 18.
static const lgfx::U8g2font helvR24(u8g2_font_helvR24_tr); ///< Static instance for Helvetica Regular 24.

static const lgfx::U8g2font ncenB08(u8g2_font_ncenB08_tr); ///< Static instance for New Century Schoolbook Bold 08.
static const lgfx::U8g2font ncenB10(u8g2_font_ncenB10_tr); ///< Static instance for New Century Schoolbook Bold 10.
static const lgfx::U8g2font ncenB12(u8g2_font_ncenB12_tr); ///< Static instance for New Century Schoolbook Bold 12.
static const lgfx::U8g2font ncenB14(u8g2_font_ncenB14_tr); ///< Static instance for New Century Schoolbook Bold 14.
static const lgfx::U8g2font ncenB18(u8g2_font_ncenB18_tr); ///< Static instance for New Century Schoolbook Bold 18.
static const lgfx::U8g2font ncenB24(u8g2_font_ncenB24_tr); ///< Static instance for New Century Schoolbook Bold 24.

static const lgfx::U8g2font ncenR08(u8g2_font_ncenR08_tr); ///< Static instance for New Century Schoolbook Regular 08.
static const lgfx::U8g2font ncenR10(u8g2_font_ncenR10_tr); ///< Static instance for New Century Schoolbook Regular 10.
static const lgfx::U8g2font ncenR12(u8g2_font_ncenR12_tr); ///< Static instance for New Century Schoolbook Regular 12.
static const lgfx::U8g2font ncenR14(u8g2_font_ncenR14_tr); ///< Static instance for New Century Schoolbook Regular 14.
static const lgfx::U8g2font ncenR18(u8g2_font_ncenR18_tr); ///< Static instance for New Century Schoolbook Regular 18.
static const lgfx::U8g2font ncenR24(u8g2_font_ncenR24_tr); ///< Static instance for New Century Schoolbook Regular 24.

static const lgfx::U8g2font timB08(u8g2_font_timB08_tr); ///< Static instance for Times Bold 08.
static const lgfx::U8g2font timB10(u8g2_font_timB10_tr); ///< Static instance for Times Bold 10.
static const lgfx::U8g2font timB12(u8g2_font_timB12_tr); ///< Static instance for Times Bold 12.
static const lgfx::U8g2font timB14(u8g2_font_timB14_tr); ///< Static instance for Times Bold 14.
static const lgfx::U8g2font timB18(u8g2_font_timB18_tr); ///< Static instance for Times Bold 18.
static const lgfx::U8g2font timB24(u8g2_font_timB24_tr); ///< Static instance for Times Bold 24.

static const lgfx::U8g2font timR08(u8g2_font_timR08_tr); ///< Static instance for Times Regular 08.
static const lgfx::U8g2font timR10(u8g2_font_timR10_tr); ///< Static instance for Times Regular 10.
static const lgfx::U8g2font timR12(u8g2_font_timR12_tr); ///< Static instance for Times Regular 12.
static const lgfx::U8g2font timR14(u8g2_font_timR14_tr); ///< Static instance for Times Regular 14.
static const lgfx::U8g2font timR18(u8g2_font_timR18_tr); ///< Static instance for Times Regular 18.
static const lgfx::U8g2font timR24(u8g2_font_timR24_tr); ///< Static instance for Times Regular 24.

#endif // CONFIG_FONTS_H