// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#include "audio/sound.hpp"

namespace openhoi {

// Sound constructor
Sound::Sound(std::string fileName, ALuint buffer, short* samples)
    : fileName(fileName), buffer(buffer), samples(samples) {}

// Sound destructor
Sound::~Sound() {
  alDeleteBuffers(1, &buffer);
  delete[] samples;
}

// Gets the sound file name
std::string const& Sound::getFileName() const { return fileName; }

// Gets the OpenAL sound buffer
ALuint const& Sound::getBuffer() const { return buffer; }

}  // namespace openhoi