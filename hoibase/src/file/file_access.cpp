// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/file/file_access.hpp"

#include <boost/format.hpp>

#include "hoibase/helper/os.hpp"

#ifdef OPENHOI_OS_WINDOWS
#  include <KnownFolders.h>
#  include <ShlObj.h>
#  include <wchar.h>
#else
#  include <OgreLogManager.h>
#  include <pwd.h>
#  include <sys/types.h>
#  include <unistd.h>

#  include <boost/algorithm/string.hpp>
#  if defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
#    ifndef _GNU_SOURCE
#      define _GNU_SOURCE
#    endif
#    include <dlfcn.h>
#  endif
#endif
#include <array>
#include <fstream>
#include <stdexcept>

// The game config directory
#ifdef OPENHOI_OS_WINDOWS
#  define OPENHOI_CONFIG_DIRECTORY_NAME "openhoi"
#else
// Starting with "." makes it a hidden folder on Linux etc.
#  define OPENHOI_CONFIG_DIRECTORY_NAME ".openhoi"
#endif

namespace openhoi {

// Cached game config and asset directories, etc.
filesystem::path FileAccess::gameConfigDirectory;
filesystem::path FileAccess::gameAssetRootDirectory;
#if defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
filesystem::path FileAccess::ogrePluginDirectory;
#endif

// Get the current user's home directory. If it cannot be found, an exception
// will be thrown.
filesystem::path FileAccess::getUserHomeDirectory() {
  filesystem::path homeDirectory;

#ifdef OPENHOI_OS_WINDOWS
  // Get user's AppData\Local directory
  PWSTR path = NULL;
  HRESULT hr =
      SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &path);
  if (SUCCEEDED(hr)) homeDirectory = path;
  CoTaskMemFree(path);
#else
  // Get user's home directory
  const char* home = NULL;
  if ((home = getenv("HOME")) == NULL) {
    passwd* pwd = getpwuid(getuid());
    if (pwd) home = pwd->pw_dir;
  }
  if (home != NULL) homeDirectory = home;
#endif

  if (!filesystem::is_directory(homeDirectory))
    throw std::runtime_error("Unable to find user home directory");

  return homeDirectory;
}

// Get the temp directory. If it cannot be found, an exception will be thrown.
filesystem::path FileAccess::getTempDirectory() {
  return filesystem::temp_directory_path();
}

#ifndef OPENHOI_OS_WINDOWS
// Get the log directory. If it cannot be found, an exception will be thrown.
filesystem::path FileAccess::getLogDirectory() {
  filesystem::path logDirectory = filesystem::path("/var/log");

  if (!filesystem::is_directory(logDirectory))
    throw std::runtime_error("Unable to find log directory");

  return logDirectory;
}
#endif

// Get the game config directory. If it does not exist, this function also tries
// to create it. In case something bad happens, an exception will be thrown.
filesystem::path FileAccess::getUserGameConfigDirectory() {
  // Check if we have already calculated the game config directory
  if (FileAccess::gameConfigDirectory.empty()) {
    // Yeah, this is not thread-safe..

    // Get home directory
    filesystem::path homeDirectory = getUserHomeDirectory();

    // Get config directory which is placed inside the home directory
    filesystem::path configDirectory =
        homeDirectory / filesystem::path(OPENHOI_CONFIG_DIRECTORY_NAME);

    // Create the folder in case it does not exist
    if (!filesystem::is_directory(configDirectory)) {
      if (filesystem::exists(configDirectory)) {
        // Delete the file named like the desired config directory
        // This logic exists just to be sure - this code branch should never be
        // active..
        filesystem::remove(configDirectory);
      }

      // Create the directory
      if (!filesystem::create_directory(configDirectory))
        throw std::runtime_error(
            (boost::format("Unable to find create game config directory '%s'") %
             configDirectory.u8string())
                .str());

#ifdef OPENHOI_OS_WINDOWS
      // Convert config directory path to LPSTR
      const int size = WideCharToMultiByte(CP_UTF8, 0, configDirectory.c_str(),
                                           -1, NULL, 0, 0, NULL);
      std::vector<char> buf(size);
      WideCharToMultiByte(CP_UTF8, 0, configDirectory.c_str(), -1, &buf[0],
                          size, 0, NULL);

      // Make directory hidden
      SetFileAttributes(std::string(&buf[0]).c_str(), FILE_ATTRIBUTE_HIDDEN);
#endif
    }

    FileAccess::gameConfigDirectory = configDirectory;
  }

  // Return the directory path
  return FileAccess::gameConfigDirectory;
}

// Get the game asset root directory. If it cannot be found, an exception will
// be thrown.
filesystem::path FileAccess::getAssetRootDirectory() {
  // Check if we have already fetched the game asset directory
  if (FileAccess::gameAssetRootDirectory.empty()) {
    // Yeah, this is not thread-safe..

#ifdef OPENHOI_OS_MACOS
    char path[1024];

    // Get main bundle
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    CFStringRef cfStringRef =
        CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);

    FileAccess::gameAssetRootDirectory =
        filesystem::path(path) / filesystem::path(assets);
#else
#  ifdef OPENHOI_OS_WINDOWS
    std::array<std::string, 5> possibleResourcePaths = {
        "", "dist", "..\\dist", "..\\..\\dist", "..\\..\\..\\dist"};
#  else
#    ifdef _DEBUG
    std::array<std::string, 8> possibleResourcePaths = {
        "",
        "dist",
        "../dist",
        "../../dist",
        "../../../dist",
        "/usr/local/share/openhoi/",
        "/usr/share/openhoi/",
        "/opt/openhoi"};
#    else
    std::array<std::string, 8> possibleResourcePaths = {
        "",
        "/usr/local/share/openhoi/",
        "/usr/share/openhoi/",
        "/opt/openhoi",
        "dist",
        "../dist",
        "../../dist",
        "../../../dist"};
#    endif
#  endif

    filesystem::path possibleDir;
    for (std::string& dir : possibleResourcePaths) {
      possibleDir = filesystem::path(dir);
      if (filesystem::is_directory(possibleDir)) {
        FileAccess::gameAssetRootDirectory = filesystem::canonical(possibleDir);
        break;
      }
    }
#endif

    if (!filesystem::is_directory(FileAccess::gameAssetRootDirectory))
      throw std::runtime_error("Unable to find game asset root directory");
  }

  return FileAccess::gameAssetRootDirectory;
}

// Custom fopen override for Windows
FILE* FileAccess::fopen(char const* fileName, char const* mode) {
#ifdef OPENHOI_OS_WINDOWS
  FILE* fp;
  errno_t err;
  err = fopen_s(&fp, fileName, mode);
  if (err == 0) {
    // No error, return file pointer
    return fp;
  } else {
    // File could not be opened, return nullptr
    return nullptr;
  }
#else
  return ::fopen(fileName, mode);
#endif
}

// Custom fopen override for Windows
FILE* FileAccess::fopen(filesystem::path file, char const* mode) {
  return FileAccess::fopen(file.c_str(), mode);
}

// Read the provided file to the provided data pointer. The file length is
// returned or -1 in case the file could not be read.
long FileAccess::readFile(filesystem::path file, unsigned char** data) {
  // Check if it is an regular file
  if (!filesystem::is_regular_file(file)) return -1;

  // Get file path as char pointer
  std::string filePathStr = file.string();
  const char* filePath = filePathStr.c_str();

  // Try to open file
  auto closeFile = [](FILE* f) { fclose(f); };
  auto holder = std::unique_ptr<FILE, decltype(closeFile)>(
      FileAccess::fopen(filePath, "rb"), closeFile);
  if (!holder) return -1;

  // Get opened file
  FILE* fp = holder.get();

  // Get file size
  uintmax_t size;
  try {
    size = filesystem::file_size(file);
  } catch (filesystem::filesystem_error&) {
    return -1;
  }

  // Allocate memory for file
  *data = (unsigned char*)malloc(size);

  // Read file into data pointer
  size_t result = fread(*data, 1, size, fp);
  if (result != size) return -1;

  // Return the data length
  return (long)size;
}

// Gets the OGRE plugin directory. In case the plugins should be located
// relatively to the executables, an empty path is returned.
filesystem::path FileAccess::getOgrePluginDirectory() {
#if defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
  // Check if we have already fetched the OGRE plugin directory
  if (FileAccess::ogrePluginDirectory.empty()) {
    // Yeah, this is not thread-safe..

    // Get path where our used libOgreMain resides. We will then use this base
    // path in order to locate the directory where the OGRE plugins are stored:
    Dl_info dlInfo;
    dladdr((void*)Ogre::LogManager::getSingletonPtr, &dlInfo);

    // Create filesystem::path from dlInfo.dli_fname.
    // dlInfo.dli_fname can be something like:
    //  - /usr/local/lib/libOgreMain.so.1.12.2
    //  - /usr/lib/x86_64-linux-gnu/libOgreMain.so
    //  - etc.
    filesystem::path libFilePath = filesystem::path(dlInfo.dli_fname);

    // Extract the directory where the file "dlInfo.dli_fname" is stored at
    filesystem::path libDir = libFilePath.parent_path();

    // Build the expected file name based on the dlInfo.dli_fname. We want to
    // use the same extension (e.g. ".so.1.12.2")
    filesystem::path fileNameOnly = libFilePath.filename();
    size_t dotIndex = fileNameOnly.u8string().find(".");
    std::string extension;
    if (dotIndex != std::string::npos)
      extension = fileNameOnly.u8string().substr(dotIndex);
    std::string expectedPluginFileName =
        std::string(OGRE_PLUGIN_STBI) + extension;

    // Build a list of possible plugin library directories
    std::vector<std::string> possiblePluginDirectories;
    possiblePluginDirectories.push_back(libDir.u8string());
    filesystem::directory_iterator endItr;
    for (filesystem::directory_iterator itr(libDir); itr != endItr; ++itr) {
      filesystem::path path = itr->path();
      if (filesystem::is_directory(itr->status()) &&
          boost::algorithm::to_lower_copy(path.filename().u8string())
                  .find("ogre", 0) == 0) {
        possiblePluginDirectories.push_back(path.u8string());
      }
    }

    // Search for the right plugin library directory
    filesystem::path possibleFileName;
    for (std::string& dir : possiblePluginDirectories) {
      possibleFileName = filesystem::path(dir) / expectedPluginFileName;
      if (filesystem::exists(possibleFileName) &&
          !filesystem::is_directory(possibleFileName)) {
        FileAccess::ogrePluginDirectory =
            filesystem::canonical(possibleFileName).parent_path();
        break;
      }
    }
  }

  return FileAccess::ogrePluginDirectory;
#else
  // Use an empty path because all libs are stored in the same directory than
  // our executables
  return filesystem::path();
#endif
}

}  // namespace openhoi
