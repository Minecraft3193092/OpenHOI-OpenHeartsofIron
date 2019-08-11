// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "options.hpp"
#include "state/state_manager.hpp"

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgreSceneManager.h>
#include <hoibase/file/filesystem.hpp>
#include <hoibase/helper/os.hpp>

#include <string>

namespace openhoi {

// Manager used to handle the main game loop and the basic logic required to run
// the game
class GameManager final : public OgreBites::ApplicationContext,
                          public OgreBites::InputListener {
 public:
  static GameManager& getInstance() {
    // Thread-safe C++11 singleton
    static GameManager instance;
    return instance;
  }

  // Start the main loop
  void run();

  /// Request game exit
  void requestExit();

  // Gets the game options
  Options* const& getOptions() const;

  // Gets the state manager
  StateManager* const& getStateManager() const;

  // Gets the OGRE scene manager
  Ogre::SceneManager* const& getSceneManager() const;

  // Creates the OGRE root (overrides OGRE Bites)
  virtual void createRoot();

  // Create a new render window (overrides OGRE Bites)
  virtual OgreBites::NativeWindowPair createWindow(
      const Ogre::String& name, uint32_t w = 0, uint32_t h = 0,
      Ogre::NameValuePairList miscParams = Ogre::NameValuePairList());

  // Locate resources (overrides OGRE Bites)
  virtual void locateResources();

  // Load resources (overrides OGRE Bites)
  virtual void loadResources();

  // Frame rendering queued event (overrides OGRE Bites)
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

 protected:
  // Initializes the game manager
  GameManager();

  // Destroys the game manager
  ~GameManager();

 private:
  // Load and configure the render system
  void loadRenderSystem();

  // Gets the full path to the provided OGRE plugin
  std::string getPluginPath(std::string pluginName);

  // Recursively declare resources with the provided type
  void declareResources(filesystem::path directory, std::string resourceType);

  Options* options;
  StateManager* stateManager;
  Ogre::SceneManager* sceneManager;
};

}  // namespace openhoi