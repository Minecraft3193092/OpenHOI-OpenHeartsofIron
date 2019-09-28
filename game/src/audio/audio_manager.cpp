// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#include "audio/audio_manager.hpp"

#include <OgreLogManager.h>

namespace openhoi {

// Initializes the audio manager
AudioManager::AudioManager() {
  // Try to open the default device. Returns NULL in case no device was found
  ALCdevice* tmpDev = alcOpenDevice(NULL);

  if (tmpDev) {
    // Get default device name
    std::string defaultDeviceName =
        std::string(alcGetString(tmpDev, ALC_ALL_DEVICES_SPECIFIER));

#ifdef _DEBUG
    Ogre::LogManager::getSingletonPtr()->logMessage(
        "*** Possible audio devices ***");
#endif

    // At least one device could be opened. So now, iterate over all known
    // devices
    if (alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATION_EXT") ==
        AL_TRUE) {
      const char* devs =
          (const char*)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
      while (std::string(devs).size()) {
        // Get device name
        std::string deviceName = devs;
        devs += deviceName.size() + 1;

        // Create audio device and add it to list
        std::shared_ptr<AudioDevice> dev = addAudioDevice(deviceName);

#ifdef _DEBUG
        Ogre::LogManager::getSingletonPtr()->logMessage(" [*] " +
                                                        dev->getFriendlyName());
#endif

        // Set the default device
        if (deviceName == defaultDeviceName) {
          selectedDevice = dev;

#ifdef _DEBUG
          Ogre::LogManager::getSingletonPtr()->logMessage(
              "   -> Default device, pre-selected");
#endif
        }
      }

      // If we could somehow not find any device, kill the one we have created
      // previously
      if (!selectedDevice) {
        alcCloseDevice(tmpDev);
        tmpDev = nullptr;
      }
    } else {
      // No device iteration possible. Use only the default device
      selectedDevice = addAudioDevice(defaultDeviceName);

#ifdef _DEBUG
      Ogre::LogManager::getSingletonPtr()->logMessage(
          " [*] " + selectedDevice->getFriendlyName());
      Ogre::LogManager::getSingletonPtr()->logMessage(
          "   -> Default device, pre-selected");
#endif
    }

    // Check if the temporary device is still there. If yes, this means for us
    // that we will reuse it now for the game. Thus. we set the main device
    // pointer to `tmpDev` and then create the OpenAL context
    if (tmpDev) {
      // Set device pointer
      device = tmpDev;

      // Create context
      if (!createContext()) {
        delete device;
        device = nullptr;
      }
    }
  } else {
    Ogre::LogManager::getSingletonPtr()->logMessage(
        "*** No audio devices found! ***", Ogre::LogMessageLevel::LML_CRITICAL);
  }
}

// Destroys the audio manager
AudioManager::~AudioManager() {
  if (context) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
  }
  if (device) alcCloseDevice(device);
}

// Add audio device to list of possible devices. Returns the newly added device
std::shared_ptr<AudioDevice> AudioManager::addAudioDevice(std::string name) {
  // Create audio device and add it to list
  std::shared_ptr<AudioDevice> dev(new AudioDevice(name));
  devices.push_back(dev);

  // Return device
  return dev;
}

// Create OpenAL context from the current device
bool AudioManager::createContext() {
  Ogre::LogManager::getSingletonPtr()->logMessage("Creating audio context");

  // Create context
  context = alcCreateContext(device, NULL);

  // Validate context
  if (context) {
    // Set distance attenuation model
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    // Set listener gain
    alListenerf(AL_GAIN, (ALfloat)1.0f);

    // Log and return success
    Ogre::LogManager::getSingletonPtr()->logMessage("Audio context created");
    return true;
  } else {
    // Log and return error
    Ogre::LogManager::getSingletonPtr()->logMessage(
        "Audio context could not be created",
        Ogre::LogMessageLevel::LML_CRITICAL);
    return false;
  }
}

// Sets the current/active device
void AudioManager::setDevice(std::shared_ptr<AudioDevice> device) {
  Ogre::LogManager::getSingletonPtr()->logMessage(
      "*** Changing audio device ***");

  if (this->device) {
    // Destroy previous device
    Ogre::LogManager::getSingletonPtr()->logMessage(
        "Destroying current device");
    alcCloseDevice(this->device);
    this->device = nullptr;
  }

  if (device) {
    // Open device
    Ogre::LogManager::getSingletonPtr()->logMessage(
        "Opening new audio device \"" + device->getFriendlyName() + "\"");

    this->device =
        alcOpenDevice((const ALCchar*)selectedDevice->getName().c_str());
    if (this->device) {
      // Create context
      if (!createContext()) {
        delete this->device;
        this->device = nullptr;
      }
    }
  } else {
    // Empty device
    this->device = nullptr;
  }
}

// Gets the current device
std::shared_ptr<AudioDevice> const& AudioManager::getDevice() const {
  return selectedDevice;
}

}  // namespace openhoi