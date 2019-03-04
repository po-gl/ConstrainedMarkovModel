//
// Created by Porter on 2019-03-01.
//

#ifndef MARKOV_DEBUG_H
#define MARKOV_DEBUG_H

class Debug {
private:
    static bool isDebugEnabled;

public:
    static void setDebugEnabled(bool value);
    static bool getIsDebugEnabled();
};

#endif //MARKOV_DEBUG_H
