// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "audio_device.hpp"
#include "sound.hpp"

#include <hoibase/file/filesystem.hpp>

#include <al.h>
#include <alc.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace openhoi {

// Audio manager for openhoi
class AudioManager final {
 public:
  // Initializes the audio manager
  AudioManager();

  // Destroys the audio manager
  ~AudioManager();

  // Sets the current device
  void setDevice(std::shared_ptr<AudioDevice> device);

  // Gets the current device
  std::shared_ptr<AudioDevice> const& getDevice() const;

  // Gets all possible devices
  std::vector<std::shared_ptr<AudioDevice>> const& getPossibleDevices() const;

  // Loads all background audio tracks in a separate track and starts to play
  // them as soon as the first file was fully loaded (async)
  void loadBackgroundMusicAsync(filesystem::path directory);

  // Loads all audio effect files found in the the provided directory into
  // memory and thus makes them playable
  void loadEffects(filesystem::path directory);

 private:
  // Add audio device to list of possible devices. Returns the newly added
  // device
  std::shared_ptr<AudioDevice> addAudioDevice(std::string name);

  // Create OpenAL context from the current device
  bool createContext();

 
  // Loads all background audio tracks in a separate track and starts to play
  // them as soon as the first file was fully loaded (blocking method that
  // should not be called from outside)
  void loadBackgroundMusic(filesystem::path directory);

  // Loads an sound and adds it to the provided structure. Returns true in case
  // the sound was loaded successfully
  bool loadSound(filesystem::path audioFile, bool isBackgroundMusic);

  typedef std::unordered_map<std::string, std::shared_ptr<Sound>> SoundMap;

  std::vector<std::shared_ptr<AudioDevice>> devices;
  std::shared_ptr<AudioDevice> selectedDevice;
  SoundMap effects;
  SoundMap backgroundMusic;
  ALCdevice* device = 0;
  ALCcontext* context = 0;
};

}  // namespace openhoi