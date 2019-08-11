// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/helper/os.hpp"

namespace openhoi {

// Checks if we are currently running inside a virtual machine
bool OS::isRunningInVirtualMachine() {
#ifdef OPENHOI_OS_WINDOWS
  UINT64 time1 = __rdtsc();
  UINT64 time2 = __rdtsc();
  return ((time2 - time1) > 500);
#else
  unsigned int time1 = 0;
  unsigned int time2 = 0;
  __asm
  {
        RDTSC
        MOV time1, EAX
        RDTSC
        MOV time2, EAX
  }
  return ((time2 - time1) > 500);
#endif
}

}  // namespace openhoi
