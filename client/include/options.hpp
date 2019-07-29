// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <hoibase/openhoi.hpp>

#include <string>

namespace openhoi {

// Window mode enumeration
enum WindowMode { WINDOWED, FULLSCREEN, BORDERLESS };

// Options class
class Options final {
 public:
  // Options constructor
  Options();

  // Gets the rendering device name
  std::string const& GetRenderingDevice() const;

  // Sets the rendering device name
  void SetRenderingDevice(std::string const& renderingDevice);

  // Gets the video mode name
  std::string const& GetVideoMode() const;

  // Sets the video mode name
  void SetVideoMode(std::string const& videoMode);

  // Gets the FSAA level
  byte const& GetFullScreenAntiAliasing() const;

  // Sets the FSAA level
  void SetFullScreenAntiAliasing(byte const& fullScreenAntiAliasing);

  // Gets the window mode
  WindowMode const& GetWindowMode() const;

  // Sets the window mode
  void SetWindowMode(WindowMode const& windowMode);

  // Gets the vertical sync flag
  bool const& IsVerticalSync() const;

  // Sets the vertical sync flag
  void SetVerticalSync(bool const& verticalSync);

 private:
  std::string renderingDevice;
  std::string videoMode;
  byte fullScreenAntiAliasing;
  WindowMode windowMode;
  bool verticalSync;
};

}  // namespace openhoi