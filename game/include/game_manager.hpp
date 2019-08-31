// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "options.hpp"
#include "state/state_manager.hpp"

#ifdef OPENHOI_OS_WINDOWS
#  define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgrePrerequisites.h>
#include <OgreSceneManager.h>
#ifdef OPENHOI_OS_WINDOWS
#  undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif
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

  // Frame started event (override OGRE Bites)
  virtual bool frameStarted(const Ogre::FrameEvent& evt);

  // Frame rendering queued event (overrides OGRE Bites)
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

  // Key released event
  virtual bool keyReleased(const OgreBites::KeyboardEvent& arg);

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

  Options* options;
  StateManager* stateManager;
  Ogre::SceneManager* sceneManager;
  Ogre::Camera* camera;
};

}  // namespace openhoi