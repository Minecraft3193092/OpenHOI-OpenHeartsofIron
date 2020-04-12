// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <imgui.h>

#include <map>
#include <optional>
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

  void setOptions(std::map<KeyType, std::string> options);

  std::optional<std::pair<KeyType, std::string>> const& getSelectedEntry()
      const;

  std::optional<KeyType> const& getSelectedValue() const;

  void setSelectedEntry(KeyType selectedEntry);

  void setSelectedEntry(std::optional<std::pair<KeyType, std::string>> selectedEntry);

 private:
  std::map<KeyType, std::string> options;
  std::optional<std::pair<KeyType, std::string>> selectedEntry;
};

}  // namespace openhoi