#include <stdlib.h>

#include "../include/crash.h"

void vm_crash(CrashReason reason) {
    (void)reason;
    abort();
}

