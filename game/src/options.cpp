// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "options.hpp"

#include <hoibase/file/file_access.hpp>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <algorithm>
#include <fstream>

#define OPENHOI_CONFIG_FILE_NAME "options.json"

#define OPTION_KEY_VIDEO_MODE videoMode
#define OPTION_KEY_FSAA fsaa
#define OPTION_KEY_WINDOW_MODE windowMode
#define OPTION_KEY_VSYNC verticalSync
#define OPTION_KEY_AUDIO_DEVICE audioDevice
#define OPTION_KEY_MUSIC_VOLUME musicVolume
#define OPTION_KEY_EFFECTS_VOLUME effectsVolume

namespace openhoi {

// Options constructor
Options::Options()
    : videoMode(""),
      fullScreenAntiAliasing(8),
      windowMode(WindowMode::BORDERLESS),
      verticalSync(false),
      audioDevice(""),
      musicVolume(35),
      effectsVolume(70) {
  // Load options from file, overwriting the defaults set before
  loadFromFile();
}

// Load options from file
void Options::loadFromFile() {
  // Open input stream
  std::ifstream ifs(FileAccess::getUserGameConfigDirectory() /
                    filesystem::path(OPENHOI_CONFIG_FILE_NAME));
  rapidjson::IStreamWrapper isw(ifs);

  // Create document and parse JSON
  rapidjson::Document doc;
  doc.ParseStream(isw);

  // Check if document is empty
  if (!doc.IsObject()) return;

  // Get members (=options) from document
  if (doc[TOSTRING(OPTION_KEY_VIDEO_MODE)].IsString())
    videoMode = doc[TOSTRING(OPTION_KEY_VIDEO_MODE)].GetString();
  if (doc[TOSTRING(OPTION_KEY_FSAA)].IsInt())
    fullScreenAntiAliasing = (byte)doc[TOSTRING(OPTION_KEY_FSAA)].GetInt();
  if (doc[TOSTRING(OPTION_KEY_WINDOW_MODE)].IsInt())
    windowMode =
        static_cast<WindowMode>(doc[TOSTRING(OPTION_KEY_WINDOW_MODE)].GetInt());
  if (doc[TOSTRING(OPTION_KEY_VSYNC)].IsBool())
    verticalSync = doc[TOSTRING(OPTION_KEY_VSYNC)].GetBool();
  if (doc[TOSTRING(OPTION_KEY_AUDIO_DEVICE)].IsString())
    audioDevice = doc[TOSTRING(OPTION_KEY_AUDIO_DEVICE)].GetString();
  if (doc[TOSTRING(OPTION_KEY_MUSIC_VOLUME)].IsFloat())
    musicVolume = doc[TOSTRING(OPTION_KEY_MUSIC_VOLUME)].GetInt();
  if (doc[TOSTRING(OPTION_KEY_EFFECTS_VOLUME)].IsFloat())
    effectsVolume = doc[TOSTRING(OPTION_KEY_EFFECTS_VOLUME)].GetInt();
}

// Save options to file
void Options::saveToFile() {
  // Create document
  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  // Add members (=options) to document
  doc.AddMember(TOSTRING(OPTION_KEY_VIDEO_MODE), videoMode, allocator);
  doc.AddMember(TOSTRING(OPTION_KEY_FSAA), fullScreenAntiAliasing, allocator);
  doc.AddMember(TOSTRING(OPTION_KEY_WINDOW_MODE), windowMode, allocator);
  doc.AddMember(TOSTRING(OPTION_KEY_VSYNC), verticalSync, allocator);
  doc.AddMember(TOSTRING(OPTION_KEY_AUDIO_DEVICE), audioDevice, allocator);
  doc.AddMember(TOSTRING(OPTION_KEY_MUSIC_VOLUME), musicVolume, allocator);
  doc.AddMember(TOSTRING(OPTION_KEY_EFFECTS_VOLUME), effectsVolume, allocator);

  // Open output stream
  std::ofstream ofs(FileAccess::getUserGameConfigDirectory() /
                    filesystem::path(OPENHOI_CONFIG_FILE_NAME));
  rapidjson::OStreamWrapper osw(ofs);
  rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);

  // Write document to file system
  doc.Accept(writer);
}

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
float const& Options::getMusicVolume() const {
  return (float)(musicVolume / 100.0f);
}

// Sets the music volume
void Options::setMusicVolume(float const& musicVolume) {
  this->musicVolume =
      musicVolume > 1.0f ? 100 : (int)(std::max(musicVolume, 0.0f) * 100);
}

// Gets the effects volume
float const& Options::getEffectsVolume() const {
  return (float)(effectsVolume / 100.0f);
}

// Sets the effects volume
void Options::setEffectsVolume(float const& effectsVolume) {
  this->effectsVolume =
      effectsVolume > 1.0f ? 100 : (int)(std::max(effectsVolume, 0.0f) * 100);
}

}  // namespace openhoi