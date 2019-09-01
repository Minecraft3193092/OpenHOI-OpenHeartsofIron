// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "gui/debug_console.hpp"
#include "gui/gui_manager.hpp"

namespace openhoi {

// Creates the debug console
DebugConsole::DebugConsole() {
  /// xxx
}

// Destroys the debug console
DebugConsole::~DebugConsole() {
  // xxxx
}

// Toggle debug console
void DebugConsole::toggle() { showConsole = !showConsole; }

// Add log to debug console
void DebugConsole::addLog(const char* fmt, ...) {
  // xxxx
}

// Draw the debug console (inherited from GuiElement)
void DebugConsole::draw() {
  GuiManager& guiManager = GuiManager::getInstance();

  ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Debug console", &showConsole,
                    ImGuiWindowFlags_NoDocking)) {
    ImGui::End();
    return;
  }

  ImGui::TextWrapped(
      "This example implements a console with basic coloring, completion and "
      "history. A more elaborate implementation may want to store entries "
      "along with extra data such as timestamp, emitter, etc.");
  ImGui::TextWrapped(
      "Enter 'HELP' for help, press TAB to use text completion.");

  ImGui::End();
}

}  // namespace openhoi