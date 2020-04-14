// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#include "gui/imgui_helper.hpp"

#include <imgui_internal.h>

#include "game_manager.hpp"

namespace openhoi {

#define SOUND_CLICK "click"
#define SOUND_HOVER "hover"

// Returns if the item was hovered in the last frame
bool ImGuiHelper::isItemHoveredLastFrame() {
  ImGuiContext* ctx = ImGui::GetCurrentContext();
  if (ctx->HoveredIdPreviousFrame)
    return ctx->HoveredIdPreviousFrame == ctx->CurrentWindow->DC.LastItemId;
  return false;
}

// openhoi wrapper for ImGui::Button();
bool ImGuiHelper::Button(const char* label,
                         const ImVec2& size /* = ImVec2(0, 0) */) {
  bool clicked = ImGui::Button(label, size);
  if (ImGui::IsItemClicked()) {
    GameManager::getInstance().getAudioManager()->playSound(SOUND_CLICK);
  } else if (ImGui::IsItemHovered() && !isItemHoveredLastFrame()) {
    GameManager::getInstance().getAudioManager()->playSound(SOUND_HOVER);
  }
  return clicked;
}

// openhoi wrapper for ImGui::SliderInt()
bool ImGuiHelper::SliderInt(const char* label, int* v, int v_min, int v_max,
                            const char* format /* = "%d" */) {
  bool clicked = ImGui::SliderInt(label, v, v_min, v_max, format);
  if (ImGui::IsItemDeactivatedAfterEdit()) {
    GameManager::getInstance().getAudioManager()->playSound(SOUND_CLICK);
  }
  return clicked;
}


// openhoi wrapper for ImGui::BeginCombo() / ImGui::EndCombo() etc.
template <typename KeyType>
bool ImGuiHelper::Combo(const char* label,
                        std::shared_ptr<ComboBox<KeyType>> comboBox,
                        ImGuiComboFlags flags /* = 0 */) {
  std::optional<std::pair<KeyType, std::string>> selectedEntry = comboBox->getSelectedEntry();
  const char* selectedEntryText;
  if (selectedEntry.has_value()) {
    selectedEntryText = selectedEntry.value().second.c_str();
  } else {
    selectedEntryText = nullptr;
  }

  bool opened = ImGui::BeginCombo(label, selectedEntryText, flags);
  if (ImGui::IsItemClicked()) {
    GameManager::getInstance().getAudioManager()->playSound(SOUND_CLICK);
  }

  if (opened) {
    for (std::pair<KeyType, std::string> const& option :
         comboBox->getOptions()) {
      bool isSelected =
          selectedEntry.has_value() && selectedEntry.value() == option;
      if (ImGui::Selectable(option.second.c_str(), isSelected)) {
        GameManager::getInstance().getAudioManager()->playSound(SOUND_CLICK);
        comboBox->setSelectedEntry(option);
        selectedEntry = option;
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus(); 
      }
    }
    ImGui::EndCombo();
  }

  return opened;
}

// Explicit Combo function template instantiation
template bool ImGuiHelper::Combo<std::shared_ptr<AudioDevice>>(
    const char*, std::shared_ptr<ComboBox<std::shared_ptr<AudioDevice>>>,
    ImGuiComboFlags);

}  // namespace openhoi

/*
template <class KeyType>
const char* ComboBox<KeyType>::getSelectedEntryNameForImGui() const {
  if (selectedEntry.has_value()) {
    return selectedEntry.value().second.c_str();
  }
  return nullptr;
}

template <class KeyType>
const char* ComboBox<KeyType>::getEntriesForImGui() const {
  std::vector<std::string> entries(options.size());
  for (auto const& entry : options) {
    entries.push_back(entry.second);
  }
  return nullptr;
}*/