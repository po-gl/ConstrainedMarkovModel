#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>

#include "mnemonicmarkov.h"
#include "../console.h"
#include "../utils.h"

using namespace std;

MnemonicMarkovModel::MnemonicMarkovModel() {
  // Initialize random
  random_device rd;
  randGenerator = mt19937(rd());
  randDistribution = uniform_real_distribution<double>(0.0, 1.0);
}


MnemonicMarkovModel::MnemonicMarkovModel(MarkovModel markovModel, string constraint, Options options) {
  // Initialize random
  random_device rd;
  randGenerator = mt19937(rd());
  randDistribution = uniform_real_distribution<double>(0.0, 1.0);

  time_t startTime; // used for debug timing

  // Train model (Apply constraints)
  startTime = clock();
  this->train(markovModel, Utils::splitAndLower(constraint, "\\s,"));
  Console::debugPrint("%-35s: %f\n", "Elapsed Training Time", (float)(clock() - startTime) / CLOCKS_PER_SEC);
}


void MnemonicMarkovModel::applyConstraints(vector<string> constraintSequence) {

  // TODO: Separate these constraints into their own objects or functions to test

  // Constraints:
  // + First letter matches constraint string
  // + Word cannot be a "stop word"
  // - Words must be certain length
  // - Every other word must be certain length (last must be long)
  // - Last word must proceed an <<END>>
  // - Words must match first part of constraint if possible (relaxed)

  // Assume only Markov order 1 for now (TODO: implement more Markov orders)
  int markovOrder = 1;
  // int markovOrder = this->getMarkovOrder();

  bool firstCharMatches;
  bool wordLengthMet;
  bool proceedsEndSuitable;
  bool proceedsEnd = false;

  bool isNotStopWord;

  int wordLen = 5;

  int removedNodesCount = 0;
  int totalNodesCount = 0;

  // ** Parse constraints

  for (int i = 0; i < constraintSequence.size(); i+=markovOrder) {
    int m = ceil((double)i / markovOrder); // iterator to use accounting for Markov order

    // Wild character constraint
    if (constraintSequence[i] == "*") {
      continue;
    }

    // Where word is a map from the word (first) to all possible proceeding words (second)
    for (auto word = transitionMatrices[m].begin(); word != transitionMatrices[m].end();) {

      // vector<string> wordsInLookahead = Utils::split(word->first, "\\s");  // split on spaces
      vector<string> wordsInLookahead = { word->first }; // Assume only Markov order 1 so don't perform expensive split
      auto testFirst = word->first;
      auto testSecond = word->second;

      if (wordsInLookahead.size() < markovOrder) {
        word = transitionMatrices[m].erase(word);
        removedNodesCount++;
        continue;
      }

      firstCharMatches = true;
      wordLengthMet = true;
      isNotStopWord = true;
      for (int j = i, k = 0; j < constraintSequence.size() && k < wordsInLookahead.size(); j++, k++) {

        if (constraintSequence[j][0] != wordsInLookahead[k][0]) {
          firstCharMatches = false;
        }

        if (wordsInLookahead[k].size() < wordLen) {
          wordLengthMet = false;
        }

        if (Utils::isStopWord(wordsInLookahead[k])) {
          isNotStopWord = false;
        }
      }

      proceedsEndSuitable = (i == constraintSequence.size() - 1);
      // Set to true if <<END>> is found in the node's proceeding map
      if (proceedsEndSuitable) {  // constraint for end node
        proceedsEnd = (word->second.find(END) != word->second.end());
      }

      // Remove nodes that don't satisfy the constraint
      // (letter in constraint string == first letter of word)
      // if (!firstCharMatches || !wordLengthMet || (!proceedsEnd && proceedsEndSuitable)) {
      // if (!firstCharMatches || !isNotStopWord || (!proceedsEnd && proceedsEndSuitable)) {
      if (!firstCharMatches || !isNotStopWord) {
      // if (!firstCharMatches || !wordLengthMet) {
      // if (!firstCharMatches) {
        this->removedNodesbyConstraint[m].push_back(word->first);  // Save removed nodes
        word = this->transitionMatrices[m].erase(word);
        removedNodesCount++;
      } else {
        word++;
      }
      totalNodesCount++;
    }
  }

  Console::debugPrint("%-35s: %d / %d\n", "Removed nodes", removedNodesCount, totalNodesCount);
}
