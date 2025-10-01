/**
 * @file AudioManager.h
 * @brief This file defines the AudioManager class, responsible for handling audio output
 * functionalities within the WobysGUI firmware. It provides an interface
 * for playing sounds and streams, managing volume, and enabling/disabling audio.
 * The implementation uses direct ESP32 I2S/DAC driver usage to ensure licensing flexibility,
 * avoiding copyleft effects on the overall framework. Advanced decoding (e.g., MP3) is left
 * for user implementation or permissively licensed third-party libraries.
 *
 * @version 1.0.4
 * @date 2025-09-16
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
 * This product incorporates software components licensed under various open-source licenses.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */
#pragma once
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Arduino.h>
#include <string>
#include <functional>
#include <atomic>
#include <FS.h>
#include <LittleFS.h> 

#include "ClickSoundData.h"
#include <driver/i2s_std.h>
#include <driver/i2s_common.h>
#include <driver/gpio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "Config.h"
#include "SettingsManager.h"

class IconElement;
struct AudioManagerConfig;

/**
 * @brief Manages audio output functionalities, including playing sounds/streams,
 * volume control, and enabling/disabling audio.
 *
 * This class is designed to provide licensing flexibility by using direct ESP32 I2S/DAC
 * drivers (new ESP-IDF API) for basic PCM playback (e.g., WAV). It uses a dedicated
 * FreeRTOS task to handle audio processing, and buffers rapid sound requests to prevent
 * hardware conflicts.
 */
class AudioManager {
public:
    /**
     * @brief Callback type for when audio playback finishes.
     * @param info A string providing context about the finished playback (e.g., file path or URL).
     */
    using PlaybackFinishedCallback = std::function<void(const std::string& info)>;
    /**
     * @brief Callback type for when an audio playback error occurs.
     * @param info A string providing context about the playback attempt (e.g., file path or URL).
     * @param errorMsg A string describing the nature of the error.
     */
    using PlaybackErrorCallback = std::function<void(const std::string& info, const std::string& errorMsg)>;

    PlaybackFinishedCallback _onPlaybackFinishedCallback; /**< Callback for playback finished event. */
    PlaybackErrorCallback _onPlaybackErrorCallback; /**< Callback for playback error event. */

    /**
     * @brief Constructor for the AudioManager class.
     * @param settingsManager Pointer to the SettingsManager instance for retrieving audio settings.
     */
    AudioManager(SettingsManager* settingsManager);

    /**
     * @brief Destructor for the AudioManager class.
     * Ensures I2S driver is uninstalled and resources are released.
     */
    ~AudioManager();

    /**
     * @brief Initializes the AudioManager with hardware-specific configurations.
     * This method must be called after constructing the object.
     * @param config A struct containing all necessary hardware configuration parameters for audio.
     * @return True if initialization is successful, false otherwise.
     */
    bool init(const AudioManagerConfig& config);

    /**
     * @brief Main loop function for AudioManager.
     * This function is now empty as all audio processing is handled by the dedicated FreeRTOS task.
     */
    void loop();

    /**
     * @brief Sets the pointer to the SD filesystem for playing audio files from SD card.
     * @param sdFsPtr Pointer to the `fs::FS` object representing the SD card.
     */
    void setSdFilesystem(fs::FS* sdFsPtr);

    /**
     * @brief Requests playback of an audio file from the SD card. This will interrupt any
     * currently playing sound and clear any pending click sound requests.
     * Only WAV files with compatible PCM format are supported by default.
     * @param filePath The full path to the audio file on the SD card.
     */
    void playFile(const std::string& filePath);

    /**
     * @brief Requests playback of a predefined click sound from LittleFS.
     * Multiple rapid requests will be buffered and played sequentially.
     */
    void playClickSound();

    /**
     * @brief Requests playback of an audio stream from a given URL.
     * This functionality is not implemented by default due to licensing complexities.
     * @param url The URL of the audio stream. Requires an active Wi-Fi connection.
     */
    void playStream(const std::string& url);

    /**
     * @brief Stops any currently active audio playback and clears the click sound buffer.
     */
    void stop();

    /**
     * @brief Sets the audio output volume.
     * @param volume_0_100 The desired volume level, ranging from 0 (mute) to 100 (max).
     */
    void setVolume(int volume_0_100);

    /**
     * @brief Retrieves the current audio output volume.
     * @return The current volume level, ranging from 0 to 100.
     */
    int getVolume() const;

    /**
     * @brief Enables or disables the overall audio functionality.
     * When disabled, no sounds or streams will be played.
     * @param enabled True to enable audio, false to disable.
     */
    void setEnabled(bool enabled);

    /**
     * @brief Checks if audio functionality is currently enabled.
     * @return True if audio is enabled, false otherwise.
     */
    bool isEnabled() const;

    /**
     * @brief Sets the callback function to be called when audio playback finishes.
     * @param cb The callback function.
     */
    void setOnPlaybackFinishedCallback(PlaybackFinishedCallback cb) { _onPlaybackFinishedCallback = cb; }

    /**
     * @brief Sets the callback function to be called when an audio playback error occurs.
     * @param cb The callback function.
     */
    void setOnPlaybackErrorCallback(PlaybackErrorCallback cb) { _onPlaybackErrorCallback = cb; }

    /**
     * @brief Sets the IconElement instance used to display the current speaker/volume status.
     * The AudioManager will update this icon (its character) based on its internal state.
     * @param icon Pointer to the IconElement instance for the speaker icon.
     */
    void setSpeakerIconElement(IconElement* icon);

private:
    /**
     * @brief Enumeration for task notification events.
     * Used by the main thread to signal the playback task.
     */
    enum TaskNotification : uint32_t {
        NOTIFY_PLAY      = 1 << 0, /**< Notification to start or continue playback. */
        NOTIFY_STOP      = 1 << 1, /**< Notification to stop current playback. */
        NOTIFY_TERMINATE = 1 << 2, /**< Notification for the task to terminate. */
    };

    const AudioManagerConfig* _config;
    SettingsManager* _settingsManager;
    fs::FS* _sdFsPtr = nullptr;
    IconElement* _speakerIconElement = nullptr;
    
    i2s_chan_handle_t _tx_channel_handle = nullptr;
    int _bclkPin, _lrckPin, _doutPin;
    int _sampleRate;
    int _channels;
    
    std::atomic<int> _currentVolume_0_100;
    std::atomic<bool> _isEnabled;
    std::atomic<bool> _isInitializedAndReady;

    std::string _fileToPlay;
    bool _playFileFromLittleFS;
    
    static const int WAV_BUFFER_SIZE = 2048;
    uint8_t _wavBuffer[WAV_BUFFER_SIZE];
    float _currentSoftwareGain = 1.0f;

    TaskHandle_t _playbackTaskHandle = nullptr;
    SemaphoreHandle_t _playbackMutex = nullptr;
    SemaphoreHandle_t _taskDoneSignal = nullptr;

    std::atomic<int> _pendingClickSounds;

    /** @brief Atomic flag indicating if the I2S channel is currently enabled (data flow active). */
    std::atomic<bool> _isChannelCurrentlyActive = false;

    /** @brief Timestamp of the last audio activity (playback or silence send) in the task. */
    unsigned long _lastActivityTime = 0;

    /** @brief Timeout in milliseconds after which the I2S channel is disabled if no activity. */
    const unsigned long _idleTimeoutMs = 1000; // Example: Keep channel active for 1000ms after last sound.

    bool _initI2S();
    void _deinitI2S();
    void _setI2SGain(float gain);
    
    void _createPlaybackTask();
    void _destroyPlaybackTask();
    static void playbackTask(void* pvParameters);
    
    float _mapVolumeToInternalScale(int volume_0_100) const;
    bool _ensureClickSoundFileExists();
    const char* _getVolumeIconString() const;
};

#endif // AUDIO_MANAGER_H