// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <hoibase/openhoi.hpp>

#include <string>

namespace openhoi {

// Window mode enumeration
enum WindowMode { WINDOWED = 0, FULLSCREEN = 1, BORDERLESS = 2 };

// Options class
class Options final {
 public:
  // Options constructor
  Options();

  // Save options to file
  void saveToFile();

  // Gets the video mode name
  std::string const& getVideoMode() const;

  // Sets the video mode name
  void setVideoMode(std::string const& videoMode);

  // Gets the FSAA level
  byte const& getFullScreenAntiAliasing() const;

  // Sets the FSAA level
  void setFullScreenAntiAliasing(byte const& fullScreenAntiAliasing);

  // Gets the window mode
  WindowMode const& getWindowMode() const;

  // Sets the window mode
  void setWindowMode(WindowMode const& windowMode);

  // Gets the vertical sync flag
  bool const& isVerticalSync() const;

  // Sets the vertical sync flag
  void setVerticalSync(bool const& verticalSync);

  // Gets the audio device name
  std::string const& getAudioDevice() const;

  // Sets the audio device name
  void setAudioDevice(std::string const& soundDevice);

  // Gets the music volume
  float const& getMusicVolume() const;

  // Sets the music volume
  void setMusicVolume(float const& musicVolume);

  // Gets the effects volume
  float const& getEffectsVolume() const;

  // Sets the effects volume
  void setEffectsVolume(float const& effectsVolume);

 private:
  // Load options from file
  void loadFromFile();

  std::string videoMode;
  byte fullScreenAntiAliasing;
  WindowMode windowMode;
  bool verticalSync;
  std::string audioDevice;
  float musicVolume;
  float effectsVolume;
};

}  // namespace openhoi