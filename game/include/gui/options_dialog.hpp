// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <imgui.h>

#include "gui/gui_element.hpp"

namespace openhoi {

// Options dialog
class OptionsDialog final : public GuiElement {
 public:
  // Creates the options dialog
  OptionsDialog();

  // Destroys the options dialog
  ~OptionsDialog();

  // Draw the options dialog (inherited from GuiElement)
  virtual void draw();
};

}  // namespace openhoi