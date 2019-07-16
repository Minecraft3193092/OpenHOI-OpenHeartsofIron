// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "state/state_manager.hpp"

namespace openhoi {

// StateManager destructor
StateManager::~StateManager() {
  // If a state is active, shut down the state to clean up
  if (this->currentState != nullptr) {
    // Switch state to null
    SwitchToNewState(nullptr);
  }

  if (this->newState != nullptr) delete newState;
  if (this->currentState != nullptr) delete currentState;
}

// Starts up the state manager *
void StateManager::Startup(State* firstState) {
  // Can't start up the state manager again if it's already running
  if (this->currentState != nullptr || this->newState != nullptr) return;

  // Initialize with first state
  RequestStateChange(firstState);
}

// Update the current state
void StateManager::UpdateState() {
  // Check if a state change was requested. If yes, switch to the new state
  if (this->newState != nullptr) SwitchToNewState(this->newState);

  // If a state is active, update it
  if (this->currentState != nullptr) this->currentState->UpdateScene();
}

// Request state manager to change state
void StateManager::RequestStateChange(State* newState) {
  // Don't change the state if the requested state class matches the current
  // state
  if (this->currentState && newState == this->currentState) return;

  // Backup old state
  State* oldState = this->newState;

  // Store new state to request a state change
  this->newState = newState;

  // If necessary, delete old state
  if (oldState != nullptr) delete oldState;
}

// Switch to the new state
void StateManager::SwitchToNewState(State* newState) {
  // If a state is active, shut it down
  if (this->currentState != nullptr) {
    // Remove the current scene
    this->currentState->RemoveScene();

    // Shutdown the current state
    this->currentState->Shutdown();
  }

  // Backup old state
  State* oldState = this->currentState;

  // Switch to the new state, might be null if no new state should be activated
  this->currentState = newState;

  // Delete "old" new state
  this->newState = nullptr;

  // If necessary, delete old state
  if (oldState != nullptr) delete oldState;

  // If a state is active, start it up
  if (this->currentState) {
    // Startup new state
    this->currentState->Startup();

    // Create the new scene
    this->currentState->CreateScene();
  }
}

}  // namespace openhoi