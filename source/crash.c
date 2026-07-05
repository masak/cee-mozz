#include <stdlib.h>

#include "../include/crash.h"

_Noreturn void vm_crash(CrashReason reason) {
    (void)reason;
    abort();
}

