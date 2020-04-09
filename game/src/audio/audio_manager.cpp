// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#include "audio/audio_manager.hpp"

#include <OgreLogManager.h>

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <hoibase/file/file_access.hpp>
#include <random>
#include <stb_vorbis.c>
#include <thread>

#include "game_manager.hpp"

namespace openhoi {

// Initializes the audio manager
AudioManager::AudioManager()
    : playingBackgroundMusic(0), device(0), context(0) {
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
        alcCloseDevice(device);
        device = nullptr;
      }
    }
  } else {
    Ogre::LogManager::getSingletonPtr()->logMessage(
        "*** No audio devices found! ***", Ogre::LogMessageLevel::LML_CRITICAL);
  }

  // Initialize iterators
  backgroundMusicIt = backgroundMusic.begin();
}

// Destroys the audio manager
AudioManager::~AudioManager() {
  if (context) {
    for (const auto& audioSource : playing) {
      alDeleteSources(1, &audioSource);
    }
    if (playingBackgroundMusic) alDeleteSources(1, &playingBackgroundMusic);

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
        alcCloseDevice(this->device);
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

// Starts the loading of background music
void AudioManager::loadBackgroundMusicAsync(filesystem::path directory) {
  // Invoke the function `loadAndPlayBackgroundMusic` in a separate thread
  std::thread{&AudioManager::loadAndPlayBackgroundMusic, this, directory}
      .detach();
}

// Loads all background audio tracks in a separate track and starts to play them
// as soon as the first file was fully loaded
void AudioManager::loadAndPlayBackgroundMusic(filesystem::path directory) {
  // Iterate through all files in directory
  std::vector<filesystem::path> files;
  for (const auto& entry : filesystem::directory_iterator(directory)) {
    files.push_back(entry.path());
  }

  // Shuffle file list
  std::random_device rd;
  std::mt19937 twister(rd());
  std::shuffle(files.begin(), files.end(), twister);

  // Move smallest file to the beginning to decrease loading time for music on
  // startup
  auto smallestFileSize = UINTMAX_MAX;
  for (auto it = files.begin(); it != files.end(); ++it) {
    auto fileSize = filesystem::file_size(*it);
    if (fileSize < smallestFileSize) smallestFileSize = fileSize;
  }
  for (auto it = files.begin(); it != files.end(); ++it) {
    auto file = *it;
    auto fileSize = filesystem::file_size(file);
    if (fileSize <= smallestFileSize && it != files.begin()) {
      files.erase(it);
      files.insert(files.begin(), file);
      break;
    }
  }
  // Load all music files
  for (const auto& file : files) {
    auto soundPtr = loadSound(file);
    if (soundPtr) backgroundMusic.insert({soundPtr->getFileName(), soundPtr});
  }
}

// Loads all audio effect files found in the the provided directory into memory
// and thus makes them playable
void AudioManager::loadEffects(filesystem::path directory) {
  // Iterate through all files in directory
  for (const auto& entry : filesystem::directory_iterator(directory)) {
    auto soundPtr = loadSound(entry.path());
    if (soundPtr) effects.insert({soundPtr->getFileName(), soundPtr});
  }
}

// Play the provided sound with the given volume
void AudioManager::playSound(std::shared_ptr<Sound> sound, float volume) {
  // Generate source and play audio from it
  ALuint source = generateSourceAndPlaySound(sound, volume);

  // Add source to list of playing sources
  playing.push_back(source);
}

// Play the sound effect (no background music) identified by it's name with the
// given volume
void AudioManager::playSound(std::string sound, float volume) {
  auto soundPtr = effects[sound];
  if (soundPtr) {
    playSound(soundPtr, volume);
  } else {
    Ogre::LogManager::getSingletonPtr()->logMessage(
        (boost::format("Unable to play sound effect '%s' as it was not found") %
         sound)
            .str());
  }
}

// Play the sound effect (no background music) identified by it's name with
// the configured effects volume
void AudioManager::playSound(std::string sound) {
  Options* options = GameManager::getInstance().getOptions();
  playSound(sound, options->getEffectsVolume());
}

// Generate OpenAL source from the provided sound and play it with the given
// volume
ALuint AudioManager::generateSourceAndPlaySound(std::shared_ptr<Sound> sound,
                                                float volume) {
  // Create audio source object
  ALuint source;
  alGenSources(1, &source);
  assert(source > 0);

  alSourcei(source, AL_LOOPING, AL_FALSE);
  alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
  alSourcei(source, AL_BUFFER, sound->getBuffer());
  alSourcef(source, AL_GAIN, volume);

  // Play audio from source
  alSourcePlay(source);

  // Return source
  return source;
}

// Update audio stats (e.g. progress of background music)
void AudioManager::updateStats() {
  Options* options = GameManager::getInstance().getOptions();
  ALint state;

  // Check background audio status
  if (backgroundMusic.size() > 0) {
    bool backgroundMusicPlaying = false;
    if (backgroundMusicIt != backgroundMusic.end()) {
      // Get sound pointer
      std::shared_ptr<Sound> soundPtr = backgroundMusicIt->second;

      // Check sound state (if it is still playing)
      alGetSourcei(playingBackgroundMusic, AL_SOURCE_STATE, &state);
      backgroundMusicPlaying = state == AL_INITIAL || state == AL_PLAYING;
    }

    const float backgroundMusicVolume = options->getMusicVolume();
    if (!backgroundMusicPlaying) {
      // Delete audio source if required
      if (playingBackgroundMusic) {
        alDeleteSources(1, &playingBackgroundMusic);
        playingBackgroundMusic = 0;
      }

      // Switch to next background audio track
      if (backgroundMusicIt != backgroundMusic.end() &&
          std::next(backgroundMusicIt) != backgroundMusic.end())
        backgroundMusicIt++;
      else
        backgroundMusicIt = backgroundMusic.begin();

      // Play the next track
      playingBackgroundMusic = generateSourceAndPlaySound(
          backgroundMusicIt->second, backgroundMusicVolume);
    } else {
      alSourcef(playingBackgroundMusic, AL_GAIN, backgroundMusicVolume);
    }
  }

  // Check for finished audio effects and deletes their sources
  ALuint source;
  const float effectsVolume = options->getEffectsVolume();
  for (auto it = playing.begin(); it != playing.end();) {
    // Get source from iterator
    source = *it;

    // Check sound state (if it is still playing) and handle it
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    if (state == AL_STOPPED) {
      // Audio has stopped, so we can delete it
      alDeleteSources(1, &source);
      it = playing.erase(it);
    } else {
      // Adjust volume if required
      alSourcef(source, AL_GAIN, effectsVolume);

      // Switch to next element in list
      it++;
    }
  }
}

// Loads an sound and returns the sound in case the file was loaded
// successfully
std::shared_ptr<Sound> AudioManager::loadSound(filesystem::path audioFile) {
  if (!filesystem::is_regular_file(audioFile)) return nullptr;

  // Check for OGG file type
  std::string extension =
      boost::algorithm::to_lower_copy(audioFile.extension().u8string());
  if (extension != ".ogg") return nullptr;

  // Read file and check result
  unsigned char* data = nullptr;
  int fileSize = (int)FileAccess::readFile(audioFile, &data);
  if (fileSize < 1) return nullptr;

  // Clear OpenAL error flag
  alGetError();

  // Open Vorbis stream and decode data
  auto* vorbis = stb_vorbis_open_memory(data, fileSize, NULL, NULL);
  if (!vorbis) {
    Ogre::LogManager::getSingletonPtr()->logMessage(
        (boost::format("Audio file '%s' could not be decoded") %
         audioFile.filename().u8string())
            .str(),
        Ogre::LogMessageLevel::LML_CRITICAL);

    // Free the data malloc'd in FileAccess::readFile!
    free(data);

    return nullptr;
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
         audioFile.filename().u8string() % error)
            .str(),
        Ogre::LogMessageLevel::LML_CRITICAL);
    return nullptr;
  }
  alBufferData(buffer, format, samples, size, sampleRate);
  if ((error = alGetError()) != AL_NO_ERROR) {
    Ogre::LogManager::getSingletonPtr()->logMessage(
        (boost::format(
             "Unable to copy data into OpenAL buffer for file '%s': %d") %
         audioFile.filename().u8string() % error)
            .str(),
        Ogre::LogMessageLevel::LML_CRITICAL);
    alDeleteBuffers(1, &buffer);
    return nullptr;
  }

  // Create sound object
  std::string fileName = audioFile.stem().u8string();
  std::shared_ptr<Sound> sound(new Sound(fileName, buffer, samples));

  Ogre::LogManager::getSingletonPtr()->logMessage(
      (boost::format("Audio file '%s' loaded") %
       audioFile.filename().u8string())
          .str());

  // Audio file successfully loaded
  return sound;
}

}  // namespace openhoi