// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#include "audio/sound.hpp"

#include <cassert>

namespace openhoi {

// Sound constructor
Sound::Sound(std::string fileName, ALuint buffer, short* samples)
    : fileName(fileName), buffer(buffer), samples(samples) {
  // Create audio source object
  alGenSources(1, &source);
  assert(source > 0);

  alSourcei(source, AL_LOOPING, AL_FALSE);
  alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
  alSourcei(source, AL_BUFFER, this->buffer);
  alSourcef(source, AL_MIN_GAIN, 0.0f);
  alSourcef(source, AL_MAX_GAIN, 1.0f);
}

// Sound destructor
Sound::~Sound() {
  alSourcei(source, AL_BUFFER, NULL);
  alDeleteBuffers(1, &buffer);
  delete[] samples;
}

// Gets the sound file name
std::string const& Sound::getFileName() const { return fileName; }

// Gets the OpenAL sound source
ALuint const& Sound::getSource() const { return source; }

}  // namespace openhoi