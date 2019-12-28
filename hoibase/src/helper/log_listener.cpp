// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/helper/log_listener.hpp"
#include "hoibase/openhoi.hpp"

#ifndef OPENHOI_OS_WINDOWS
#  include <syslog.h>
#endif

namespace openhoi {

// Log listener constructor
LogListener::LogListener(bool gameClientExecutable /* = true */) {
#ifndef OPENHOI_OS_WINDOWS
  // Set syslog log mask
  setlogmask(LOG_UPTO(LOG_NOTICE));

  // Open syslog
  const char* logName;
  if (gameClientExecutable)
    logName = OPENHOI_GAME_NAME;
  else
    logName = OPENHOI_GAME_NAME "-server";
  openlog(logName, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
#endif
}

// Log listener destructor
LogListener::~LogListener() {
#ifndef OPENHOI_OS_WINDOWS
  // Close syslog
  closelog();
#endif
}

// This is called whenever the log receives a message and is about to write it
// out
void LogListener::messageLogged(const Ogre::String& message,
                                Ogre::LogMessageLevel lml, bool maskDebug,
                                const Ogre::String& logName,
                                bool& skipThisMessage) {
#ifndef OPENHOI_OS_WINDOWS
  // As we are not writing to a simple file on non-Windows systems but to
  // syslog, we have to convert the OGRE log message level to make it compatible
  // with syslog. Then we can log the message accordingly.

  int logLevel;
  switch (lml) {
    case Ogre::LogMessageLevel::LML_NORMAL:
      logLevel = LOG_INFO;
      break;
    case Ogre::LogMessageLevel::LML_WARNING:
      logLevel = LOG_WARNING;
      break;
    case Ogre::LogMessageLevel::LML_CRITICAL:
      logLevel = LOG_ERR;
      break;
    default:
      logLevel = LOG_NOTICE;
  }

  syslog(logLevel, message.c_str());
#endif
}

}  // namespace openhoi