// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "gui/debug_console.hpp"
#include "gui/imgui_renderable.hpp"

#include <OgrePrerequisites.h>
#include <OgreRenderQueueListener.h>
#include <OgreSceneManager.h>

namespace openhoi {

// GUI manager for openhoi
class GuiManager final : public Ogre::RenderQueueListener {
 public:
  static GuiManager& getInstance() {
    // Thread-safe C++11 singleton
    static GuiManager instance;
    return instance;
  }

  // Initialize GUI manager
  void initialize(Ogre::SceneManager* sceneManager);

  // Render queue ended event
  virtual void renderQueueEnded(Ogre::uint8 queueGroupId,
                                const Ogre::String& invocation,
                                bool& /*repeatThisInvocation*/);

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

  // Load a single font
  ImFont* loadFont(std::string name);

  // Create font texture
  void createFontTexture();

  // Create GUI material
  void createMaterial();

  Ogre::SceneManager* sceneManager;
  Ogre::TexturePtr fontTexture;
  std::vector<std::vector<ImWchar>> codePointRanges;
  ImGuiRenderable* renderable;
  bool frameEnded;
  DebugConsole* debugConsole;
  ImFont* defaultFont;
  ImFont* bigFont;
};

}  // namespace openhoi