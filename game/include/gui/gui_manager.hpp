// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "gui/debug_console.hpp"
#include "gui/imgui_renderable.hpp"

#include <hoibase/helper/os.hpp>

#include <OgrePrerequisites.h>
#include <OgreRenderQueueListener.h>
#include <OgreSceneManager.h>
#include <SDL.h>
#include <imgui.h>

namespace openhoi {

// Link between a OGRE render window and a SDL window
struct NativeWindowPair {
  Ogre::RenderWindow* ogre;
  SDL_Window* sdl;
};

// GUI manager for openhoi
class GuiManager final : public Ogre::RenderQueueListener {
 public:
  // Initializes the GUI manager
  GuiManager();

  // Destroys the GUI manager
  ~GuiManager();

  // Initialize GUI manager
  void initialize(Ogre::SceneManager* sceneManager,
                  Ogre::RenderSystem* renderSystem,
                  std::vector<NativeWindowPair> windows);

  // Render queue ended event
  virtual void renderQueueEnded(Ogre::uint8 queueGroupId,
                                const Ogre::String& invocation,
                                bool& /*repeatThisInvocation*/);

  // Handle SDL event
  void handleEvent(SDL_Event event);

  // Render new GUI frame
  void newFrame();

  // Gets the default font
  ImFont* getDefaultFont();

  // Gets the big font
  ImFont* getBigFont();

  // Toggle debug console
  void toggleDebugConsole();

 private:
  // Configure GUI
  void configureGui();

  // Load a single font
  ImFont* loadFont(std::string name);

  // Create font texture
  void createFontTexture();

  // Create GUI material
  void createMaterial();

  Ogre::SceneManager* sceneManager;
  Ogre::TexturePtr fontTexture;
  std::vector<std::vector<ImWchar>> codePointRanges;
  ImGuiRenderable renderable;
  bool frameEnded;
  DebugConsole* debugConsole;
  ImFont* defaultFont;
  ImFont* bigFont;
  SDL_Window* window;
};

}  // namespace openhoi