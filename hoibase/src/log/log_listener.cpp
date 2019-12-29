// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/log/log_listener.hpp"
#include "hoibase/openhoi.hpp"

namespace openhoi {

// This is called whenever the log receives a message and is about to write it
// out
void LogListener::messageLogged(const Ogre::String& message,
                                Ogre::LogMessageLevel lml, bool /*maskDebug*/,
                                const Ogre::String& /*logName*/,
                                bool& /*skipThisMessage*/) {
  // Send event to all registered handlers
  for (auto const& logHandler : logHandlers) {
    logHandler->messageLogged(message, lml);
  }
}

// Register a new event handler that will be called for each logged message
void LogListener::registerLogHandler(LogHandler* logHandler) {
  logHandlers.push_back(logHandler);
}

}  // namespace openhoi