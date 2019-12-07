// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

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
void sigintHandler(int sig) {
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
  HANDLE mutexHandle = CreateMutex(NULL, FALSE, TEXT(OPENHOI_UNIQUE_HANDLE));
  DWORD lastError = GetLastError();
  if (lastError == ERROR_ALREADY_EXISTS) {
    MessageBox(NULL, "You cannot run " OPENHOI_GAME_NAME "twice!",
               OPENHOI_GAME_NAME, MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
    exit(exitStatus);
  }
#else
  // Don't allow people to run openhoi with root permissions because we will
  // access the user's home directory
  if (geteuid() == 0) {
    std::cerr << "Please do not run " OPENHOI_GAME_NAME
                 " with root permissions!"
              << std::endl;
    exit(exitStatus);
  }

  // Enforce the game to run only once
  filesystem::path tempDirectory = FileAccess::getTempDirectory();
  filesystem::path lockFile = tempDirectory / OPENHOI_UNIQUE_HANDLE;
  lockFilePath = lockFile.c_str();
  struct sigaction sigAction;
  int mutexFd;
  mutexFd = open(lockFilePath, O_CREAT | O_EXCL);
  if (mutexFd < 0) {
    std::cerr << "You cannot run " OPENHOI_GAME_NAME "twice!" << std::endl;
    exit(exitStatus);
  }
  sigAction.sa_handler = sigintHandler;
  sigemptyset(&sigAction.sa_mask);
  sigAction.sa_flags = 0;
  sigaction(SIGINT, &sigAction, NULL);
#endif

  try {
    // Initialize game manager
    GameManager& gameManager = GameManager::getInstance();

    // Run game
    gameManager.run();

    exitStatus = EXIT_SUCCESS;
  } catch (const std::exception& e) {
    // Handle exception
    std::string exception = Debug::prettyPrintException(&e);
#ifdef OPENHOI_OS_WINDOWS
    MessageBox(NULL, exception.c_str(), "An exception has occured",
               MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cerr << "An exception has occured:" << std::endl
              << exception << std::endl;
#endif
  }

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