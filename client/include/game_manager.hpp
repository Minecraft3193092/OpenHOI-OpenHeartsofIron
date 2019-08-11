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

#include <array>
#include <map>
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

  // Gets the OGRE camera
  Ogre::Camera* const& getCamera() const;

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

  // Create camera
  void createCamera();

  // Gets the full path to the provided OGRE plugin
  std::string getPluginPath(std::string pluginName);

  // Declare resources in the provided directory (non-recusive!) with the
  // provided type for the given resource group
  void declareResources(filesystem::path directory, std::string resourceType,
                        std::string resourceGroup = Ogre::RGN_DEFAULT);

  std::array<std::string, 3> defaultResourceGroups = {Ogre::RGN_DEFAULT, OPENHOI_RSG_COA_TEXTURES, OPENHOI_RSG_FLAG_TEXTURES};
  Options* options;
  StateManager* stateManager;
  Ogre::SceneManager* sceneManager;
  Ogre::Camera* camera;
};

}  // namespace openhoi