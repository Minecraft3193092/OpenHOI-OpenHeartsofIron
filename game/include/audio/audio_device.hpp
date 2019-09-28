// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <string>

namespace openhoi {

// Audio device which is just a wrapper of OpenAL's audio device name string for convinence reasons
class AudioDevice final {
 public:
  // Creates the audio device
  AudioDevice(std::string name);

  // Gets the device name
  std::string const& getName() const;

  // Gets the friendly device name
  std::string const& getFriendlyName() const;

 private:
  std::string name;
};

}  // namespace openhoi