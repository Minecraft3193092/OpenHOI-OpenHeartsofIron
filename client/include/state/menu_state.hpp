// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "state.hpp"

#include <OgreRectangle2D.h>

namespace openhoi {

// Main menu state class
class MenuState final : public State {
 public:
  // MenuState destructor
  ~MenuState(){};

  // Called when the state is started up
  void Startup();

  // Called when the state is shutting down
  void Shutdown();

  // Used to create the scene
  void CreateScene();

  // Used to update the scene
  void UpdateScene();

  // Used to remove the scene
  void RemoveScene();

 private:
  std::string backgroundImageName;
  Ogre::Rectangle2D* backgroundImageRect;
};

}  // namespace openhoi