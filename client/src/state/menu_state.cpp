// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "state/menu_state.hpp"
#include "game_manager.hpp"

#include <Ogre.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>

namespace openhoi {

// Called when the state is started up
void MenuState::Startup() {
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Menu State startup ***");

  backgroundImageName =
      OPENHOI_BUILD_DYNAMIC_OBJECT_NAME("Main_Menu_Background");
}

// Called when the state is shutting down
void MenuState::Shutdown() {
  Ogre::LogManager::getSingletonPtr()->logMessage(
      "*** Menu State shutdown ***");
}

// Used to create the scene
void MenuState::CreateScene() {
  GameManager& gameManager = GameManager::getInstance();

  // Set ambient light
  gameManager.getSceneManager()->setAmbientLight(
      Ogre::ColourValue(1.0f, 1.0f, 1.0f));

  // Create background material
  Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
      backgroundImageName, Ogre::RGN_DEFAULT);
  material->getTechnique(0)->getPass(0)->createTextureUnitState(
      "background.jpg");
  material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
  material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
  material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

  // Create background rectangle covering the whole screen
  backgroundImageRect = OGRE_NEW Ogre::Rectangle2D(true);
  backgroundImageRect->setCorners(-1.0, 1.0, 1.0, -1.0);
  backgroundImageRect->setMaterial(material);

  // Render the background before everything else
  backgroundImageRect->setRenderQueueGroup(
      Ogre::RenderQueueGroupID::RENDER_QUEUE_BACKGROUND);

  // Use infinite AAB to always stay visible
  Ogre::AxisAlignedBox aabInf;
  aabInf.setInfinite();
  backgroundImageRect->setBoundingBox(aabInf);

  // Attach background to the scene
  Ogre::SceneNode* node =
      gameManager.getSceneManager()->getRootSceneNode()->createChildSceneNode(
          backgroundImageName);
  node->attachObject(backgroundImageRect);
}

// Used to update the scene
void MenuState::UpdateScene() {}

// Used to remove the scene
void MenuState::RemoveScene() {
  // Delete background image
  GameManager::getInstance()
      .getSceneManager()
      ->getRootSceneNode()
      ->removeAndDestroyChild(backgroundImageName);
  if (backgroundImageRect) delete backgroundImageRect;
  backgroundImageRect = nullptr;
}

}  // namespace openhoi