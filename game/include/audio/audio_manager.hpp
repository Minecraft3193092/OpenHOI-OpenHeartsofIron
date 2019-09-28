// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "audio_device.hpp"

#include <al.h>
#include <alc.h>

#include <memory>
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

 private:
  // Add audio device to list of possible devices. Returns the newly added
  // device
  std::shared_ptr<AudioDevice> addAudioDevice(std::string name);

  // Create OpenAL context from the current device
  bool createContext();

  std::vector<std::shared_ptr<AudioDevice>> devices;
  std::shared_ptr<AudioDevice> selectedDevice;
  ALCdevice* device;
  ALCcontext* context;
};

}  // namespace openhoi