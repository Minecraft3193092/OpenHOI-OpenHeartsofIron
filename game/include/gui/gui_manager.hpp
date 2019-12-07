// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "gui/debug_console.hpp"

#include <hoibase/helper/os.hpp>

#include <OgrePrerequisites.h>
#include <SDL.h>
#include <imgui.h>

namespace openhoi {

// GUI manager for openhoi
class GuiManager final {
 public:
  // Initializes the GUI manager
  GuiManager();

  // Destroys the GUI manager
  ~GuiManager();

  // Initialize GUI manager
  void initialize(Ogre::SceneManager* sceneManager,
                  Ogre::RenderSystem* renderSystem, SDL_Window* window);

  // Handle SDL event
  void handleEvent(SDL_Event event);

  // Render new GUI frame
  void newFrame(const Ogre::FrameEvent& e);

  // Gets the default font
  ImFont* getDefaultFont();

  // Gets the big font
  ImFont* getBigFont();

  // Toggle debug console
  void toggleDebugConsole();

 private:
  // Configure GUI
  void configureGui();

  Ogre::SceneManager* sceneManager;
  SDL_Window* window;
  DebugConsole* debugConsole;
  ImFont* defaultFont;
  ImFont* bigFont;
};

}  // namespace openhoi