//
// Created by Porter on 2019-03-01.
//

#include "console.h"
#include "debug.h"

void Console::debugPrint(const char * format, ...) {
    if (Debug::getIsDebugEnabled()) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

