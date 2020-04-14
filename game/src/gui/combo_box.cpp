// Copyright 2020 the openhoi authors. See COPYING.md for legal info.

#include "gui/combo_box.hpp"

#include <memory>
#include <vector>

#include "audio/audio_device.hpp"

namespace openhoi {

template <class KeyType>
ComboBox<KeyType>::ComboBox() {
  setSelectedEntry(std::nullopt);
}

template <class KeyType>
ComboBox<KeyType>::ComboBox(std::map<KeyType, std::string> options)
    : options(options) {
  setSelectedEntry(std::nullopt);
}

template <class KeyType>
ComboBox<KeyType>::ComboBox(std::map<KeyType, std::string> options,
                            KeyType selectedEntry)
    : options(options) {
  setSelectedEntry(selectedEntry);
}

template <class KeyType>
std::map<KeyType, std::string> const& ComboBox<KeyType>::getOptions() const {
  return options;
}

template <class KeyType>
void ComboBox<KeyType>::setOptions(std::map<KeyType, std::string> options) {
  this->options = options;
}

template <class KeyType>
std::optional<std::pair<KeyType, std::string>> const&
ComboBox<KeyType>::getSelectedEntry() const {
  return this->selectedEntry;
}

template <class KeyType>
std::optional<KeyType> ComboBox<KeyType>::getSelectedValue() {
  std::optional<KeyType> selected = std::nullopt;
  if (this->selectedEntry.has_value()) {
    selected = this->selectedEntry.value().first;
  }
  return selected;
}

template <class KeyType>
void ComboBox<KeyType>::setSelectedEntry(KeyType selectedEntry) {
  std::optional<std::pair<KeyType, std::string>> newSelected = std::nullopt;
  for (std::pair<KeyType, std::string> const& entry : options) {
    if (entry.first == selectedEntry) {
      newSelected = std::optional<std::pair<KeyType, std::string>>(entry);
      break;
    }
  }
  this->selectedEntry = newSelected;
}

template <class KeyType>
void ComboBox<KeyType>::setSelectedEntry(
    std::optional<std::pair<KeyType, std::string>> selectedEntry) {
  std::optional<std::pair<KeyType, std::string>> newSelected = std::nullopt;
  for (std::pair<KeyType, std::string> const& entry : options) {
    if (entry == selectedEntry) {
      newSelected = std::optional<std::pair<KeyType, std::string>>(entry);
      break;
    }
  }
  this->selectedEntry = newSelected;
}

// Explicit ComboBox template instantiation
template class ComboBox<std::shared_ptr<AudioDevice>>;

}  // namespace openhoi