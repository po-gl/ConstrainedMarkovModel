#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>

#include "mnemonicmarkov.h"
#include "utils.h"

using namespace std;

MnemonicMarkovModel::MnemonicMarkovModel() {
  // Initialize random
  random_device rd;
  randGenerator = mt19937(rd());
  randDistribution = uniform_real_distribution<double>(0.0, 1.0);
}


void MnemonicMarkovModel::applyConstraints(vector<string> constraintSentence, int markovOrder) {

  // Constraints:
  // + First letter matches constraint string
  // + Words must be certain length
  // + Every other word must be certain length (last must be long)
  // + Last word must proceed an <<END>>
  // - Words must match first part of constraint if possible (relaxed)

  bool firstCharMatches;
  bool wordLengthMet;
  bool proceedsEndSuitable;
  bool proceedsEnd = false;

  int wordLen = 5;

  int debugCount = 0;
  int debugCountTot = 0;

  for (int i = 0; i < constraintSentence.size(); i+=markovOrder) {
    int m = ceil((double)i / markovOrder);
    string constraintWord = constraintSentence[i];
    printf("DEBUG:: constraintWord: %s\n", constraintWord.c_str());



    // auto matrix = transitionMatrices[i];
    for (auto node = transitionMatrices[m].begin(); node != transitionMatrices[m].end();) {
      vector<string> nodeSequence = Utils::split(node->first, "\\s");  // split on spaces
      auto testfirst = node->first;
      auto testsecond = node->second;

      if (nodeSequence.size() < markovOrder) {
        node = transitionMatrices[m].erase(node);
        debugCount++;
        continue;
      }

      firstCharMatches = true;
      for (int j = i, k = 0; j < constraintSentence.size() && k < nodeSequence.size(); j++, k++) {
        if (constraintSentence[j][0] != nodeSequence[k][0]) {
          firstCharMatches = false;
          break;
        }
      }


      wordLengthMet = true;
      for (int j = i, k = 0; j < constraintSentence.size() && k < nodeSequence.size(); j++, k++) {
        if (nodeSequence[k].size() < wordLen) {
          wordLengthMet = false;
          break;
        }
      }

      proceedsEndSuitable = (i == constraintSentence.size() - 1);
      // Set to true if <<END>> is found in the node's proceeding map
      if (proceedsEndSuitable) {  // constraint for end node
        proceedsEnd = (node->second.find(END) != node->second.end());
      }

      // Remove nodes that don't satisfy the constraint
      // (letter in constraint string == first letter of word)
      if (!firstCharMatches || !wordLengthMet || (!proceedsEnd && proceedsEndSuitable)) {
//      if (!firstCharMatches || !wordLengthMet) {
//      if (!firstCharMatches) {
        node = transitionMatrices[m].erase(node);
        debugCount++;
      } else {
        node++;
      }
      debugCountTot++;
    }
  }

  printf("DEBUG:: Removed nodes: %d / %d\n", debugCount, debugCountTot); // 534472/549018
}
