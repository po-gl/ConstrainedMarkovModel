//
// Created by Porter on 2019-03-01.
//

#include "debug.h"

bool Debug::isDebugEnabled = false;

void Debug::setDebugEnabled(bool value) {
    Debug::isDebugEnabled = value;
}

bool Debug::getIsDebugEnabled() {
    return Debug::isDebugEnabled;
}

