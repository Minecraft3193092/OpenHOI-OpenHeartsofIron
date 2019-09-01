// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "gui/debug_console.hpp"

#include <ImguiManager.h>
#include <OgrePrerequisites.h>
#include <OgreSceneManager.h>

namespace openhoi {

// GUI manager for openhoi
class GuiManager final {
 public:
  static GuiManager& getInstance() {
    // Thread-safe C++11 singleton
    static GuiManager instance;
    return instance;
  }

  // Initialize GUI manager
  void initialize(Ogre::SceneManager* sceneManager);

  // Load and create fonts
  void loadFonts();

  // Get input listener
  OgreBites::InputListener* getInputListener();

  // Render new GUI frame
  void newFrame(const Ogre::FrameEvent& evt, Ogre::uint32 windowWidth,
                Ogre::uint32 windowHeight);

  // Gets the default font
  ImFont* getDefaultFont();

  // Gets the big font
  ImFont* getBigFont();

  // Toggle debug console
  void toggleDebugConsole();

 protected:
  // Initializes the GUI manager
  GuiManager();

  // Destroys the GUI manager
  ~GuiManager();

 private:
  // Configure GUI
  void configureGui();

  Ogre::ImguiManager* imGuiManager;
  DebugConsole* debugConsole;
  ImFont* defaultFont;
  ImFont* bigFont;
};

}  // namespace openhoi