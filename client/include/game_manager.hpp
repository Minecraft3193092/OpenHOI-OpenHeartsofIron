// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "options.hpp"
#include "state/state_manager.hpp"

#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
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
  std::unique_ptr<Options> const& GetOptions() const;

  // Gets the state manager
  std::unique_ptr<StateManager> const& GetStateManager() const;

  // Gets the OGRE root object
  std::unique_ptr<Ogre::Root> const& GetRoot() const;

  // Gets the OGRE render window
  std::unique_ptr<Ogre::RenderWindow> const& GetWindow() const;

  // Gets the OGRE scene manager
  std::unique_ptr<Ogre::SceneManager> const& GetSceneManager() const;

  // Gets the OGRE camera
  std::unique_ptr<Ogre::Camera> const& GetCamera() const;

 protected:
  // Initializes the game manager
  GameManager();
  // Destroys the GameManager
  ~GameManager();

 private:
  std::unique_ptr<Options> options;
  std::unique_ptr<StateManager> stateManager;
  std::unique_ptr<Ogre::Root> root;
  std::unique_ptr<Ogre::RenderWindow> window;
  std::unique_ptr<Ogre::SceneManager> sceneManager;
  std::unique_ptr<Ogre::Camera> camera;
};

}  // namespace openhoi