// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "state/menu_state.hpp"
#include "game_manager.hpp"
#include "graphic/texture_helper.hpp"
#include "gui/gui_manager.hpp"

#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayManager.h>
#include <OgrePrerequisites.h>
#include <OgreTextureManager.h>

#include <algorithm>
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

  // Create logo
  createLogo();
}

// Create background image
void MenuState::createBackground() {
  GameManager& gameManager = GameManager::getInstance();

  // Create background material
  Ogre::MaterialPtr material =
      Ogre::MaterialManager::getSingleton().getByName("background");
  assert(material.operator bool() && material->isLoaded());

  // Get background texture
  Ogre::TexturePtr texture =
      TextureHelper::extractTextureFromSimpleMaterial(material);
  assert(texture.operator bool() && texture->isLoaded());

  // Create background rectangle covering the whole screen
  Ogre::uint32 windowWidth = gameManager.getRenderWindow()->getWidth();
  Ogre::uint32 windowHeight = gameManager.getRenderWindow()->getHeight();
  Ogre::Real xRatio = 1, yRatio = 1;
  if (texture->getWidth() != windowWidth ||
      texture->getHeight() != windowHeight) {
    // Texture size is not the same as the window size. Thus, we have to find a
    // stretch factor for the corners
    Ogre::Real backgroundRatio =
        TextureHelper::getTextureWidthHeightRatio(texture);
    if (backgroundRatio >= 1) {
      // Width of BG image is greater or equal than height
      xRatio =
          (Ogre::Math::Abs(1 - (Ogre::Real)texture->getWidth() / windowWidth) /
           2) +
          1;
    } else {
      // Height of BG image is greater than width
      yRatio = (Ogre::Math::Abs(1 - (Ogre::Real)texture->getHeight() /
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

// Create logo
void MenuState::createLogo() {
  GameManager& gameManager = GameManager::getInstance();
  Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

  // Create an overlay
  Ogre::Overlay* overlay = overlayManager.create(
      OPENHOI_BUILD_DYNAMIC_OBJECT_NAME("Main_Menu_Logo"));

  // Get width/height ratio of logo
  Ogre::TexturePtr texture =
      TextureHelper::extractTextureFromSimpleMaterial("logo");
  assert(texture.operator bool() && texture->isLoaded());
  Ogre::Real logoRatio = TextureHelper::getTextureWidthHeightRatio(texture);

  // Calculate panel (=logo) height
  int windowWidth = gameManager.getRenderWindow()->getWidth();
  int windowHeight = gameManager.getRenderWindow()->getHeight();
  Ogre::Real panelHeight =
      (0.4f / logoRatio) * ((Ogre::Real)windowWidth / windowHeight);

  // Create a panel
  Ogre::OverlayContainer* panel =
      static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement(
          "Panel", OPENHOI_BUILD_DYNAMIC_OBJECT_NAME("Main_Menu_Logo_Panel")));
  panel->setPosition(0.3f, 0.5f - (panelHeight / 2));
  panel->setDimensions(0.4f, panelHeight);
  panel->setMaterialName("logo");

  // Add panel to overlay
  overlay->add2D(panel);

  // Show the overlay
  overlay->show();
}

// Used to update the scene
void MenuState::updateScene() {
  // xxxx
}

// Used to update the GUI of the scene
void MenuState::updateGui() {
  // Get window size
  GameManager& gameManager = GameManager::getInstance();
  int windowWidth = gameManager.getRenderWindow()->getWidth();
  int windowHeight = gameManager.getRenderWindow()->getHeight();

  // ----------------------------------------------------------------------------

  // Render menu buttons
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImVec2 buttonAreaSize = ImVec2(windowWidth, windowHeight * (Ogre::Real)0.1f);
  ImGui::SetNextWindowSize(buttonAreaSize, ImGuiCond_Always);
  ImGui::SetNextWindowContentSize(buttonAreaSize);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("Game Menu Buttons", NULL,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

  ImVec2 buttonSize = ImVec2(200, std::min(50.0f, buttonAreaSize.y));
  const int numberOfButtons = 5;
  float widthOfAllButtons = (buttonSize.x * numberOfButtons);
  float buttonSpacing =
      std::min(buttonSize.x * 0.4f,
               (windowWidth - widthOfAllButtons) / (numberOfButtons + 1));
  float widthOfAllSpacings = buttonSpacing * (numberOfButtons - 1);
  float buttonLeftPos =
      (windowWidth - widthOfAllButtons - widthOfAllSpacings) / 2;
  float buttonTopPos = (buttonAreaSize.y - buttonSize.y) / 2;

  ImGui::PushFont(GuiManager::getInstance().getBigFont());
  ImVec4 originalButtonStyle = ImGui::GetStyle().Colors[ImGuiCol_Button];
  ImGui::GetStyle().Colors[ImGuiCol_Button] =
      ImVec4(originalButtonStyle.x, originalButtonStyle.y,
             originalButtonStyle.z, 0.8f);

  ImGui::SetCursorPos(ImVec2(buttonLeftPos, buttonTopPos));
  ImGui::Button("Singleplayer", buttonSize);

  buttonLeftPos += buttonSpacing + buttonSize.x;
  ImGui::SetCursorPos(ImVec2(buttonLeftPos, buttonTopPos));
  ImGui::Button("Multiplayer", buttonSize);

  buttonLeftPos += buttonSpacing + buttonSize.x;
  ImGui::SetCursorPos(ImVec2(buttonLeftPos, buttonTopPos));
  ImGui::Button("Options", buttonSize);

  buttonLeftPos += buttonSpacing + buttonSize.x;
  ImGui::SetCursorPos(ImVec2(buttonLeftPos, buttonTopPos));
  ImGui::Button("Credits", buttonSize);

  buttonLeftPos += buttonSpacing + buttonSize.x;
  ImGui::SetCursorPos(ImVec2(buttonLeftPos, buttonTopPos));
  if (ImGui::Button("Quit", buttonSize)) gameManager.requestExit();

  ImGui::GetStyle().Colors[ImGuiCol_Button] = originalButtonStyle;
  ImGui::PopFont();

  ImGui::End();
  ImGui::PopStyleVar();

  // ----------------------------------------------------------------------------

  // Render game version
  std::string version = "v" OPENHOI_GAME_VERSION;
  ImVec2 textSize = ImGui::CalcTextSize(version.c_str());
  ImGui::SetNextWindowPos(
      ImVec2(windowWidth - textSize.x - 10, windowHeight - textSize.y - 10),
      ImGuiCond_Always);
  ImGui::SetNextWindowSize(textSize, ImGuiCond_Always);
  ImGui::SetNextWindowContentSize(textSize);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("Game Menu Version", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground);
  ImGui::Text(version.c_str(), nullptr);
  ImGui::End();
  ImGui::PopStyleVar();
}

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