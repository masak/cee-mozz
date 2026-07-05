#ifndef CRASH_H
#define CRASH_H

typedef enum {
    CRASH_OK = 0,
    CRASH_INVALID_TAG,
} CrashReason;

void vm_crash(CrashReason reason);

#endif

