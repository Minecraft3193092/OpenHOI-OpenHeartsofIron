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

}  // namespace openhoi