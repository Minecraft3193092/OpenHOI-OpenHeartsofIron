// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "game_manager_base.hpp"
#include "options.hpp"
#include "state/state_manager.hpp"

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <hoibase/helper/os.hpp>

#include <string>

namespace openhoi {

// Manager used to handle the main game loop and the basic logic required to run
// the game
class GameManager final {
 public:
  static GameManager& GetInstance() {
    // Thread-safe C++11 singleton
    static GameManager instance;
    return instance;
  }

  // Start the main loop
  void Run();

  /// Request game exit
  void RequestExit();

  // Gets the game options
  std::shared_ptr<Options> const& GetOptions() const;

  // Gets the state manager
  std::unique_ptr<StateManager> const& GetStateManager() const;

  // Gets the OGRE root object (no smart pointer because we don't own the root)
  Ogre::Root* const& GetRoot() const;

  // Gets the OGRE scene manager
  std::unique_ptr<Ogre::SceneManager> const& GetSceneManager() const;

 protected:
  // Initializes the game manager
  GameManager();

  // Destroys the game manager
  ~GameManager();

 private:
  std::unique_ptr<GameManagerBase> gameManagerBase;
  std::shared_ptr<Options> options;
  std::unique_ptr<StateManager> stateManager;
  std::unique_ptr<Ogre::SceneManager> sceneManager;
  Ogre::Root* root;
};

}  // namespace openhoi