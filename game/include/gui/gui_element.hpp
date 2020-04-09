// Copyright 2018-2020 the openhoi authors. See COPYING.md for legal info.

#pragma once

namespace openhoi {

// Abstract GUI element base class
class GuiElement {
 public:
  // Toggle the GUI element's visibility flag
  void toggle();

  // Gets the visibility flag
  bool const& isVisible() const;

  // Sets the visibility flag
  void setVisible(bool const& visible);

  // Draw the GUI element
  virtual void draw() = 0;

 protected:
  // Creates the GUI element
  GuiElement();

  bool visible;
};

}  // namespace openhoi