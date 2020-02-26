//
// Created by Porter on 2019-03-01.
//

#ifndef MARKOV_DEBUG_H
#define MARKOV_DEBUG_H

class Debug {
private:
    static bool isDebugEnabled;
    static bool isDeepDebugEnabled; // Slow

public:
    static void setDebugEnabled(bool value);
    static void setDeepDebugEnabled(bool value);
    static bool getIsDebugEnabled();
    static bool getIsDeepDebugEnabled();
};

#endif //MARKOV_DEBUG_H
