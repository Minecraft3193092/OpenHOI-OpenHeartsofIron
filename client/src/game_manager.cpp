// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"

#include <OgreLogManager.h>
#include <OgreOverlaySystem.h>
#include <OgreRTShaderSystem.h>
#include <OgreRoot.h>
#include <OgreTextureManager.h>

namespace openhoi {

// Initializes the game manager
GameManager::GameManager() {
  // Create options instance
  options.reset(new Options());

  // Create the game manager base delegate
  gameManagerBase = OGRE_NEW GameManagerBase(options);

  // Set OGRE root
  root = gameManagerBase->getRoot();

  // Create a generic scene manager
  sceneManager = root->createSceneManager(Ogre::ST_GENERIC);

  // Register our scene with the RTSS
  Ogre::RTShader::ShaderGenerator* shaderGenerator =
      Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shaderGenerator->addSceneManager(sceneManager);

  // Disable mipmapping
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
}

// Destroys the gam manager
GameManager::~GameManager() {
  // Shutdown state manager
  if (stateManager) stateManager->RequestStateChange(nullptr);
}

// Gets the game options
std::shared_ptr<Options> const& GameManager::GetOptions() const {
  return options;
}

// Gets the state manager
std::unique_ptr<StateManager> const& GameManager::GetStateManager() const {
  return stateManager;
}

// Gets the OGRE root object (no smart pointer because we don't own the root)
Ogre::Root* const& GameManager::GetRoot() const { return this->root; }

// Gets the OGRE scene manager
Ogre::SceneManager* const& GameManager::GetSceneManager() const {
  return sceneManager;
}

// Start the main loop
void GameManager::Run() {
  // Start rendering
  root->startRendering();

  // Close down the game
  gameManagerBase->closeApp();
}

// Request game exit
void GameManager::RequestExit() { root->queueEndRendering(true); }

}  // namespace openhoi