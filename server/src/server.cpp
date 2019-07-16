// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include <hoibase/file/file_access.hpp>
#include <hoibase/helper/os.hpp>
#include <hoibase/openhoi.hpp>

#include <boost/program_options.hpp>
#include <iostream>

#ifndef OPENHOI_OS_WINDOWS
#  include <unistd.h>
#endif

#define TITLE OPENHOI_GAME_NAME " server v" OPENHOI_GAME_VERSION

namespace po = boost::program_options;

// Main entry point of program
int main(int argc, const char* argv[]) {
#ifndef OPENHOI_OS_WINDOWS
  // Don't allow people to run openhoi with root permissions because we will
  // access the user's home directory
  if (geteuid() == 0) {
    std::cerr << "Please do not run the server with root permissions!"
              << std::endl;
    exit(EXIT_SUCCESS);
  }

  // Set console title
  std::cout << "\033]0;" << TITLE << "\007";
#else
  // Set console title
  SetConsoleTitle(TITLE);
#endif

  // Print out header
  std::cout << TITLE << std::endl;
  std::cout << "Copyright (c) the openhoi authors" << std::endl;
  std::cout << OPENHOI_GIT_URL << std::endl << std::endl;

  // Parse program options
  std::filesystem::path configFile;
  po::options_description desc("Options");
  desc.add_options()("help", "Produce help message")(
      "config",
      po::value<std::filesystem::path>(&configFile)
          ->default_value(openhoi::FileAccess::GetUserGameConfigDirectory() /
                          "server.yml"),
      "Path to config file");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(EXIT_SUCCESS);
  }

  while (true) {
    // xxx
  }

  // Terminate program
  exit(EXIT_SUCCESS);
}