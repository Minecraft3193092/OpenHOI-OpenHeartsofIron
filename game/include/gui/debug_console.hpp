// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "gui/gui_element.hpp"

namespace openhoi {

// Debug console GUI element
class DebugConsole final : public GuiElement {
 public:
   // Creates the debug console
  DebugConsole();

  // Destroys the debug console
  ~DebugConsole();

  // Add log to debug console
  void addLog(const char* fmt, ...);

  // Draw the debug console (inherited from GuiElement)
  virtual void draw();
};

}  // namespace openhoi