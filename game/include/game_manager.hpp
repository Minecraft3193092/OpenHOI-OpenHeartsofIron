// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "gui/gui_manager.hpp"
#include "options.hpp"
#include "state/state_manager.hpp"

#include <hoibase/file/filesystem.hpp>
#include <hoibase/helper/os.hpp>

#include <OgreFrameListener.h>
#include <OgreMaterialManager.h>
#include <OgreOverlaySystem.h>
#include <OgrePrerequisites.h>
#include <OgreRTShaderSystem.h>
#include <OgreSceneManager.h>

#include <array>
#include <map>
#include <string>

namespace openhoi {

// Manager used to handle the main game loop and the basic logic required to run
// the game
class GameManager final : public Ogre::FrameListener,
                          public Ogre::MaterialManager::Listener {
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

  // Gets the GUI manager
  GuiManager* const& getGuiManager() const;

  // Gets the OGRE scene manager
  Ogre::SceneManager* const& getSceneManager() const;

  // Gets the OGRE camera
  Ogre::Camera* const& getCamera() const;

  // Get the OGRE render window
  Ogre::RenderWindow* const& getRenderWindow() const;

  // Frame started event
  virtual bool frameStarted(const Ogre::FrameEvent& /*evt*/);

  // Frame rendering queued event
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& /*evt*/);

  // Key released event
  // bool keyReleased(const KeyboardEvent& arg);

  // Hook point where shader based technique will be created. Will be called
  // whenever the material manager won't find appropriate technique to satisfy
  // the target scheme name. If the scheme name is out target RT Shader System
  // scheme name we will try to create shader generated technique for it
  Ogre::Technique* handleSchemeNotFound(unsigned short /*schemeIndex*/,
                                        const Ogre::String& schemeName,
                                        Ogre::Material* originalMaterial,
                                        unsigned short /*lodIndex*/,
                                        const Ogre::Renderable* /*rend*/);

  // Called right before illuminated passes were created, so that owner of
  // runtime generated technique can handle this
  bool beforeIlluminationPassesCleared(Ogre::Technique* tech);

  // Called right after illuminated passes were created, so that owner of
  // runtime generated technique can handle this
  bool afterIlluminationPassesCreated(Ogre::Technique* tech);

 protected:
  // Initializes the game manager
  GameManager();

  // Destroys the game manager
  ~GameManager();

 private:
  // Create a new render window
  void createWindow();

  // Load and configure the render system
  void loadRenderSystem();

  // Create camera
  void createCamera();

  // Poll for events
  void pollEvents();

  // Locate resources
  void locateResources();

  // Load resources
  void loadResources();

  // Gets the full path to the provided OGRE plugin
  std::string getPluginPath(std::string pluginName);

  Options* options;
  StateManager* stateManager;
  GuiManager* guiManager;
  Ogre::Root* root;
  Ogre::OverlaySystem* overlaySystem;
  Ogre::SceneManager* sceneManager;
  std::vector<NativeWindowPair> windows;
  Ogre::Camera* camera;
  Ogre::RTShader::ShaderGenerator* shaderGenerator;
};

}  // namespace openhoi