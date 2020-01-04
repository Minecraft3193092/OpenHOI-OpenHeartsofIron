// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <OgrePrerequisites.h>

#include "state.hpp"

namespace openhoi {

// State manager class
class StateManager final {
 public:
  // StateManager destructor
  ~StateManager();

  // Starts up the state manager *
  void startup(State* firstState);

  // Update the current state
  void updateState(const Ogre::FrameEvent& evt);

  // Update the GUI of the current state
  void updateGui();

  // Request state manager to change state
  void requestStateChange(State* newState);

 private:
  // Switch to the new state
  void switchToNewState(State* newState);

  // Describes the current state
  State* currentState = nullptr;
  // Describes the (possible) new state
  State* newState = nullptr;
};

}  // namespace openhoi