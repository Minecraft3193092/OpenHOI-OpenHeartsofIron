// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "state/menu_state.hpp"
#include "game_manager.hpp"

#include <Ogre.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>

#include <cassert>

namespace openhoi {

// Called when the state is started up
void MenuState::startup() {
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Menu State startup ***");

  backgroundImageName =
      OPENHOI_BUILD_DYNAMIC_OBJECT_NAME("Main_Menu_Background");
}

// Called when the state is shutting down
void MenuState::shutdown() {
  Ogre::LogManager::getSingletonPtr()->logMessage(
      "*** Menu State shutdown ***");
}

// Used to create the scene
void MenuState::createScene() {
  // Set ambient light
  GameManager::getInstance().getSceneManager()->setAmbientLight(
      Ogre::ColourValue(1.0f, 1.0f, 1.0f));

  // Create background image
  createBackground();
}

// Create background image
void MenuState::createBackground() {
  GameManager& gameManager = GameManager::getInstance();

  // Get background texture
  Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(
      "background.jpg", Ogre::RGN_DEFAULT);
  assert(texture->isLoaded());

  // Create background material
  Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
      backgroundImageName, Ogre::RGN_DEFAULT);
  material->getTechnique(0)->getPass(0)->createTextureUnitState(
      texture->getName());
  material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
  material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
  material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

  // Create background rectangle covering the whole screen
  int windowWidth = gameManager.getRenderWindow()->getWidth();
  int windowHeight = gameManager.getRenderWindow()->getHeight();
  Ogre::Real xRatio = 1, yRatio = 1;
  if (texture->getWidth() != windowWidth ||
      texture->getHeight() != windowHeight) {
    // Texture size is not the same as the window size. Thus, we have to find a
    // stretch factor for the corners
    Ogre::Real backgroundRatio =
        (Ogre::Real)texture->getWidth() / texture->getHeight();
    if (backgroundRatio >= 1) {
      // Width of BG image is greater or equal than height
      xRatio =
          (OPENHOI_OGRE_ABS(1 - (Ogre::Real)texture->getWidth() / windowWidth) /
           2) +
          1;
    } else {
      // Height of BG image is greater than width
      yRatio = (OPENHOI_OGRE_ABS(1 - (Ogre::Real)texture->getHeight() /
                                         windowHeight) /
                2) +
               1;
    }
  }
  backgroundImageRect = OGRE_NEW Ogre::Rectangle2D(true);
  backgroundImageRect->setCorners(xRatio * -1, yRatio, xRatio, yRatio * -1);
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
void MenuState::updateScene() {}

// Used to remove the scene
void MenuState::removeScene() {
  // Delete background image
  GameManager::getInstance()
      .getSceneManager()
      ->getRootSceneNode()
      ->removeAndDestroyChild(backgroundImageName);
  if (backgroundImageRect) delete backgroundImageRect;
  backgroundImageRect = nullptr;
}

}  // namespace openhoi