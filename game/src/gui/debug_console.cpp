// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "gui/debug_console.hpp"
#include "gui/gui_manager.hpp"

#include <hoibase/helper/unique_id.hpp>

namespace openhoi {

// Creates the debug console
DebugConsole::DebugConsole() {
  // Do not show the console by default
  showConsole = false;

  // create empty input buffer
  memset(inputBuffer, 0, sizeof(inputBuffer));

  // Reset history pos
  historyPos = -1;

  addLog("Hey hey");
}

// Destroys the debug console
DebugConsole::~DebugConsole() {
  // Clear log
  clearLog();
}

// Toggle debug console
void DebugConsole::toggle() { showConsole = !showConsole; }

// Add log to debug console
void DebugConsole::addLog(const char* fmt, ...) {
  char buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, IM_ARRAYSIZE(buffer), fmt, args);
  buffer[IM_ARRAYSIZE(buffer) - 1] = 0;
  va_end(args);
  items.push_back(strdup(buffer));
}

// Clear debug console log
void DebugConsole::clearLog() {
  // xxxx
}

// Draw the debug console (inherited from GuiElement)
void DebugConsole::draw() {
  if (!showConsole) return;

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
  ImGui::Separator();

  filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
  ImGui::Separator();

  const float footerHeightToReserve =
      ImGui::GetStyle().ItemSpacing.y +
      ImGui::GetFrameHeightWithSpacing();  // 1 separator, 1 input text
  ImGui::BeginChild(
      UniqueID::generate().c_str(), ImVec2(0, -footerHeightToReserve), false,
      ImGuiWindowFlags_HorizontalScrollbar);  // Leave room for 1 separator + 1
                                              // input text
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::Selectable("Clear")) clearLog();
    ImGui::EndPopup();
  }

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                      ImVec2(4, 1));  // Tighten spacing

  for (int i = 0; static_cast<std::vector<std::string>::size_type>(i) < items.size(); i++) {
    const char* item = items[i].c_str();
    if (!filter.PassFilter(item)) continue;

    bool popColor = false;
    if (strstr(item, "[error]")) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
      popColor = true;
    } else if (strncmp(item, "# ", 2) == 0) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
      popColor = true;
    }
    ImGui::TextUnformatted(item);
    if (popColor) ImGui::PopStyleColor();
  }

  // Scroll to bottom
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    ImGui::SetScrollHereY(1.0f);

  ImGui::PopStyleVar();
  ImGui::EndChild();
  ImGui::Separator();

  // Command-line
  bool reclaimFocus = false;
  if (ImGui::InputText(
          "Input", inputBuffer, IM_ARRAYSIZE(inputBuffer),
          ImGuiInputTextFlags_EnterReturnsTrue |
              ImGuiInputTextFlags_CallbackCompletion |
              ImGuiInputTextFlags_CallbackHistory,
          [](ImGuiInputTextCallbackData* data) {
            DebugConsole* console = (DebugConsole*)data->UserData;
            return console->textEditCallback(data);
          },
          (void*)this)) {
    std::string command = inputBuffer;
    // if (!command.empty()) ExecCommand(s);
    reclaimFocus = true;
  }

  // Auto-focus on window apparition
  ImGui::SetItemDefaultFocus();
  if (reclaimFocus)
    ImGui::SetKeyboardFocusHere(-1);  // Auto focus previous widget

  ImGui::End();
}

// Command line input callback
int DebugConsole::textEditCallback(ImGuiInputTextCallbackData* data) {
  switch (data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion: {
      // ---> Command line text completion

      // Locate beginning of current word
      const char* word_end = data->Buf + data->CursorPos;
      const char* word_start = word_end;
      while (word_start > data->Buf) {
        const char c = word_start[-1];
        if (c == ' ' || c == '\t' || c == ',' || c == ';') break;
        word_start--;
      }

      // Build a list of candidates
      ImVector<const char*> candidates;
      /*for (int i = 0; i < Commands.Size; i++)
        if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) ==
            0)
          candidates.push_back(Commands[i]);
          */
      if (candidates.Size == 0) {
        // No match
        addLog("No match for \"%.*s\"!\n", (int)(word_end - word_start),
               word_start);
      } else if (candidates.Size == 1) {
        // Single match. Delete the beginning of the word and replace it
        // entirely so we've got nice casing
        data->DeleteChars((int)(word_start - data->Buf),
                          (int)(word_end - word_start));
        data->InsertChars(data->CursorPos, candidates[0]);
        data->InsertChars(data->CursorPos, " ");
      } else {
        // Multiple matches. Complete as much as we can, so inputing "C" will
        // complete to "CL" and display "CLEAR" and "CLASSIFY"
        int match_len = (int)(word_end - word_start);
        for (;;) {
          int c = 0;
          bool all_candidates_matches = true;
          for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
            if (i == 0)
              c = toupper(candidates[i][match_len]);
            else if (c == 0 || c != toupper(candidates[i][match_len]))
              all_candidates_matches = false;
          if (!all_candidates_matches) break;
          match_len++;
        }

        if (match_len > 0) {
          data->DeleteChars((int)(word_start - data->Buf),
                            (int)(word_end - word_start));
          data->InsertChars(data->CursorPos, candidates[0],
                            candidates[0] + match_len);
        }

        // List matches
        addLog("Possible matches:\n");
        for (int i = 0; i < candidates.Size; i++)
          addLog("- %s\n", candidates[i]);
      }

      break;
    }
    case ImGuiInputTextFlags_CallbackHistory: {
      // --> Command line history

      const int prevHistoryPos = historyPos;
      if (data->EventKey == ImGuiKey_UpArrow) {
        if (historyPos == -1)
          historyPos = history.size() - 1;
        else if (historyPos > 0)
          historyPos--;
      } else if (data->EventKey == ImGuiKey_DownArrow && historyPos != -1 && static_cast<std::vector<std::string>::size_type>(++historyPos) >= history.size()) {
          historyPos = -1;
      }

      if (prevHistoryPos != historyPos) {
        std::string historyStr = historyPos >= 0 ? history[historyPos] : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, historyStr.c_str());
      }
    }
  }
  return 0;
}

}  // namespace openhoi