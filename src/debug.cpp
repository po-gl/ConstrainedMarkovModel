//
// Created by Porter on 2019-03-01.
//

#include "debug.h"

bool Debug::isDebugEnabled = false;
bool Debug::isDeepDebugEnabled = false;

void Debug::setDebugEnabled(bool value) {
    Debug::isDebugEnabled = value;
}

void Debug::setDeepDebugEnabled(bool value) {
    Debug::isDeepDebugEnabled = value;
}

bool Debug::getIsDebugEnabled() {
    return Debug::isDebugEnabled;
}

bool Debug::getIsDeepDebugEnabled() {
    return Debug::isDeepDebugEnabled;
}
