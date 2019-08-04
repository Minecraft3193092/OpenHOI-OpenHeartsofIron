// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"

#include <hoibase/helper/debug.hpp>
#include <hoibase/helper/os.hpp>
#include <hoibase/openhoi.hpp>

#include <iostream>

#ifndef OPENHOI_OS_WINDOWS
#  include <unistd.h>
#endif

using namespace openhoi;

// Main entry point of program
#ifdef OPENHOI_OS_WINDOWS
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR strCmdLine,
                   INT nCmdShow)
#else
int main(int argc, const char* argv[])
#endif
{
  int exitStatus = EXIT_FAILURE;

#ifndef OPENHOI_OS_WINDOWS
  // Don't allow people to run openhoi with root permissions because we will
  // access the user's home directory
  if (geteuid() == 0) {
    std::cerr << "Please do not run the game server with root permissions!"
              << std::endl;
    exit(exitStatus);
  }
#endif

  try {
    // Initialize game manager
    GameManager& gameManager = GameManager::getInstance();

    // Run game
    gameManager.run();

    exitStatus = EXIT_SUCCESS;
  } catch (const std::exception& e) {
    // Handle exception
    std::string exception = Debug::PrettyPrintException(&e);
#ifdef OPENHOI_OS_WINDOWS
    MessageBox(NULL, exception.c_str(), "An exception has occured",
               MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cerr << "An exception has occured: " << exception << std::endl;
#endif
  }

  // Terminate program
  exit(exitStatus);
}