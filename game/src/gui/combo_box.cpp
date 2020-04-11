// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#include "gui/combo_box.hpp"

namespace openhoi {

template <class KeyType>
ComboBox<KeyType>::ComboBox(){
  setSelectedEntry(nullptr);
};

template <class KeyType>
ComboBox<KeyType>::ComboBox(std::map<KeyType, std::string> options) : options(options) {
  setSelectedEntry(nullptr);
}

template <class KeyType>
ComboBox<KeyType>::ComboBox(std::map<KeyType, std::string> options, KeyType selectedEntry)
    : options(options) {
  setSelectedEntry(selectedEntry);
}

template <class KeyType>
std::map<KeyType, std::string> const& ComboBox<KeyType>::getOptions() const {
  return options;
}

template <class KeyType>
void ComboBox<KeyType>::setOptions(std::map<KeyType, std::string> options) const {
  this->options = options;
}

template <class KeyType>
std::pair<KeyType, std::string> const& ComboBox<KeyType>::getSelectedEntry()
    const {
  return nullptr;
}

template <class KeyType>
void ComboBox<KeyType>::setSelectedEntry(KeyType selectedEntry) const {
  std::pair<KeyType, std::string> newSelected = nullptr;
  for (auto const& entry : options) {
    if (entry.first == selectedEntry) {
      newSelected = entry;
      break;
    }
  }
  this->selectedEntry = newSelected;
}

template <class KeyType>
const char* ComboBox<KeyType>::getSelectedEntryNameForImGui() const {
  if (selectedEntry) {
    return selectedEntry.second.c_str();
  }
  return nullptr;
}

template <class KeyType>
const char* ComboBox<KeyType>::getEntriesForImGui() const {
  std::string[options.size()] entries;
  int i = 0;
  for (const auto& [std::ignore, value] : options) {
    entries[i++] = key;
  }
  return entries.c_str();
}

}  // namespace openhoi