/**
 * @file LanguageAssets.h
 * @brief Declares structures and external constants for managing language-specific assets.
 *
 * This header file provides external declarations for the `LanguageAsset` structure
 * and global constant arrays. It defines the structure linking a language enum
 * with its file path and embedded JSON content. These declarations allow other
 * modules to access language asset metadata and content definitions.
 *
 * @version 1.0.8
 * @date 2025-09-10
 * @author György Oberländer
 * @contact gyorgy.oberlander@gmail.com
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
 * This product incorporates software components licensed under various open-source licenses.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */

#ifndef LANGUAGEASSETS_H
#define LANGUAGEASSETS_H

#include "LanguageManager.h" // Required for LanguageManager::Language enum

/**
 * @brief Structure to hold metadata and content for a specific language asset.
 *
 * This structure links a `LanguageManager::Language` enum value to its file path
 * on the filesystem (e.g., LittleFS) and its embedded JSON content string literal.
 */
struct LanguageAsset {
    LanguageManager::Language languageEnum; ///< The `LanguageManager::Language` enum value for this asset.
    const char*               filePath;     ///< The path to the language JSON file on the filesystem (e.g., "/en.json").
    const char*               jsonContent;  ///< The raw JSON content string literal embedded in code.
};

/**
 * @brief External declaration for the default English language JSON content.
 *
 * This constant pointer holds the address of the raw JSON string literal
 * defining English UI strings. Its definition is provided in `LanguageAssets.cpp`.
 */
extern const char* default_en_json;

/**
 * @brief External declaration for the default Hungarian language JSON content.
 *
 * This constant pointer holds the address of the raw JSON string literal
 * defining Hungarian UI strings. Its definition is provided in `LanguageAssets.cpp`.
 */
extern const char* default_hu_json;

/**
 * @brief External declaration for an array of all available language assets.
 *
 * This array provides a central collection of all supported languages,
 * linking their enum, file path, and embedded JSON content.
 * Its definition is provided in `LanguageAssets.cpp`.
 */
extern const LanguageAsset languageAssets[];

/**
 * @brief External declaration for the total number of language assets available.
 *
 * This constant specifies the number of entries in the `languageAssets` array.
 * Its definition is provided in `LanguageAssets.cpp`.
 */
extern const size_t languageAssetCount;

#endif // LANGUAGEASSETS_H