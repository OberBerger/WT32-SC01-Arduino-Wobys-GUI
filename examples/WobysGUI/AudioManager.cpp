/**
 * @file AudioManager.cpp
 * @brief This file implements the AudioManager class, responsible for handling audio output
 * functionalities within the WobysGUI firmware. It provides an interface
 * for playing sounds and streams, managing volume, and enabling/disabling audio.
 * The implementation uses direct ESP32 I2S/DAC driver usage to ensure licensing flexibility,
 * avoiding copyleft effects on the overall framework. Advanced decoding (e.g., MP3) is left
 * for user implementation or permissively licensed third-party libraries.
 *
 * @version 1.0.5
 * @date 2025-09-17
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
 */
#include "AudioManager.h"
#include "ClickSoundData.h"
#include <LittleFS.h>
#include <WiFi.h>
#include "SystemInitializer.h"
#include "IconElement.h"

/**
 * @brief Structure for the RIFF chunk header in a WAV file.
 */
struct RIFFHeader {
  char chunkID[4];    /**< "RIFF" */
  uint32_t chunkSize; /**< Total file size - 8 */
  char format[4];     /**< "WAVE" */
};

/**
 * @brief Structure for the "fmt " subchunk in a WAV file.
 */
struct FMTChunk {
  char subchunk1ID[4];    /**< "fmt " */
  uint32_t subchunk1Size; /**< 16 for PCM */
  uint16_t audioFormat;   /**< 1 for PCM */
  uint16_t numChannels;   /**< Number of audio channels */
  uint32_t sampleRate;    /**< Sample rate in Hz */
  uint32_t byteRate;      /**< Byte rate: sampleRate * numChannels * bitsPerSample / 8 */
  uint16_t blockAlign;    /**< Block align: numChannels * bitsPerSample / 8 */
  uint16_t bitsPerSample; /**< Bits per sample */
};

/**
 * @brief Structure for the "data" subchunk header in a WAV file.
 */
struct DataChunkHeader {
  char subchunk2ID[4];    /**< "data" */
  uint32_t subchunk2Size; /**< Size of the actual audio data */
};


/**
 * @brief Constructs an AudioManager object.
 * @param settingsManager Pointer to the SettingsManager instance for retrieving audio settings.
 */
AudioManager::AudioManager(SettingsManager* settingsManager)
  : _settingsManager(settingsManager),
    _sdFsPtr(nullptr),
    _config(nullptr),
    _speakerIconElement(nullptr),
    _tx_channel_handle(nullptr),
    _bclkPin(0), _lrckPin(0), _doutPin(0),
    _sampleRate(0), _channels(0),
    _fileToPlay(""),
    _playFileFromLittleFS(false),
    _currentSoftwareGain(1.0f),
    _playbackTaskHandle(nullptr),
    _pendingClickSounds(0),
    _isChannelCurrentlyActive(false),
    _lastActivityTime(0) {
  _currentVolume_0_100 = 0;
  _isEnabled = false;
  _isInitializedAndReady = false;

  _playbackMutex = xSemaphoreCreateMutex();
  _taskDoneSignal = xSemaphoreCreateBinary();

  if (!_playbackMutex || !_taskDoneSignal) {
    DEBUG_ERROR_PRINTLN("AudioManager: ERROR - Failed to create semaphores!");
  }
  DEBUG_INFO_PRINTLN("AudioManager: Constructor executed.");
}

/**
 * @brief Destroys the AudioManager object.
 * Ensures graceful shutdown of the audio task and I2S driver.
 */
AudioManager::~AudioManager() {
  DEBUG_INFO_PRINTLN("AudioManager: Destructor started.");
  setEnabled(false);
  if (_playbackMutex) vSemaphoreDelete(_playbackMutex);
  if (_taskDoneSignal) vSemaphoreDelete(_taskDoneSignal);
  DEBUG_INFO_PRINTLN("AudioManager: Destructor finished.");
}

/**
 * @brief Initializes the AudioManager with hardware-specific configurations.
 * This method must be called after constructing the object.
 * @param config A struct containing all necessary hardware configuration parameters for audio.
 * @return True if initialization is successful, false otherwise.
 */
bool AudioManager::init(const AudioManagerConfig& config) {
  DEBUG_INFO_PRINTLN("AudioManager: init() starting...");
  _config = &config;
  _bclkPin = _config->bclkPin;
  _lrckPin = _config->lrckPin;
  _doutPin = _config->doutPin;
  _sampleRate = _config->sampleRate;
  _channels = _config->channels;

  if (!_ensureClickSoundFileExists()) {
    DEBUG_WARN_PRINTLN("AudioManager: Warning - Click sound file creation failed.");
  }

  setVolume(_settingsManager->getSoundVolume(AUDIO_DEFAULT_VOLUME_PERCENT));
  setEnabled(_settingsManager->isSoundEnabled(true));

  _isInitializedAndReady = true;
  DEBUG_INFO_PRINTLN("AudioManager: init() successfully completed.");
  return true;
}

/**
 * @brief Main loop function for AudioManager.
 * This function is now empty as all audio processing is handled by the dedicated FreeRTOS task.
 */
void AudioManager::loop() {
  // The main loop is no longer needed for audio processing.
}

/**
 * @brief Internal helper to initialize the ESP32 I2S driver.
 * @return True if I2S driver installation is successful, false otherwise.
 */
bool AudioManager::_initI2S() {
  if (_tx_channel_handle) {
    DEBUG_INFO_PRINTLN("AudioManager: I2S already initialized.");
    return true;
  }
  DEBUG_INFO_PRINTLN("AudioManager: Initializing I2S driver...");

  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
  esp_err_t err = i2s_new_channel(&chan_cfg, &_tx_channel_handle, NULL);
  if (err != ESP_OK) {
    DEBUG_ERROR_PRINTF("AudioManager: ERROR - Failed to create I2S channel (Error: %d)!\n", err);
    _tx_channel_handle = nullptr;
    return false;
  }

  i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(_sampleRate),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, (_channels == 1 ? I2S_SLOT_MODE_MONO : I2S_SLOT_MODE_STEREO)),
    .gpio_cfg = {
      .mclk = I2S_GPIO_UNUSED,
      .bclk = (gpio_num_t)_bclkPin,
      .ws = (gpio_num_t)_lrckPin,
      .dout = (gpio_num_t)_doutPin,
      .din = I2S_GPIO_UNUSED,
      .invert_flags = { .mclk_inv = false, .bclk_inv = false, .ws_inv = false },
    },
  };

  err = i2s_channel_init_std_mode(_tx_channel_handle, &std_cfg);
  if (err != ESP_OK) {
    DEBUG_ERROR_PRINTF("AudioManager: ERROR - Failed to init I2S STD mode (Error: %d)!\n", err);
    if (_tx_channel_handle) {
      i2s_del_channel(_tx_channel_handle);
      _tx_channel_handle = nullptr;
    }
    return false;
  }

  DEBUG_INFO_PRINTLN("AudioManager: I2S initialized successfully.");
  return true;
}

/**
 * @brief Internal helper to de-initialize the ESP32 I2S driver.
 */
void AudioManager::_deinitI2S() {
  if (!_tx_channel_handle) {
    return;
  }
  DEBUG_INFO_PRINTLN("AudioManager: De-initializing I2S driver (deleting channel)...");
  esp_err_t err = i2s_del_channel(_tx_channel_handle);
  if (err != ESP_OK) {
    DEBUG_WARN_PRINTF("AudioManager: Warning - Failed to delete I2S channel (Error: %d)\n", err);
  }
  _tx_channel_handle = nullptr;
  DEBUG_INFO_PRINTLN("AudioManager: I2S channel de-initialized.");
}

/**
 * @brief Creates the FreeRTOS playback task.
 */
void AudioManager::_createPlaybackTask() {
  if (_playbackTaskHandle) {
    return;
  }
  DEBUG_INFO_PRINTLN("AudioManager: Creating playback task...");
  xTaskCreatePinnedToCore(
    playbackTask,
    "AudioPlaybackTask",
    8192,
    this,
    tskIDLE_PRIORITY + 5,
    &_playbackTaskHandle,
    APP_CPU_NUM);
}

/**
 * @brief Destroys the FreeRTOS playback task gracefully.
 */
void AudioManager::_destroyPlaybackTask() {
  if (!_playbackTaskHandle) {
    return;
  }
  DEBUG_INFO_PRINTLN("AudioManager: Requesting playback task termination...");
  xTaskNotify(_playbackTaskHandle, TaskNotification::NOTIFY_TERMINATE, eSetBits);

  if (xSemaphoreTake(_taskDoneSignal, pdMS_TO_TICKS(2000)) == pdTRUE) {
    DEBUG_INFO_PRINTLN("AudioManager: Playback task terminated gracefully.");
  } else {
    DEBUG_WARN_PRINTLN("AudioManager: Timeout waiting for task termination. Forcibly deleting.");
    vTaskDelete(_playbackTaskHandle);
  }
  _playbackTaskHandle = nullptr;
}

/**
 * @brief Internal FreeRTOS task entry point for audio playback.
 * This static function contains the main loop for processing playback requests from a queue.
 * @param pvParameters A pointer to the AudioManager instance (passed as 'this').
 */
void AudioManager::playbackTask(void* pvParameters) {
  AudioManager* self = static_cast<AudioManager*>(pvParameters);
  DEBUG_INFO_PRINTLN("AudioPlaybackTask: Started.");

  if (!self->_initI2S()) {
    DEBUG_ERROR_PRINTLN("AudioPlaybackTask: Failed to initialize I2S. Terminating.");
    xSemaphoreGive(self->_taskDoneSignal);
    vTaskDelete(NULL);
    return;
  }
  // Signal to the main thread that _initI2S() was successful.
  xSemaphoreGive(self->_taskDoneSignal);
  DEBUG_INFO_PRINTLN("AudioPlaybackTask: I2S init signal given to main thread.");


  uint32_t notificationValue = 0;
  bool terminate = false;
  // localChannelEnabled flag for internal task management. This is the ONLY place
  // controlling i2s_channel_enable/disable to minimize hardware state transitions.
  bool localChannelEnabled = false;

  // Create a buffer for silence samples (if needed, to avoid gaps)
  int16_t silenceBuffer[WAV_BUFFER_SIZE / sizeof(int16_t)];
  memset(silenceBuffer, 0, WAV_BUFFER_SIZE);

  while (!terminate) {
    // Determine wait time. Always stay responsive, as the channel might be enabled.
    TickType_t waitTime = pdMS_TO_TICKS(5);

    // Wait for notifications.
    xTaskNotifyWait(0, ULONG_MAX, &notificationValue, waitTime);

    // Handle TERMINATE notification.
    if (notificationValue & TaskNotification::NOTIFY_TERMINATE) {
      terminate = true;
      // Disable I2S channel before task termination if it was enabled.
      if (localChannelEnabled) {
        esp_err_t err = i2s_channel_disable(self->_tx_channel_handle);
        if (err == ESP_OK) {
          localChannelEnabled = false;
          self->_isChannelCurrentlyActive.store(false);
          DEBUG_INFO_PRINTLN("AudioPlaybackTask: I2S channel DISABLED on TERMINATE notification.");
        } else {
          DEBUG_WARN_PRINTF("AudioPlaybackTask: WARNING - Failed to disable I2S channel on TERMINATE (Error: %d).\n", err);
        }
      }
    }

    // Handle STOP notification.
    if (notificationValue & TaskNotification::NOTIFY_STOP) {
      // Queues already cleared by AudioManager::stop().
    }
    notificationValue = 0;  // Clear processed notifications.

    if (terminate) continue;  // If terminating, skip further processing and exit loop.

    // Re-check for pending work after notifications, as a new PLAY request might have arrived.
    xSemaphoreTake(self->_playbackMutex, portMAX_DELAY);
    bool hasPendingWork = !self->_fileToPlay.empty() || self->_pendingClickSounds > 0;
    xSemaphoreGive(self->_playbackMutex);

    if (hasPendingWork) {
      // Ensure I2S channel is enabled if we are about to play a sound.
      if (!localChannelEnabled) {
        esp_err_t err = i2s_channel_enable(self->_tx_channel_handle);
        if (err == ESP_OK) {
          localChannelEnabled = true;
          self->_isChannelCurrentlyActive.store(true);  // Update atomic flag for AudioManager.
          DEBUG_INFO_PRINTLN("AudioPlaybackTask: I2S channel ENABLED for playback.");
        } else {
          DEBUG_ERROR_PRINTF("AudioPlaybackTask: ERROR - Failed to enable I2S channel (Error: %d). Skipping playback cycle.\n", err);
          vTaskDelay(pdMS_TO_TICKS(100));  // Delay on error to prevent fast looping.
          continue;
        }
      }
      self->_lastActivityTime = millis();  // Mark activity timestamp.

      // Process all pending sounds in a loop.
      while (true) {
        std::string sourceToPlay = "";
        bool playFromLittleFS = false;

        // Atomically check for work: file playback has priority over click sounds.
        xSemaphoreTake(self->_playbackMutex, portMAX_DELAY);
        if (!self->_fileToPlay.empty()) {
          sourceToPlay = self->_fileToPlay;
          playFromLittleFS = self->_playFileFromLittleFS;
          self->_fileToPlay = "";
        } else if (self->_pendingClickSounds > 0) {
          self->_pendingClickSounds--;
          sourceToPlay = CLICK_SOUND_FILENAME;
          playFromLittleFS = true;
        }
        xSemaphoreGive(self->_playbackMutex);

        if (sourceToPlay.empty()) {
          break;  // No more work in the queue, exit inner loop.
        }

        fs::FS* fs = playFromLittleFS ? &LittleFS : self->_sdFsPtr;
        File audioFile;
        if (fs) {
          audioFile = fs->open(sourceToPlay.c_str(), "r");
        }

        if (!audioFile) {
          DEBUG_ERROR_PRINTF("AudioPlaybackTask: Failed to open %s.\n", sourceToPlay.c_str());
          if (self->_onPlaybackErrorCallback) self->_onPlaybackErrorCallback(sourceToPlay, "Failed to open file in task.");
          continue;
        }

        // Parse and validate WAV header.
        RIFFHeader riffHeader;
        FMTChunk fmtChunk;
        DataChunkHeader dataHeader;
        if (audioFile.read((uint8_t*)&riffHeader, sizeof(RIFFHeader)) != sizeof(RIFFHeader) || strncmp(riffHeader.chunkID, "RIFF", 4) != 0 || strncmp(riffHeader.format, "WAVE", 4) != 0) {
          DEBUG_ERROR_PRINTF("AudioPlaybackTask: Invalid RIFF/WAVE header for '%s'.\n", sourceToPlay.c_str());
          if (self->_onPlaybackErrorCallback) self->_onPlaybackErrorCallback(sourceToPlay, "Invalid WAV header in task.");
          audioFile.close();
          continue;
        }
        if (audioFile.read((uint8_t*)&fmtChunk, sizeof(FMTChunk)) != sizeof(FMTChunk) || strncmp(fmtChunk.subchunk1ID, "fmt ", 4) != 0 || fmtChunk.audioFormat != 1 || fmtChunk.bitsPerSample != 16 || fmtChunk.numChannels != self->_channels || fmtChunk.sampleRate != self->_sampleRate) {
          DEBUG_ERROR_PRINTF("AudioPlaybackTask: WAV format mismatch for '%s'. Got Ch:%u, SR:%u, Bits:%u, Expected Ch:%u, SR:%u, Bits:16\n", sourceToPlay.c_str(), fmtChunk.numChannels, fmtChunk.sampleRate, fmtChunk.bitsPerSample, self->_channels, self->_sampleRate);
          if (self->_onPlaybackErrorCallback) self->_onPlaybackErrorCallback(sourceToPlay, "Unsupported WAV format in task.");
          audioFile.close();
          continue;
        }
        bool dataChunkFound = false;
        while (audioFile.available()) {
          if (audioFile.read((uint8_t*)&dataHeader, sizeof(DataChunkHeader)) != sizeof(DataChunkHeader)) break;
          if (strncmp(dataHeader.subchunk2ID, "data", 4) == 0) {
            dataChunkFound = true;
            break;
          }
          audioFile.seek(dataHeader.subchunk2Size, SeekCur);
        }
        if (!dataChunkFound) {
          if (self->_onPlaybackErrorCallback) self->_onPlaybackErrorCallback(sourceToPlay, "'data' chunk not found in WAV file.");
          audioFile.close();
          continue;
        }

        // Read audio data, apply software gain, and write to I2S.
        while (audioFile.available()) {
          size_t bytesRead = audioFile.read(self->_wavBuffer, WAV_BUFFER_SIZE);
          if (bytesRead == 0) break;

          int16_t* samples = (int16_t*)self->_wavBuffer;
          size_t numSamples = bytesRead / sizeof(int16_t);
          float currentGain = self->_currentSoftwareGain;
          for (size_t i = 0; i < numSamples; ++i) {
            long scaled_sample = static_cast<long>(samples[i] * currentGain);
            samples[i] = static_cast<int16_t>(constrain(scaled_sample, -32768L, 32767L));
          }

          size_t bytesWritten = 0;
          i2s_channel_write(self->_tx_channel_handle, self->_wavBuffer, bytesRead, &bytesWritten, portMAX_DELAY);

          // Check for termination/stop request during active playback.
          if (xTaskNotifyWait(0, 0, &notificationValue, 0) == pdPASS) {
            if (notificationValue & (TaskNotification::NOTIFY_STOP | TaskNotification::NOTIFY_TERMINATE)) {
              break;
            }
          }
        }

        audioFile.close();
        if (self->_onPlaybackFinishedCallback) self->_onPlaybackFinishedCallback(sourceToPlay);
        self->_lastActivityTime = millis();  // Mark activity after each sound.
      }                                      // End of inner work loop (processes all pending sounds).

    } else {  // No pending work found (hasPendingWork is false).
      // If channel is currently enabled, send silence to keep I2S "warm".
      if (localChannelEnabled) {
        size_t bytesWritten = 0;
        // Timeout 0 ensures non-blocking write. If DMA is busy, it won't block.
        i2s_channel_write(self->_tx_channel_handle, (const void*)silenceBuffer, WAV_BUFFER_SIZE, &bytesWritten, 0);
        vTaskDelay(pdMS_TO_TICKS(1));        // Briefly yield CPU.
        self->_lastActivityTime = millis();  // Mark silence as activity.
      }
    }

    // --- Channel Disabling Logic ---
    // This is the key change: only disable if idle timeout is reached AND no pending work.
    xSemaphoreTake(self->_playbackMutex, portMAX_DELAY);
    bool noPendingWorkFinally = self->_fileToPlay.empty() && self->_pendingClickSounds == 0;
    xSemaphoreGive(self->_playbackMutex);

    if (localChannelEnabled && noPendingWorkFinally && (millis() - self->_lastActivityTime > self->_idleTimeoutMs)) {
      esp_err_t err = i2s_channel_disable(self->_tx_channel_handle);
      if (err == ESP_OK) {
        localChannelEnabled = false;
        self->_isChannelCurrentlyActive.store(false);  // Update atomic flag.
        DEBUG_INFO_PRINTF("AudioPlaybackTask: I2S channel DISABLED (idle for %lu ms).\n", self->_idleTimeoutMs);
      } else {
        DEBUG_WARN_PRINTF("AudioPlaybackTask: WARNING - Failed to disable I2S channel (Error: %d).\n", err);
      }
    }
  }  // End of main task loop (while !terminate).

  DEBUG_INFO_PRINTLN("AudioPlaybackTask: Terminating...");
  // De-initialize I2S hardware. This calls i2s_del_channel().
  self->_deinitI2S();
  xSemaphoreGive(self->_taskDoneSignal);  // Signal task completion to the main thread.
  vTaskDelete(NULL);                      // Delete the task.
}

/**
 * @brief Sets the pointer to the SD filesystem for playing audio files from SD card.
 * @param sdFsPtr Pointer to the `fs::FS` object representing the SD card.
 */
void AudioManager::setSdFilesystem(fs::FS* sdFsPtr) {
  xSemaphoreTake(_playbackMutex, portMAX_DELAY);
  _sdFsPtr = sdFsPtr;
  xSemaphoreGive(_playbackMutex);
  DEBUG_INFO_PRINTLN("AudioManager: SD filesystem pointer set.");
}

/**
 * @brief Requests playback of an audio file from the SD card.
 * @param filePath The full path to the audio file on the SD card.
 */
void AudioManager::playFile(const std::string& filePath) {
  if (!_isInitializedAndReady || !_isEnabled || !_playbackTaskHandle) {
    if (_onPlaybackErrorCallback) _onPlaybackErrorCallback(filePath, "Audio manager not ready or disabled.");
    return;
  }

  // Protect shared variables with a mutex.
  xSemaphoreTake(_playbackMutex, portMAX_DELAY);
  _pendingClickSounds = 0;  // Clear any pending click sounds, as file playback has priority.
  _fileToPlay = filePath;
  _playFileFromLittleFS = false;
  xSemaphoreGive(_playbackMutex);

  // Notify the playback task to start processing the new file.
  xTaskNotify(_playbackTaskHandle, TaskNotification::NOTIFY_PLAY, eSetBits);
  DEBUG_INFO_PRINTF("AudioManager: Queued file for playback: %s\n", filePath.c_str());
}

/**
 * @brief Requests playback of a predefined click sound from LittleFS.
 * Multiple rapid requests will be buffered and played sequentially.
 */
void AudioManager::playClickSound() {
  if (!_isInitializedAndReady || !_isEnabled || !_playbackTaskHandle) {
    return;
  }
  if (_settingsManager && !_settingsManager->isClickSoundEnabled()) {
    return;  // Click sound is disabled in settings.
  }

  _pendingClickSounds++;  // Increment the counter for pending click sounds.
  // Notify the playback task to process the new click sound request.
  xTaskNotify(_playbackTaskHandle, TaskNotification::NOTIFY_PLAY, eSetBits);
}

/**
 * @brief Requests playback of an audio stream from a given URL.
 * This functionality is not implemented by default due to licensing complexities.
 * @param url The URL of the audio stream.
 */
void AudioManager::playStream(const std::string& url) {
  DEBUG_WARN_PRINTLN("AudioManager: playStream() is not implemented.");
  if (_onPlaybackErrorCallback) {
    _onPlaybackErrorCallback(url, "Stream playback not implemented.");
  }
}

/**
 * @brief Stops any currently active audio playback.
 */
void AudioManager::stop() {
  if (_playbackTaskHandle) {
    // Protect shared variables with a mutex.
    xSemaphoreTake(_playbackMutex, portMAX_DELAY);
    _pendingClickSounds = 0;  // Clear all pending click sounds.
    _fileToPlay = "";         // Clear any pending file playback request.
    xSemaphoreGive(_playbackMutex);
    // Notify the playback task to stop any current playback.
    xTaskNotify(_playbackTaskHandle, TaskNotification::NOTIFY_STOP, eSetBits);
  }
}

/**
 * @brief Sets the audio output volume.
 * @param volume_0_100 The desired volume level, ranging from 0 (mute) to 100 (max).
 */
void AudioManager::setVolume(int volume_0_100) {
  volume_0_100 = constrain(volume_0_100, 0, 100);
  _currentVolume_0_100 = volume_0_100;

  xSemaphoreTake(_playbackMutex, portMAX_DELAY);
  _currentSoftwareGain = _mapVolumeToInternalScale(_currentVolume_0_100);
  xSemaphoreGive(_playbackMutex);

  if (_speakerIconElement && isEnabled()) {
    _speakerIconElement->setIcon(_getVolumeIconString());
  }
  DEBUG_INFO_PRINTF("AudioManager: Volume set to %d (Gain: %.2f)\n", volume_0_100, _currentSoftwareGain);
}

/**
 * @brief Retrieves the current audio output volume.
 * @return The current volume level, ranging from 0 to 100.
 */
int AudioManager::getVolume() const {
  return _currentVolume_0_100.load();
}

/**
 * @brief Enables or disables the overall audio functionality.
 * @param enabled True to enable audio, false to disable.
 */
void AudioManager::setEnabled(bool enabled) {
  if (_isEnabled == enabled) {
    return;
  }

  _isEnabled = enabled;

  if (enabled) {
    DEBUG_INFO_PRINTLN("AudioManager: Enabling audio...");
    // Reset _lastActivityTime to ensure the idle timeout doesn't immediately trigger a disable.
    _lastActivityTime = millis();

    // Clear any pending signal from previous init failures.
    if (_taskDoneSignal) xSemaphoreTake(_taskDoneSignal, 0);

    // Create task (which calls _initI2S to setup driver).
    _createPlaybackTask();

    // Wait for the task to signal that I2S initialization within the task is complete.
    if (xSemaphoreTake(_taskDoneSignal, pdMS_TO_TICKS(1000)) == pdTRUE) {
      // After successful _initI2S() in task, _tx_channel_handle should be valid.
      // The task itself manages i2s_channel_enable/disable based on activity.
      if (_tx_channel_handle) {
        // Set _isChannelCurrentlyActive to true, reflecting that the channel is ready to be used by the task.
        // The task will perform the actual i2s_channel_enable when it starts sending data/silence.
        _isChannelCurrentlyActive.store(true);
        DEBUG_INFO_PRINTLN("AudioManager: I2S driver init successful in task.");
      } else {
        DEBUG_ERROR_PRINTLN("AudioManager: ERROR - _tx_channel_handle NULL after task init signal. I2S init failed in task.");
        _isEnabled = false;  // Disable AudioManager if I2S init failed.
      }
    } else {
      DEBUG_ERROR_PRINTLN("AudioManager: ERROR - Timeout waiting for playback task I2S init signal. Disabling audio.");
      _isEnabled = false;
      _destroyPlaybackTask();
    }

    if (_speakerIconElement && isEnabled()) {
      _speakerIconElement->setIcon(_getVolumeIconString());
    }
  } else {  // Disabling audio
    DEBUG_INFO_PRINTLN("AudioManager: Disabling audio...");
    // _destroyPlaybackTask() will send TERMINATE signal to task.
    // The task will then handle disabling I2S channel and de-initializing I2S driver.
    _destroyPlaybackTask();

    _isChannelCurrentlyActive.store(false);  // Global flag reset after task termination.
    _lastActivityTime = 0;                   // Reset last activity time.

    if (_speakerIconElement) {
      _speakerIconElement->setIcon(AUDIO_ICON_DISABLED);
    }
  }
}

/**
 * @brief Checks if audio functionality is currently enabled.
 * @return True if audio is enabled, false otherwise.
 */
bool AudioManager::isEnabled() const {
  return _isEnabled.load();
}

/**
 * @brief Internal helper method to scale the UI volume (0-100) to an internal float scale (0.0-1.0).
 * @param volume_0_100 The UI volume level (0-100).
 * @return The scaled volume as a float between 0.0 and 1.0.
 */
float AudioManager::_mapVolumeToInternalScale(int volume_0_100) const {
  return static_cast<float>(volume_0_100) / 100.0f;
}

/**
 * @brief Internal helper method to ensure the click sound WAV file exists on LittleFS.
 * @return True if the click sound file exists or was successfully created, false otherwise.
 */
bool AudioManager::_ensureClickSoundFileExists() {
  if (LittleFS.exists(CLICK_SOUND_FILENAME)) {
    File f = LittleFS.open(CLICK_SOUND_FILENAME, "r");
    if (f && f.size() == CLICK_SOUND_DATA_SIZE) {
      f.close();
      return true;
    }
    if (f) f.close();
  }

  DEBUG_INFO_PRINTLN("AudioManager: Writing click sound file to LittleFS...");
  File clickFile = LittleFS.open(CLICK_SOUND_FILENAME, "w");
  if (!clickFile) {
    DEBUG_ERROR_PRINTLN("AudioManager: ERROR - Failed to open click sound file for writing.");
    return false;
  }
  size_t written = clickFile.write(click_sound_raw_data, CLICK_SOUND_DATA_SIZE);
  clickFile.close();
  return written == CLICK_SOUND_DATA_SIZE;
}

/**
 * @brief Internal helper method to determine the appropriate volume icon string based on current volume level.
 * @return A C-style string representing the volume icon.
 */
const char* AudioManager::_getVolumeIconString() const {
  if (!isEnabled() || getVolume() == 0) {
    return AUDIO_ICON_DISABLED;
  } else if (getVolume() <= 33) {
    return AUDIO_ICON_VOLUME_LOW;
  } else if (getVolume() <= 66) {
    return AUDIO_ICON_VOLUME_MID;
  } else {
    return AUDIO_ICON_VOLUME_HIGH;
  }
}

/**
 * @brief Sets the IconElement instance used to display the current speaker/volume status.
 * @param icon Pointer to the IconElement instance for the speaker icon.
 */
void AudioManager::setSpeakerIconElement(IconElement* icon) {
  _speakerIconElement = icon;
  if (_speakerIconElement) {
    _speakerIconElement->setIcon(_getVolumeIconString());
  }
}