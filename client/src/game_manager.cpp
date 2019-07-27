// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"

#include <hoibase/file/file_access.hpp>

namespace openhoi {

// Initializes the game manager
GameManager::GameManager() {
  // Create options instance
  this->options.reset(new Options());

  // Create root object of OGRE system
  this->root.reset(OGRE_NEW Ogre::Root("", "", (FileAccess::GetUserGameConfigDirectory() / "client.log").u8string()));
}

// Destroys the GameManager
GameManager::~GameManager() {}

// Gets the game options
std::unique_ptr<Options> const& GameManager::GetOptions() const {
  return this->options;
}

// Gets the state manager
std::unique_ptr<StateManager> const& GameManager::GetStateManager() const {
  return this->stateManager;
}

// Gets the OGRE root object
std::unique_ptr<Ogre::Root> const& GameManager::GetRoot() const {
  return this->root;
}

// Gets the OGRE render window
std::unique_ptr<Ogre::RenderWindow> const& GameManager::GetWindow() const {
  return this->window;
}

// Gets the OGRE scene manager
std::unique_ptr<Ogre::SceneManager> const& GameManager::GetSceneManager()
    const {
  return this->sceneManager;
}

// Gets the OGRE camera
std::unique_ptr<Ogre::Camera> const& GameManager::GetCamera() const {
  return this->camera;
}

  // Start the main loop
void GameManager::Run()
{
  // xxx
}

/// Request game exit
void GameManager::RequestExit() {
  // xxxx
}

}  // namespace openhoi