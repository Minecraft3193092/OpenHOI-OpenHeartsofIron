// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <imgui.h>
#include <map>

namespace openhoi {

// ImGui helper
class ImGuiHelper final {
 public:
  // openhoi wrapper for ImGui::Button();
  static bool Button(const char* label, const ImVec2& size = ImVec2(0, 0));

  // openhoi wrapper for ImGui::SliderInt()
  static bool SliderInt(const char* label, int* v, int v_min, int v_max,
                        const char* format = "%d");

  // openhoi wrapper for ImGui::BeginCombo()
  static bool BeginCombo(const char* label, const char* preview_value,
                         ImGuiComboFlags flags = 0);

 private:
  // Returns if the item was hovered in the last frame
  static bool isItemHoveredLastFrame();
};

}  // namespace openhoi