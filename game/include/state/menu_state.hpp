// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "state.hpp"

#include <hoibase/helper/os.hpp>

#ifdef OPENHOI_OS_WINDOWS
#  define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif
#include <OgreRectangle2D.h>
#ifdef OPENHOI_OS_WINDOWS
#  undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

namespace openhoi {

// Main menu state class
class MenuState final : public State {
 public:
  // MenuState destructor
  ~MenuState(){};

  // Called when the state is started up
  void startup();

  // Called when the state is shutting down
  void shutdown();

  // Used to create the scene
  void createScene();

  // Used to update the scene
  void updateScene();

  // Used to update the GUI of the scene
  void updateGui();

  // Used to remove the scene
  void removeScene();

 private:
  // Create background image
  void createBackground();

  // Create logo
  void createLogo();

  std::string backgroundImageName;
  Ogre::Rectangle2D* backgroundImageRect;
};

}  // namespace openhoi