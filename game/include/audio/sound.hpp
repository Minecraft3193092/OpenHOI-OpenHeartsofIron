// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <al.h>

#include <string>

namespace openhoi {

// Represents an loaded sound that can be played at any time
class Sound final {
 public:
  // Sound constructor
  Sound(std::string fileName, ALuint buffer, short* samples);

  // Sound destructor
  ~Sound();

  // Gets the sound file name
  std::string const& getFileName() const;

  // Gets the OpenAL sound buffer
  ALuint const& getBuffer() const;

  private:
  std::string fileName;
  ALuint source;
  ALuint buffer;
  short* samples;
};

}  // namespace openhoi