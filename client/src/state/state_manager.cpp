// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "state/state_manager.hpp"

namespace openhoi {

// StateManager destructor
StateManager::~StateManager() {
  // If a state is active, shut down the state to clean up
  if (currentState != nullptr) {
    // Switch state to null
    switchToNewState(nullptr);
  }

  if (newState != nullptr) delete newState;
  if (currentState != nullptr) delete currentState;
}

// Starts up the state manager *
void StateManager::startup(State* firstState) {
  // Can't start up the state manager again if it's already running
  if (currentState != nullptr || newState != nullptr) return;

  // Initialize with first state
  requestStateChange(firstState);
}

// Update the current state
void StateManager::updateState() {
  // Check if a state change was requested. If yes, switch to the new state
  if (newState != nullptr) switchToNewState(newState);

  // If a state is active, update it
  if (currentState != nullptr) currentState->updateScene();
}

// Request state manager to change state
void StateManager::requestStateChange(State* newState) {
  // Don't change the state if the requested state class matches the current
  // state
  if (currentState && newState == currentState) return;

  // Backup old state
  State* oldState = this->newState;

  // Store new state to request a state change
  this->newState = newState;

  // If necessary, delete old state
  if (oldState != nullptr) delete oldState;
}

// Switch to the new state
void StateManager::switchToNewState(State* newState) {
  // If a state is active, shut it down
  if (currentState != nullptr) {
    // Remove the current scene
    currentState->removeScene();

    // Shutdown the current state
    currentState->shutdown();
  }

  // Backup old state
  State* oldState = currentState;

  // Switch to the new state, might be null if no new state should be activated
  this->currentState = newState;

  // Delete "old" new state
  this->newState = nullptr;

  // If necessary, delete old state
  if (oldState != nullptr) delete oldState;

  // If a state is active, start it up
  if (currentState) {
    // Startup new state
    currentState->startup();

    // Create the new scene
    currentState->createScene();
  }
}

}  // namespace openhoi