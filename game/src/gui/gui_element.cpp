// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#include "gui/gui_element.hpp"

namespace openhoi {

// Creates the GUI element
GuiElement::GuiElement() {
  // Show the element by default
  visible = true;
}

// Toggle the GUI element's visibility flag
void GuiElement::toggle() { visible = !visible; }

// Gets the visibility flag
bool const& GuiElement::isVisible() const { return visible; }

// Sets the visibility flag
void GuiElement::setVisible(bool const& visible) { this->visible = visible; }

}  // namespace openhoi