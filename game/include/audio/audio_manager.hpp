// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "audio_device.hpp"
#include "sound.hpp"

#include <hoibase/file/filesystem.hpp>

#include <al.h>
#include <alc.h>

#include <list>
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

  // Starts the loading of background music
  void loadBackgroundMusicAsync(filesystem::path directory);

  // Loads all audio effect files found in the the provided directory into
  // memory and thus makes them playable
  void loadEffects(filesystem::path directory);

  // Play the provided sound with the given volume
  void playSound(std::shared_ptr<Sound> sound, float volume);

  // Play the sound effect (no background music) identified by it's name with
  // the given volume
  void playSound(std::string sound, float volume);

  // Update audio stats (e.g. progress of background music)
  void updateStats();

 private:
  typedef std::unordered_map<std::string, std::shared_ptr<Sound>> SoundMap;

  // Add audio device to list of possible devices. Returns the newly added
  // device
  std::shared_ptr<AudioDevice> addAudioDevice(std::string name);

  // Create OpenAL context from the current device
  bool createContext();

  // Loads all background audio tracks in a separate track and starts to play
  // them as soon as the first file was fully loaded
  void loadAndPlayBackgroundMusic(filesystem::path directory);

  // Generate OpenAL source from the provided sound and play it with the given
  // volume
  ALuint generateSourceAndPlaySound(std::shared_ptr<Sound> sound, float volume);

  // Loads an sound and returns the sound in case the file was loaded
  // successfully
  std::shared_ptr<Sound> loadSound(filesystem::path audioFile);

  std::vector<std::shared_ptr<AudioDevice>> devices;
  std::shared_ptr<AudioDevice> selectedDevice;
  SoundMap effects;
  std::list<ALuint> playing;
  SoundMap backgroundMusic;
  SoundMap::iterator backgroundMusicIt;
  ALuint playingBackgroundMusic;
  ALCdevice* device;
  ALCcontext* context;
};

}  // namespace openhoi