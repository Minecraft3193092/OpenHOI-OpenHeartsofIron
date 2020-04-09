// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#include "gui/options_dialog.hpp"

#include <OgreRenderWindow.h>

#include "game_manager.hpp"
#include "gui/imgui_helper.hpp"
#include "options.hpp"

namespace openhoi {

#define OPTIONS_DIALOG_WIDTH 600
#define OPTIONS_DIALOG_HEIGHT 450

// Creates the options dialog
OptionsDialog::OptionsDialog() {
  // Options dialog is not visible by default
  setVisible(false);
}

// Destroys the options dialog
OptionsDialog::~OptionsDialog() {}

// Draw the options dialog (inherited from GuiElement)
void OptionsDialog::draw() {
  if (!isVisible()) return;

  // Set window size and position
  ImGui::SetNextWindowSize(ImVec2(OPTIONS_DIALOG_WIDTH, OPTIONS_DIALOG_HEIGHT),
                           ImGuiCond_Appearing);

  GameManager& gameManager = GameManager::getInstance();
  int windowWidth = gameManager.getRenderWindow()->getWidth();
  int windowHeight = gameManager.getRenderWindow()->getHeight();
  ImGui::SetNextWindowPos(ImVec2((windowWidth - OPTIONS_DIALOG_WIDTH) / 2,
                                 (windowHeight - OPTIONS_DIALOG_HEIGHT) / 2),
                          ImGuiCond_Appearing);

  // Get options
  Options* options = gameManager.getOptions();

  // Render window
  ImGui::Begin("Options", &this->visible,
               ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

  if (ImGui::BeginTabBar("OptionsTabs", ImGuiTabBarFlags_None)) {
    if (ImGui::BeginTabItem("Player")) {
      ImGui::Text("This is the player tab!\nblah blah blah blah blah");
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Video")) {
      ImGui::Text("This is the video tab!\nblah blah blah blah blah");
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Audio")) {
      ImGuiHelper::SliderInt("Music volume", &options->musicVolume, 0, 100,
                             "%d %%");
      ImGuiHelper::SliderInt("Effects volume", &options->effectsVolume, 0, 100,
                             "%d %%");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  ImGui::End();
}

}  // namespace openhoi