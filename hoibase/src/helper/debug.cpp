// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/helper/debug.hpp"
#include "hoibase/helper/os.hpp"

#include <OgreException.h>
#include <boost/stacktrace.hpp>

namespace openhoi {

// Pretty print exception with stacktrace
std::string Debug::prettyPrintException(const std::exception* ex) {
  std::stringstream message;

  // Add exception message
  if (const Ogre::Exception* oe = dynamic_cast<const Ogre::Exception*>(ex)) {
    message << oe->getFullDescription();
  } else {
    message << ex->what();
  }

  // Add stack trace
  message << std::endl
          << std::endl
          << "Stack trace:" << std::endl
          << boost::stacktrace::stacktrace();

  // Return pretty printed exception with stacktrace
  return message.str();
}

}  // namespace openhoi
