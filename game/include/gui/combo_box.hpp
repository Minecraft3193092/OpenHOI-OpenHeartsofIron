// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <imgui.h>
#include <map>
#include <string>

namespace openhoi {

// Combobox to be used in combination with ImGuiHelper::Combo()
template <class KeyType>
class ComboBox {
 public:
  ComboBox();
  ComboBox(std::map<KeyType, std::string> options);
  ComboBox(std::map<KeyType, std::string> options, KeyType selectedEntry);

  std::map<KeyType, std::string> const& getOptions() const;

  void setOptions(std::map<KeyType, std::string> options) const;

  std::pair<KeyType, std::string> const& getSelectedEntry() const;

  void setSelectedEntry(KeyType selectedEntry) const;

  const char* getSelectedEntryNameForImGui() const;

  const char* getEntriesForImGui() const;

 private:
  std::map<KeyType, std::string> options;
  std::pair<KeyType, std::string> selectedEntry;
};

}  // namespace openhoi