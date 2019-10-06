// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "options.hpp"

#include <algorithm>

namespace openhoi {

// Options constructor
Options::Options()
    : videoMode(""),
      fullScreenAntiAliasing(8),
      windowMode(WindowMode::BORDERLESS),
      verticalSync(false),
      audioDevice(""),
      musicVolume(0.35f),
      effectsVolume(1.0f) {}

// Gets the video mode name
std::string const& Options::getVideoMode() const { return videoMode; }

// Sets the video mode name
void Options::setVideoMode(std::string const& videoMode) {
  this->videoMode = videoMode;
}

// Gets the FSAA level
byte const& Options::getFullScreenAntiAliasing() const {
  return this->fullScreenAntiAliasing;
}

// Sets the FSAA flag
void Options::setFullScreenAntiAliasing(byte const& fullScreenAntiAliasing) {
  this->fullScreenAntiAliasing = fullScreenAntiAliasing;
}

// Gets the window mode
WindowMode const& Options::getWindowMode() const { return windowMode; }

// Sets the window mode
void Options::setWindowMode(WindowMode const& windowMode) {
  this->windowMode = windowMode;
}

// Gets the vertical sync flag
bool const& Options::isVerticalSync() const { return verticalSync; }

// Sets the vertical sync flag
void Options::setVerticalSync(bool const& verticalSync) {
  this->verticalSync = verticalSync;
}

// Gets the audio device name
std::string const& Options::getAudioDevice() const { return audioDevice; }

// Sets the audio device name
void Options::setAudioDevice(std::string const& audioDevice) {
  this->audioDevice = audioDevice;
}

// Gets the music volume
float const& Options::getMusicVolume() const { return musicVolume; }

// Sets the music volume
void Options::setMusicVolume(float const& musicVolume) {
  this->musicVolume = musicVolume > 1.0f ? 1.0f : std::max(musicVolume, 0.0f);
}

// Gets the effects volume
float const& Options::getEffectsVolume() const { return effectsVolume; }

// Sets the effects volume
void Options::setEffectsVolume(float const& effectsVolume) {
  this->effectsVolume =
      effectsVolume > 1.0f ? 1.0f : std::max(effectsVolume, 0.0f);
}

}  // namespace openhoi