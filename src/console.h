//
// Created by Porter on 2019-03-01.
//

#ifndef MARKOV_CONSOLE_H
#define MARKOV_CONSOLE_H

#include <string>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

class Console {
public:
    static void debugPrint(const char *format, ...);
    static void printHelp();
};

#endif //MARKOV_CONSOLE_H
