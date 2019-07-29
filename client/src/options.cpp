// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "options.hpp"

namespace openhoi {

// Options constructor
Options::Options()
    : renderingDevice(""),
      videoMode("1024 x  768"),
      fullScreenAntiAliasing(8),
      windowMode(WindowMode::WINDOWED),
      verticalSync(true) {}

// Gets the rendering device name
std::string const& Options::GetRenderingDevice() const {
  return renderingDevice;
}

// Sets the rendering device name
void Options::SetRenderingDevice(std::string const& renderingDevice) {
  this->renderingDevice = renderingDevice;
}

// Gets the video mode name
std::string const& Options::GetVideoMode() const { return videoMode; }

// Sets the video mode name
void Options::SetVideoMode(std::string const& videoMode) {
  this->videoMode = videoMode;
}

// Gets the FSAA level
byte const& Options::GetFullScreenAntiAliasing() const {
  return this->fullScreenAntiAliasing;
}

// Sets the FSAA flag
void Options::SetFullScreenAntiAliasing(byte const& fullScreenAntiAliasing) {
  this->fullScreenAntiAliasing = fullScreenAntiAliasing;
}

// Gets the window mode
WindowMode const& Options::GetWindowMode() const { return windowMode; }

// Sets the window mode
void Options::SetWindowMode(WindowMode const& windowMode) {
  this->windowMode = windowMode;
}

// Gets the vertical sync flag
bool const& Options::IsVerticalSync() const { return verticalSync; }

// Sets the vertical sync flag
void Options::SetVerticalSync(bool const& verticalSync) {
  this->verticalSync = verticalSync;
}

}  // namespace openhoi