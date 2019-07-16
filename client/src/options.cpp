// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "options.hpp"

namespace openhoi {

// Options constructor
Options::Options()
    : renderingDevice(""),
      videoMode("1024 x  768"),
      fullScreenAntiAliasing(8),
      windowMode(WindowMode::WINDOWED),
      verticalSync(true),
#if defined(OPENHOI_OS_WINDOWS)
      renderingSystem(RenderingSystem::DIRECTX11)
#elif defined(OPENHOI_OS_APPLE)
      renderingSystem(RenderingSystem::METAL)
#else
      renderingSystem(RenderingSystem::OPENGL3PLUS)
#endif
{
}

// Gets the rendering device name
std::string const& Options::GetRenderingDevice() const {
  return this->renderingDevice;
}

// Sets the rendering device name
void Options::SetRenderingDevice(std::string const& renderingDevice) {
  this->renderingDevice = renderingDevice;
}

// Gets the video mode name
std::string const& Options::GetVideoMode() const { return this->videoMode; }

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
WindowMode const& Options::GetWindowMode() const { return this->windowMode; }

// Sets the window mode
void Options::SetWindowMode(WindowMode const& windowMode) {
  this->windowMode = windowMode;
}

// Gets the vertical sync flag
bool const& Options::IsVerticalSync() const { return this->verticalSync; }

// Sets the vertical sync flag
void Options::SetVerticalSync(bool const& verticalSync) {
  this->verticalSync = verticalSync;
}

// Gets the rendering system
RenderingSystem const& Options::GetRenderingSystem() const {
  return this->renderingSystem;
}

// Sets the rendering system
void Options::SetRenderingSystem(RenderingSystem const& renderingSystem) {
  this->renderingSystem = renderingSystem;
}

}  // namespace openhoi