// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#include "gui/options_dialog.hpp"

#include <OgreRenderWindow.h>

#include <boost/locale.hpp>

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
  ImGui::SetNextWindowPos(ImVec2((windowWidth - OPTIONS_DIALOG_WIDTH) / 2.0f,
                                 (windowHeight - OPTIONS_DIALOG_HEIGHT) / 2.0f),
                          ImGuiCond_Appearing);

  // Get options
  std::shared_ptr<Options> options = gameManager.getOptions();

  // Render window
  ImGui::Begin(boost::locale::translate("Options").str().c_str(),
               &this->visible,
               ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

  if (ImGui::BeginTabBar("Options_Tabs", ImGuiTabBarFlags_None)) {
    if (ImGui::BeginTabItem(boost::locale::translate("Player").str().c_str())) {
      ImGui::Text("This is the player tab!\nblah blah blah blah blah");
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(boost::locale::translate("Video").str().c_str())) {
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(boost::locale::translate("Audio").str().c_str())) {
      // Get audio manager
      std::shared_ptr<AudioManager> audioManager =
          gameManager.getAudioManager();

      // Get list of all possible audio devices (+ the selected one) as a GUI
      // combobox
      std::map<std::shared_ptr<AudioDevice>, std::string> possibleDevices;
      for (auto const& dev : audioManager->getPossibleDevices()) {
        possibleDevices.insert({dev, dev->getFriendlyName()});
      }
      auto audioDeviceCombo =
          std::make_shared<ComboBox<std::shared_ptr<AudioDevice>>>(
              possibleDevices, audioManager->getDevice());

      // Render elements
      ImGuiHelper::Combo<std::shared_ptr<AudioDevice>>(
          boost::locale::translate("Audio device").str().c_str(),
          audioDeviceCombo);
      auto selectedAudioDevice = audioDeviceCombo->getSelectedValue();
      if (selectedAudioDevice.has_value()) {
        auto dev = selectedAudioDevice.value();
        if (dev != audioManager->getDevice()) {
          audioManager->setDevice(dev);
          options->setAudioDevice(dev->getFriendlyName());
        }
      } else {
        audioManager->setDevice(nullptr);
        options->setAudioDevice(nullptr);
      }

      ImGuiHelper::SliderInt(
          boost::locale::translate("Music volume").str().c_str(),
          &options->musicVolume, 0, 100, "%d %%");
      ImGuiHelper::SliderInt(
          boost::locale::translate("Effects volume").str().c_str(),
          &options->effectsVolume, 0, 100, "%d %%");

      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  ImGui::End();
}

}  // namespace openhoi