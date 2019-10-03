// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#include "audio/audio_manager.hpp"

#include <hoibase/file/file_access.hpp>

#include <OgreLogManager.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <stb_vorbis.c>

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

  Ogre::LogManager::getSingletonPtr()->logMessage(
      "*** Audio manager unloaded ***");
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
    // Make it the current context
    alcMakeContextCurrent(context);

    // Set distance attenuation model
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

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
        "Opening new audio device '" + device->getFriendlyName() + "'");

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

// Gets all possible devices
std::vector<std::shared_ptr<AudioDevice>> const&
AudioManager::getPossibleDevices() const {
  return devices;
}

// Loads all sound files in the provided directory into memory and thus makes it
// playable
void AudioManager::loadAll(filesystem::path directory) {
  // Iterate through all files in directory
  for (const auto& entry : filesystem::directory_iterator(directory)) {
    filesystem::path file = entry.path();
    if (!filesystem::is_regular_file(file)) continue;

    // Check for OGG file type
    std::string extension =
        boost::algorithm::to_lower_copy(file.extension().u8string());
    if (extension != ".ogg") continue;

    // Read file and check result
    unsigned char* data = nullptr;
    int fileSize = (int)FileAccess::readFile(file, &data);
    if (fileSize < 1) continue;

    // Clear OpenAL error flag
    alGetError();

    // Open Vorbis stream and decode data
    auto* vorbis = stb_vorbis_open_memory(data, fileSize, NULL, NULL);
    if (!vorbis) {
      Ogre::LogManager::getSingletonPtr()->logMessage(
          (boost::format("Audio file '%s' could not be decoded") %
           file.filename().u8string())
              .str(),
          Ogre::LogMessageLevel::LML_CRITICAL);

      // Free the data malloc'd in FileAccess::readFile!
      free(data);

      continue;
    }

    // Get audio information
    auto info = stb_vorbis_get_info(vorbis);
    ALenum format = info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    int sampleRate = info.sample_rate;
    int size = stb_vorbis_stream_length_in_samples(vorbis) * info.channels;
    short* samples = new short[size];
    stb_vorbis_get_samples_short_interleaved(vorbis, info.channels, samples,
                                             size);
    size *= sizeof(short);

    // Close Vorbis stream
    stb_vorbis_close(vorbis);

    // Free the data malloc'd in FileAccess::readFile!
    free(data);

    // Generate buffers
    ALuint buffer;
    ALenum error;
    alGenBuffers(1, &buffer);
    if ((error = alGetError()) != AL_NO_ERROR) {
      Ogre::LogManager::getSingletonPtr()->logMessage(
          (boost::format("Unable to generate OpenAL buffer for file '%s': %d") %
           file.filename().u8string() % error)
              .str(),
          Ogre::LogMessageLevel::LML_CRITICAL);
      continue;
    }
    alBufferData(buffer, format, samples, size, sampleRate);
    if ((error = alGetError()) != AL_NO_ERROR) {
      Ogre::LogManager::getSingletonPtr()->logMessage(
          (boost::format(
               "Unable to copy data into OpenAL buffer for file '%s': %d") %
           file.filename().u8string() % error)
              .str(),
          Ogre::LogMessageLevel::LML_CRITICAL);
      alDeleteBuffers(1, &buffer);
      continue;
    }

    Ogre::LogManager::getSingletonPtr()->logMessage(
        (boost::format("Audio file '%s' loaded") % file.filename().u8string())
            .str());
  }
}

}  // namespace openhoi