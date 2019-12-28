// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"
#include "hoibase/helper/os.hpp"
#include "log_handler.hpp"

#include <OgreLog.h>

namespace openhoi {

// OGRE log listener
class LogListener final : public Ogre::LogListener {
 public:
  // Log listener constructor
  OPENHOI_LIB_EXPORT LogListener(bool gameClientExecutable = true);

  // Log listener destructor
  ~LogListener();

  // This is called whenever the log receives a message and is about to write it
  // out
  OPENHOI_LIB_EXPORT void messageLogged(const Ogre::String& message,
                                        Ogre::LogMessageLevel lml,
                                        bool maskDebug,
                                        const Ogre::String& logName,
                                        bool& skipThisMessage);

  // Register a new event handler that will be called for each logged message
  OPENHOI_LIB_EXPORT void registerLogHandler(LogHandler* logHandler);

 private:
  std::vector<LogHandler*> logHandlers;
};

}  // namespace openhoi