// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <mutex>

// Synchronization for thread-safe access
#define OPENHOI_SYNCHRONIZED(m) \
  for (std::unique_lock<std::recursive_mutex> lk(m); lk; lk.unlock())

// EOF