// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#include "audio/audio_device.hpp"

namespace openhoi {

// Initializes the audio device
AudioDevice::AudioDevice(std::string name) { this->name = name; }

// Gets the device name
std::string const& AudioDevice::getName() const { return name; }

// Gets the friendly device name
std::string const& AudioDevice::getFriendlyName() const {
  // Remove the possible "OpenAL Soft on " prefix
  std::string prefix("OpenAL Soft on ");
  size_t pos = name.find(prefix);
  if (pos != std::string::npos) {
    std::string friendly = name.substr(pos + prefix.length());
    return friendly;
  }
  return name;
}

}  // namespace openhoi