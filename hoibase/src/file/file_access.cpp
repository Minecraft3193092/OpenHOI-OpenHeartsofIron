// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/file/file_access.hpp"
#include "hoibase/helper/os.hpp"

#include <OgreBuildSettings.h>
#include <boost/format.hpp>

#ifdef OPENHOI_OS_WINDOWS
#  include <KnownFolders.h>
#  include <ShlObj.h>
#  include <wchar.h>
#else
#  include <pwd.h>
#  include <sys/types.h>
#  include <unistd.h>
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

// The game config directory. Starting with "." makes it a hidden folder on
// Linux etc.
#define OPENHOI_CONFIG_DIRECTORY_NAME ".openhoi"

namespace openhoi {

// Cached game config and asset directories, etc.
filesystem::path FileAccess::gameConfigDirectory;
filesystem::path FileAccess::gameAssetRootDirectory;
filesystem::path FileAccess::ogrePluginDirectory;

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
            (boost::format(
                 "Unable to find create game config directory \"%s\"") %
             configDirectory.u8string())
                .str());

#if OPENHOI_OS_WINDOWS
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

#ifdef OPENHOI_OS_APPLE
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
#    ifdef _DEBUG
    std::array<std::string, 5> possibleResourcePaths = {
        "", "dist", "..\\dist", "..\\..\\dist", "..\\..\\..\\dist"};
#    else
    std::array<std::string, 5> possibleResourcePaths = {""};
#    endif
#  else
    std::array<std::string, 7> possibleResourcePaths = {
        "/usr/local/share/openhoi/",
        "/usr/share/openhoi/",
        "",
        "dist",
        "../dist",
        "../../dist",
        "../../../dist"};
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

// Gets the OGRE plugin directory. In case the plugins should be located
// relatively to the executables, an empty path is returned.
filesystem::path FileAccess::getOgrePluginDirectory() {
  // Check if we have already fetched the OGRE plugin directory
  if (FileAccess::ogrePluginDirectory.empty()) {
    // Yeah, this is not thread-safe..

#if defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
    Dl_info dlInfo;
    dladdr((boost::format("libOgreMain.so.%d.%d.%d") % OGRE_VERSION_MAJOR %
               OGRE_VERSION_MINOR % OGRE_VERSION_PATCH).c_str(),
           &dlInfo);

    filesystem::path libDir = filesystem::path(dlInfo.dli_fname).parent_path();
    // TODO: Checks
    FileAccess::ogrePluginDirectory = libDir;
#else
    // Use an empty path because all libs are stored in the same directory than
    // our executables
    FileAccess::ogrePluginDirectory = filesystem::path();
#endif
  }

  return FileAccess::ogrePluginDirectory;
}

}  // namespace openhoi