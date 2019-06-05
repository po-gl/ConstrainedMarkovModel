#ifndef MAIN_H
#define MAIN_H

#include "options.h"
#include "markov.h"
#include "mnemonicmarkov.h"

namespace Main {

  MarkovModel trainMarkov(Options options);

  MnemonicMarkovModel trainConstrainedMarkov(Options options, MarkovModel markovModel, string constraint);

  vector<vector<string> > generateSentences(Options options, MnemonicMarkovModel model);

  void printMarkovDebugInfo(Options options, MnemonicMarkovModel model);
}

#endif