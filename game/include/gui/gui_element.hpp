// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

namespace openhoi {

// Abstract GUI element base class
class GuiElement {
 public:
  // Draw the GUI element
  virtual void draw() = 0;
};

}  // namespace openhoi