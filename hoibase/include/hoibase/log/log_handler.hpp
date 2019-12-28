// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <hoibase/helper/library.hpp>

#include <OgreLog.h>

namespace openhoi {

// OGRE log handler
class LogHandler {
 public:
  // This is called whenever the log receives a message and is about to write it
  // out
  OPENHOI_LIB_EXPORT virtual void messageLogged(std::string message,
                                                Ogre::LogMessageLevel lml) = 0;
};

}  // namespace openhoi