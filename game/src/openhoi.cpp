// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include <boost/locale.hpp>
#include <hoibase/file/file_access.hpp>
#include <hoibase/helper/debug.hpp>
#include <hoibase/helper/os.hpp>
#include <hoibase/openhoi.hpp>
#include <iostream>

#include "game_manager.hpp"

#ifndef OPENHOI_OS_WINDOWS
#  include <fcntl.h>
#  include <signal.h>
#  include <unistd.h>
#endif

// Unique handle name to prevent game to run twice
#define OPENHOI_UNIQUE_HANDLE "openhoi-EE124BFD-D6B8-4CE9-BBB6-B2079D9747DA"

using namespace openhoi;

#ifndef OPENHOI_OS_WINDOWS
const char* lockFilePath;

// Handle SIGINT (release lock file)
void sigintHandler(int /*nSignum*/, siginfo_t* /*si*/, void* /*vcontext*/) {
  unlink(lockFilePath);
  _exit(EXIT_FAILURE);  // exit() is not safe in a signal handler, the we use
                        // _exit()
}
#endif

// Main entry point of program
#ifdef OPENHOI_OS_WINDOWS
#  pragma warning(push)
#  pragma warning(disable : 28251)
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR strCmdLine,
                   INT nCmdShow)
#  pragma warning(pop)
#else
int main(int argc, const char* argv[])
#endif
{
  int exitStatus = EXIT_FAILURE;

#ifdef OPENHOI_OS_WINDOWS
  // Enforce the game to run only once
  SetLastError(0);
  auto mutexHandle = CreateMutex(NULL, FALSE, TEXT(OPENHOI_UNIQUE_HANDLE));
  auto lastError = GetLastError();
  if (mutexHandle == nullptr || lastError == ERROR_ALREADY_EXISTS) {
    MessageBox(
        nullptr,
        (boost::locale::format(boost::locale::translate(
             "You cannot run {1} twice! Please stop the currently running "
             "instance of {1} before you try to run the game executable.")) %
         OPENHOI_GAME_NAME)
            .str()
            .c_str(),
        OPENHOI_GAME_NAME, MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
    exit(exitStatus);
  }
#else
  // Don't allow people to run openhoi with root permissions because we will
  // access the user's home directory
  if (geteuid() == 0) {
    std::cerr << boost::locale::format(boost::locale::translate(
                     "Please do not run {1}  with root permissions!")) %
                     OPENHOI_GAME_NAME
              << std::endl;
    exit(exitStatus);
  }

  // Enforce the game to run only once
  filesystem::path tempDirectory = FileAccess::getTempDirectory();
  filesystem::path lockFile = tempDirectory / OPENHOI_UNIQUE_HANDLE;
  lockFilePath = lockFile.c_str();
  int mutexFd;
  mutexFd = open(lockFilePath, O_CREAT | O_EXCL, 0600);
  if (mutexFd < 0) {
    std::cerr
        << boost::locale::format(boost::locale::translate(
               "You cannot run {1} twice! Please stop the currently running "
               "instance of {1} before you try to run the game executable.")) %
               OPENHOI_GAME_NAME
        << std::endl;
    exit(exitStatus);
  }
  struct sigaction sigAction;
  memset(&sigAction, 0, sizeof(struct sigaction));
  sigAction.sa_flags = SA_SIGINFO;
  sigAction.sa_sigaction = sigintHandler;
  sigaction(SIGINT, &sigAction, NULL);
  sigaction(SIGTERM, &sigAction, NULL);
  sigaction(SIGSEGV, &sigAction, NULL);
#endif

  try {
    // Initialize locale generator
    boost::locale::generator localeGenerator;
    std::string i18nDirectory =
        (FileAccess::getAssetRootDirectory() / "i18n").string();
    localeGenerator.add_messages_path(i18nDirectory);
    localeGenerator.add_messages_domain("openhoi");
    // Create system default locale
    std::locale loc = localeGenerator("");
    // Make it system global
    loc = std::locale::global(loc);
    // Set as default locale for console output
    loc = std::cout.imbue(loc);

    // Initialize game manager
    GameManager& gameManager = GameManager::getInstance();

    // Run game
    gameManager.run();

    exitStatus = EXIT_SUCCESS;
  } catch (const std::exception& e) {
    // Handle exception
    std::string exception = Debug::prettyPrintException(&e);
#ifdef OPENHOI_OS_WINDOWS
    MessageBox(
        nullptr, exception.c_str(),
        boost::locale::translate("An exception has occured").str().c_str(),
        MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cerr << boost::locale::translate("An exception has occured") << ":"
              << std::endl
              << exception << std::endl;
#endif
  }

  // Release mutex / lock file
#ifdef OPENHOI_OS_WINDOWS
  ReleaseMutex(mutexHandle);
  CloseHandle(mutexHandle);
  mutexHandle = nullptr;
#else
  unlink(lockFilePath);
  close(mutexFd);
#endif

  // Terminate program
  exit(exitStatus);
}