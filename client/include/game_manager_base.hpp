// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "options.hpp"

#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <hoibase/helper/os.hpp>

#include <string>

namespace openhoi {

// Internal game manager base component used to communicate with OGRE and OGRE
// Bites
class GameManagerBase final : public OgreBites::ApplicationContext,
                              public OgreBites::InputListener {
 public:
  // Initializes the game manager base
  GameManagerBase(std::shared_ptr<Options> options);

  // Destroys the game manager base
  ~GameManagerBase();

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

 private:
  // Create the render system
  void CreateRenderSystem();

  std::shared_ptr<Options> options;
  std::unique_ptr<Ogre::RenderSystem> renderSystem;
};

}  // namespace openhoi